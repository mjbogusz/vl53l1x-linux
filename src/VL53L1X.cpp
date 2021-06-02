#include "VL53L1X.hpp"

#include <cstring>
#include <fstream>
#include <thread>
#include <utility>

using namespace std::chrono_literals;

VL53L1X::VL53L1X(
	I2CBus& i2cBus,
	std::string gpioPath,
	uint8_t address,
	std::chrono::milliseconds timeout
):
	i2cBus(i2cBus),
	gpioPath(std::move(gpioPath)),
	address(address),
	timeout(timeout),
	interruptPolarity(0),
	decimal(0.0) {}

void VL53L1X::initialize() {
	// TODO: soft-restart, GPIO restart (?)

	// Write the default configuration, registers 0x2D to 0x87
	for (uint8_t configAddr = 0x2D; configAddr <= 0x87; configAddr++) {
		this->i2cBus.write8(this->address, configAddr, VL53L1X::DEFAULT_CONFIGURATION[configAddr - 0x2D]);
	}
	this->startRanging();
	while (!this->isDataReady()) {
		std::this_thread::sleep_for(500ms);
	}
	this->clearInterrupt();
	this->stopRanging();
	// two bounds VHV
	this->i2cBus.write8(this->address, VHV_CONFIG_TIMEOUT_MACROP_LOOP_BOUND, 0x09);
	this->i2cBus.write8(this->address, VHV_CONFIG_INIT, 0);

	this->interruptPolarity = !((this->i2cBus.read8(this->address, GPIO_HV_MUX_CTRL) & 0x10) >> 4);
}

void VL53L1X::powerOn() {
	this->setGPIO('1');

	// as per the docs (section 2.6.1, page 12), boot duration is 1.2ms max - wait 2ms just to be sure
	std::this_thread::sleep_for(2ms);
}

void VL53L1X::powerOff() {
	this->setGPIO('0');
}

void VL53L1X::setAddress(uint8_t newAddress) {
	this->i2cBus.write8(this->address, I2C_SLAVE_DEVICE_ADDRESS, newAddress & 0x7F);
	this->address = newAddress;
}

void VL53L1X::clearInterrupt() {
	this->i2cBus.write8(this->address, SYSTEM_INTERRUPT_CLEAR, 0x01);
}

void VL53L1X::startRanging() {
	this->i2cBus.write8(this->address, SYSTEM_MODE_START, 0x40);
}

void VL53L1X::stopRanging() {
	this->i2cBus.write8(this->address, SYSTEM_MODE_START, 0x00);
}

bool VL53L1X::isDataReady() {
	return (this->i2cBus.read8(this->address, GPIO_TIO_HV_STATUS) & 0x01) == this->interruptPolarity;
}

void VL53L1X::setTimingBudget(VL53L1X::TimingBudget timingBudget) {
	auto distanceMode = this->getDistanceMode();
	if (distanceMode == VL53L1X::DISTANCE_MODE_SHORT) {
		// Short DistanceMode
		switch (timingBudget) {
			case TIMING_BUDGET_15_MS:
				// only available in short distance mode
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x001D);
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x0027);
				break;
			case TIMING_BUDGET_20_MS:
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x0051);
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x006E);
				break;
			case TIMING_BUDGET_33_MS:
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x00D6);
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x006E);
				break;
			case TIMING_BUDGET_50_MS:
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x01AE);
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x01E8);
				break;
			case TIMING_BUDGET_100_MS:
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x02E1);
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x0388);
				break;
			case TIMING_BUDGET_200_MS:
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x03E1);
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x0496);
				break;
			case TIMING_BUDGET_500_MS:
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x0591);
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x05C1);
				break;
			default:
				break;
		}
	} else if (distanceMode == VL53L1X::DISTANCE_MODE_LONG) {
		switch (timingBudget) {
			case TIMING_BUDGET_20_MS:
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x001E);
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x0022);
				break;
			case TIMING_BUDGET_33_MS:
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x0060);
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x006E);
				break;
			case TIMING_BUDGET_50_MS:
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x00AD);
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x00C6);
				break;
			case TIMING_BUDGET_100_MS:
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x01CC);
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x01EA);
				break;
			case TIMING_BUDGET_200_MS:
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x02D9);
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x02F8);
				break;
			case TIMING_BUDGET_500_MS:
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x048F);
				this->i2cBus.write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x04A4);
				break;
			default:
				break;
		}
	}
}

