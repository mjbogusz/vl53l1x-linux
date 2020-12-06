#include "I2CBus.hpp"

// std::memcpy, strerror()
#include <cstring>
// open(), O_RDWR
#include <fcntl.h>
// close()
#include <unistd.h>
// I2C_SLAVE
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
// ioctl()
#include <sys/ioctl.h>

I2CBus::I2CBus(const std::string& devicePath) {
	this->fileDescriptor = open(devicePath.c_str(), O_RDWR);
	if (this->fileDescriptor < 0) {
		throw std::runtime_error(std::string("Error opening i2c device file: ") + strerror(errno));
	}
}

I2CBus::~I2CBus() {
	if (this->fileDescriptor >= 0) {
		close(this->fileDescriptor);
	}
}

void I2CBus::write8(uint8_t address, uint16_t registerAddr, uint8_t data) {
	this->write(address, registerAddr, &data, 1);
}

void I2CBus::write16(uint8_t address, uint16_t registerAddr, uint16_t data) {
	uint8_t buffer[2];

	buffer[0] = data >> 8;
	buffer[1] = data & 0x00FF;
	this->write(address, registerAddr, (uint8_t*)buffer, 2);
}

void I2CBus::write32(uint8_t address, uint16_t registerAddr, uint32_t data) {
	uint8_t buffer[4];

	buffer[0] = (data >> 24) & 0xFF;
	buffer[1] = (data >> 16) & 0xFF;
	buffer[2] = (data >> 8) & 0xFF;
	buffer[3] = (data >> 0) & 0xFF;
	this->write(address, registerAddr, (uint8_t*)buffer, 4);
}

uint8_t I2CBus::read8(uint8_t address, uint16_t registerAddr) {
	uint8_t data = 0;
	this->read(address, registerAddr, &data, 1);
	return data;
}

uint16_t I2CBus::read16(uint8_t address, uint16_t registerAddr) {
	uint8_t data[2] = {0, 0};
	this->read(address, registerAddr, data, 2);
	return (static_cast<uint16_t>(data[0]) << 8)
		+ static_cast<uint16_t>(data[1]);
}

uint32_t I2CBus::read32(uint8_t address, uint16_t registerAddr) {
	uint8_t data[4] = {0, 0, 0, 0};

	this->read(address, registerAddr, data, 4);
	return (static_cast<uint32_t>(data[0]) << 24)
		+ (static_cast<uint32_t>(data[1]) << 16)
		+ (static_cast<uint32_t>(data[2]) << 8)
		+ static_cast<uint32_t>(data[3]);
}

void I2CBus::write(uint8_t address, uint16_t registerAddr, const uint8_t* data, uint16_t length) {
	std::lock_guard<std::mutex> guard(this->lock);

	if (this->fileDescriptor < 0) {
		throw std::runtime_error("I2C device file not opened");
	}

	uint8_t writeBuffer[length + 2];
	writeBuffer[0] = (registerAddr >> 8) & 0xFF;
	writeBuffer[1] = registerAddr & 0xFF;
	for (int i = 0; i < length; ++i) {
		writeBuffer[i + 2] = data[i];
	}
	i2c_msg msgs[1] = {{
		.addr = address,
		.flags = 0,
		.len = static_cast<uint16_t>(length + 2),
		.buf = writeBuffer,
	}};
	i2c_rdwr_ioctl_data msgset = {
		.msgs = msgs,
		.nmsgs = 1,
	};

	int result = ioctl(fileDescriptor, I2C_RDWR, &msgset);
	if (result < 0) {
		throw std::runtime_error("I2C write error: " + std::to_string(result));
	}
}

void I2CBus::read(uint8_t address, uint16_t registerAddr, uint8_t* data, uint16_t length) {
	std::lock_guard<std::mutex> guard(this->lock);

	if (this->fileDescriptor < 0) {
		throw std::runtime_error("I2C device file not opened");
	}

	uint8_t writeBuffer[2] = {};
	writeBuffer[0] = (registerAddr >> 8) & 0xFF;
	writeBuffer[1] = registerAddr & 0xFF;
	i2c_msg registerSelectMsgs[1] = {{
		.addr = address,
		.flags = 0,
		.len = 2,
		.buf = writeBuffer
	}};
	i2c_rdwr_ioctl_data registerSelectMsgSet = {
		.msgs = registerSelectMsgs,
		.nmsgs = 1,
	};
	i2c_msg registerReadMsgs[1] = {{
		.addr = address,
		.flags = I2C_M_RD,
		.len = length,
		.buf = data,
	}};
	i2c_rdwr_ioctl_data registerReadMsgSet = {
		.msgs = registerReadMsgs,
		.nmsgs = 1,
	};

	int result = ioctl(fileDescriptor, I2C_RDWR, &registerSelectMsgSet);
	int result2 = ioctl(fileDescriptor, I2C_RDWR, &registerReadMsgSet);
	if (result < 0) {
		throw std::runtime_error("I2C read register select error: " + std::to_string(result));
	}
	if (result2 < 0) {
		throw std::runtime_error("I2C read error: " + std::to_string(result));
	}
}
