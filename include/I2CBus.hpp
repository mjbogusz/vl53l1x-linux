#pragma once

#include <mutex>
#include <string>

class I2CBus {
public:
	explicit I2CBus(const std::string& devicePath);
	~I2CBus();

	void write8(uint8_t address, uint16_t registerAddr, uint8_t data);

	void write16(uint8_t address, uint16_t registerAddr, uint16_t data);

	void write32(uint8_t address, uint16_t registerAddr, uint32_t data);

	uint8_t read8(uint8_t address, uint16_t registerAddr);

	uint16_t read16(uint8_t address, uint16_t registerAddr);

	uint32_t read32(uint8_t address, uint16_t registerAddr);

	void write(uint8_t address, uint16_t registerAddr, const uint8_t* data, uint16_t length);

	void read(uint8_t address, uint16_t registerAddr, uint8_t* data, uint16_t length);

private:
	int fileDescriptor;
	std::mutex lock;
};