VL53L1X::TimingBudget VL53L1X::getTimingBudget() {
	uint16_t configValue = this->i2cBus.read16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI);
	switch (configValue) {
		case 0x0051:
		case 0x001E:
			return TIMING_BUDGET_20_MS;

		case 0x00D6:
		case 0x0060:
			return TIMING_BUDGET_33_MS;

		case 0x1AE:
		case 0x00AD:
			return TIMING_BUDGET_50_MS;

		case 0x02E1:
		case 0x01CC:
			return TIMING_BUDGET_100_MS;

		case 0x03E1:
		case 0x02D9:
			return TIMING_BUDGET_200_MS;

		case 0x0591:
		case 0x048F:
			return TIMING_BUDGET_500_MS;

		default:
			return TIMING_BUDGET_20_MS;
	}
}

void VL53L1X::setDistanceMode(VL53L1X::DistanceMode mode) {
	VL53L1X::TimingBudget budget = this->getTimingBudget();

	switch (mode) {
		case VL53L1X::DISTANCE_MODE_SHORT:
			this->i2cBus.write8(this->address, PHASECAL_CONFIG_TIMEOUT_MACROP, 0x14);
			this->i2cBus.write8(this->address, RANGE_CONFIG_VCSEL_PERIOD_A, 0x07);
			this->i2cBus.write8(this->address, RANGE_CONFIG_VCSEL_PERIOD_B, 0x05);
			this->i2cBus.write8(this->address, RANGE_CONFIG_VALID_PHASE_HIGH, 0x38);
			this->i2cBus.write16(this->address, SD_CONFIG_WOI_SD0, 0x0705);
			this->i2cBus.write16(this->address, SD_CONFIG_INITIAL_PHASE_SD0, 0x0606);
			break;
		case VL53L1X::DISTANCE_MODE_LONG:
			this->i2cBus.write8(this->address, PHASECAL_CONFIG_TIMEOUT_MACROP, 0x0A);
			this->i2cBus.write8(this->address, RANGE_CONFIG_VCSEL_PERIOD_A, 0x0F);
			this->i2cBus.write8(this->address, RANGE_CONFIG_VCSEL_PERIOD_B, 0x0D);
			this->i2cBus.write8(this->address, RANGE_CONFIG_VALID_PHASE_HIGH, 0xB8);
			this->i2cBus.write16(this->address, SD_CONFIG_WOI_SD0, 0x0F0D);
			this->i2cBus.write16(this->address, SD_CONFIG_INITIAL_PHASE_SD0, 0x0E0E);
			break;
		default:
			break;
	}
	this->setTimingBudget(budget);
}

VL53L1X::DistanceMode VL53L1X::getDistanceMode() {
	uint8_t configValue = this->i2cBus.read8(this->address, PHASECAL_CONFIG_TIMEOUT_MACROP);

	if (configValue == 0x14) {
		return VL53L1X::DISTANCE_MODE_SHORT;
	}
	if (configValue == 0x0A) {
		return VL53L1X::DISTANCE_MODE_LONG;
	}
	return VL53L1X::DISTANCE_MODE_UNKNOWN;
}

void VL53L1X::setInterMeasurementPeriod(uint16_t period) {
	uint16_t clockPLL = 0x03FF & this->i2cBus.read16(this->address, VL53L1_RESULT_OSC_CALIBRATE_VAL);
	this->i2cBus.write32(this->address, VL53L1_SYSTEM_INTERMEASUREMENT_PERIOD, (uint32_t)(clockPLL * period * 1.075));

	double data = (clockPLL * period * 1075) % 1000;
	while (data > 1) {
		data /= 10.0;
	}
	this->decimal = data;
}

uint16_t VL53L1X::getInterMeasurementPeriod() {
	uint16_t clockPLL = 0x03FF & this->i2cBus.read16(this->address, VL53L1_RESULT_OSC_CALIBRATE_VAL);
	uint32_t period = this->i2cBus.read32(this->address, VL53L1_SYSTEM_INTERMEASUREMENT_PERIOD);

	return static_cast<uint16_t>((period + this->decimal) / (clockPLL * 1.075));
}

uint16_t VL53L1X::getDistance() {
	auto startTime = std::chrono::system_clock::now();
	while (true) {
		if (this->isDataReady()) {
			break;
		}
		if (this->timeout.count() && std::chrono::system_clock::now() - startTime > this->timeout) {
			return 65535;
		}
		std::this_thread::sleep_for(5ms);
	}
	uint16_t distance = this->i2cBus->read16Reg16(this->address, VL53L1_RESULT_FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0);
	this->clearInterrupt();
	if (distance > 4000) {
		distance = 16384;
	}
	return distance;
}

uint16_t VL53L1X::getSignalRate() {
	return 8 * this->i2cBus.read16(this->address, VL53L1_RESULT_DSS_ACTUAL_EFFECTIVE_SPADS_SD0);
}

void VL53L1X::setOffset(int16_t offset) {
	this->i2cBus.write16(this->address, ALGO_PART_TO_PART_RANGE_OFFSET_MM, (uint16_t)(offset * 4));
	this->i2cBus.write16(this->address, MM_CONFIG_INNER_OFFSET_MM, 0x0);
	this->i2cBus.write16(this->address, MM_CONFIG_OUTER_OFFSET_MM, 0x0);
}

int16_t VL53L1X::getOffset() {
	uint16_t tmp = this->i2cBus.read16(this->address, ALGO_PART_TO_PART_RANGE_OFFSET_MM);
	// adjust
	if (tmp & 0x1000) {
		tmp |= 0xE000;
	}
	// cast to signed and shift
	return (int16_t)(tmp) / 4;
}

void VL53L1X::setCrosstalk(uint16_t crosstalkValue) {
	this->i2cBus.write16(this->address, ALGO_CROSSTALK_COMPENSATION_X_PLANE_GRADIENT_KCPS, 0x0000);
	this->i2cBus.write16(this->address, ALGO_CROSSTALK_COMPENSATION_Y_PLANE_GRADIENT_KCPS, 0x0000);
	this->i2cBus.write16(this->address, ALGO_CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS, (crosstalkValue << 9) / 1000);
}

uint16_t VL53L1X::getCrosstalk() {
	uint16_t crosstalk = this->i2cBus.read16(this->address, ALGO_CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS);
	return (crosstalk * 1000) >> 9;
}

uint16_t VL53L1X::getDistanceThresholdLow() {
	return this->i2cBus.read16(this->address, SYSTEM_THRESH_LOW);
}

uint16_t VL53L1X::getDistanceThresholdHigh() {
	return this->i2cBus.read16(this->address, SYSTEM_THRESH_HIGH);
}

int8_t VL53L1X::calibrateOffset(uint16_t targetDistance) {
	constexpr uint8_t numberOfMeasurements = 50;

	this->i2cBus.write16(this->address, ALGO_PART_TO_PART_RANGE_OFFSET_MM, 0x0);
	this->i2cBus.write16(this->address, MM_CONFIG_INNER_OFFSET_MM, 0x0);
	this->i2cBus.write16(this->address, MM_CONFIG_OUTER_OFFSET_MM, 0x0);

	this->startRanging();
	int16_t averageDistance = 0;
	for (uint8_t i = 0; i < numberOfMeasurements; i++) {
		while (!this->isDataReady()) {
			std::this_thread::sleep_for(100ms);
		}
		averageDistance = averageDistance + this->getDistance();
		this->clearInterrupt();
	}
	this->stopRanging();

	averageDistance = averageDistance / numberOfMeasurements;
	int16_t offset = targetDistance - averageDistance;
	this->i2cBus.write16(this->address, ALGO_PART_TO_PART_RANGE_OFFSET_MM, offset * 4);
	return offset;
}

int8_t VL53L1X::calibrateCrosstalk(uint16_t targetDistance) {
	constexpr uint8_t numberOfMeasurements = 50;

	this->i2cBus.write16(this->address, ALGO_CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS, 0);

	this->startRanging();
	float averageSignalRate = 0;
	float averageDistance = 0;
	float averageSpadNb = 0;
	for (uint8_t i = 0; i < numberOfMeasurements; i++) {
		while (!this->isDataReady()) {
			std::this_thread::sleep_for(100ms);
		}
		averageSignalRate = averageSignalRate + static_cast<float>(this->getSignalRate());
		averageDistance = averageDistance + static_cast<float>(this->getDistance());
		this->clearInterrupt();
		averageSpadNb = averageSpadNb + static_cast<float>(this->getSignalRate());
	}
	this->stopRanging();

	averageDistance = averageDistance / numberOfMeasurements;
	averageSpadNb = averageSpadNb / numberOfMeasurements;
	averageSignalRate = averageSignalRate / numberOfMeasurements;

	// Calculate Xtalk value
	float crosstalk = (averageSignalRate * (1 - averageDistance / static_cast<float>(targetDistance))) / averageSpadNb;
	uint16_t crosstalkU16 = 512 * static_cast<uint16_t>(crosstalk);
	this->i2cBus.write16(this->address, ALGO_CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS, crosstalkU16);
	return crosstalkU16;
}

void VL53L1X::setGPIO(const char& value) {
	if (this->gpioPath.empty()) {
		return;
	}

	std::lock_guard<std::mutex> guard(this->gpioMutex);
	std::ofstream file;

	file.open(this->gpioPath.c_str(), std::ofstream::out);
	if (!file.is_open() || !file.good()) {
		file.close();
		throw(std::runtime_error(
			std::string("Failed opening file: ")
			+ this->gpioPath
			+ ", reason: "
			+ std::strerror(errno)
		));
	}
	file << value;
	file.close();
}
