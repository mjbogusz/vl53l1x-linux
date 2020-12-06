#include "VL53L1X.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <utility>

using namespace std::chrono_literals;

VL53L1X::VL53L1X(
	I2CBus* i2cBus,
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
		this->i2cBus->write8(this->address, configAddr, VL53L1X::DEFAULT_CONFIGURATION[configAddr - 0x2D]);
	}
	this->startRanging();
	while (!this->isDataReady()) {
		std::this_thread::sleep_for(500ms);
	}
	this->clearInterrupt();
	this->stopRanging();
	// two bounds VHV
	this->i2cBus->write8(this->address, VHV_CONFIG_TIMEOUT_MACROP_LOOP_BOUND, 0x09);
	this->i2cBus->write8(this->address, VHV_CONFIG_INIT, 0);

	this->interruptPolarity = !((this->i2cBus->read8(this->address, GPIO_HV_MUX_CTRL) & 0x10) >> 4);
}

void VL53L1X::clearInterrupt() {
	this->i2cBus->write8(this->address, SYSTEM_INTERRUPT_CLEAR, 0x01);
}

void VL53L1X::startRanging() {
	this->i2cBus->write8(this->address, SYSTEM_MODE_START, 0x40);
}

void VL53L1X::stopRanging() {
	this->i2cBus->write8(this->address, SYSTEM_MODE_START, 0x00);
}

bool VL53L1X::isDataReady() {
	return (this->i2cBus->read8(this->address, GPIO_TIO_HV_STATUS) & 0x01) == this->interruptPolarity;
}

void VL53L1X::setTimingBudgetInMs(VL53L1X::TimingBudget timingBudget) {
	auto distanceMode = this->getDistanceMode();
	if (distanceMode == VL53L1X::DistanceMode::Short) {
		// Short DistanceMode
		switch (timingBudget) {
			case 15:
				// only available in short distance mode
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x001D);
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x0027);
				break;
			case 20:
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x0051);
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x006E);
				break;
			case 33:
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x00D6);
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x006E);
				break;
			case 50:
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x01AE);
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x01E8);
				break;
			case 100:
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x02E1);
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x0388);
				break;
			case 200:
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x03E1);
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x0496);
				break;
			case 500:
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x0591);
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x05C1);
				break;
			default:
				break;
		}
	} else if (distanceMode == VL53L1X::DistanceMode::Long) {
		switch (timingBudget) {
			case 20:
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x001E);
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x0022);
				break;
			case 33:
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x0060);
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x006E);
				break;
			case 50:
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x00AD);
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x00C6);
				break;
			case 100:
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x01CC);
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x01EA);
				break;
			case 200:
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x02D9);
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x02F8);
				break;
			case 500:
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI, 0x048F);
				this->i2cBus->write16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_B_HI, 0x04A4);
				break;
			default:
				break;
		}
	}
}

VL53L1X::TimingBudget VL53L1X::getTimingBudget() {
	uint16_t configValue = this->i2cBus->read16(this->address, RANGE_CONFIG_TIMEOUT_MACROP_A_HI);
	switch (configValue) {
		case 0x0051:
		case 0x001E:
			return TB_20;

		case 0x00D6:
		case 0x0060:
			return TB_33;

		case 0x1AE:
		case 0x00AD:
			return TB_50;

		case 0x02E1:
		case 0x01CC:
			return TB_100;

		case 0x03E1:
		case 0x02D9:
			return TB_200;

		case 0x0591:
		case 0x048F:
			return TB_500;

		default:
			return TB_20;
	}
}

void VL53L1X::setDistanceMode(VL53L1X::DistanceMode mode) {
	VL53L1X::TimingBudget budget = this->getTimingBudget();

	switch (mode) {
		case VL53L1X::DistanceMode::Short:
			this->i2cBus->write8(this->address, PHASECAL_CONFIG_TIMEOUT_MACROP, 0x14);
			this->i2cBus->write8(this->address, RANGE_CONFIG_VCSEL_PERIOD_A, 0x07);
			this->i2cBus->write8(this->address, RANGE_CONFIG_VCSEL_PERIOD_B, 0x05);
			this->i2cBus->write8(this->address, RANGE_CONFIG_VALID_PHASE_HIGH, 0x38);
			this->i2cBus->write16(this->address, SD_CONFIG_WOI_SD0, 0x0705);
			this->i2cBus->write16(this->address, SD_CONFIG_INITIAL_PHASE_SD0, 0x0606);
			break;
		case VL53L1X::DistanceMode::Long:
			this->i2cBus->write8(this->address, PHASECAL_CONFIG_TIMEOUT_MACROP, 0x0A);
			this->i2cBus->write8(this->address, RANGE_CONFIG_VCSEL_PERIOD_A, 0x0F);
			this->i2cBus->write8(this->address, RANGE_CONFIG_VCSEL_PERIOD_B, 0x0D);
			this->i2cBus->write8(this->address, RANGE_CONFIG_VALID_PHASE_HIGH, 0xB8);
			this->i2cBus->write16(this->address, SD_CONFIG_WOI_SD0, 0x0F0D);
			this->i2cBus->write16(this->address, SD_CONFIG_INITIAL_PHASE_SD0, 0x0E0E);
			break;
		default:
			break;
	}
	this->setTimingBudgetInMs(budget);
}

VL53L1X::DistanceMode VL53L1X::getDistanceMode() {
	uint8_t configValue = this->i2cBus->read8(this->address, PHASECAL_CONFIG_TIMEOUT_MACROP);

	if (configValue == 0x14) {
		return VL53L1X::DistanceMode::Short;
	}
	if (configValue == 0x0A) {
		return VL53L1X::DistanceMode::Long;
	}
	return VL53L1X::DistanceMode::Unknown;
}

void VL53L1X::setInterMeasurementPeriod(uint16_t period) {
	uint16_t clockPLL = 0x03FF & this->i2cBus->read16(this->address, VL53L1_RESULT_OSC_CALIBRATE_VAL);
	this->i2cBus->write32(this->address, VL53L1_SYSTEM_INTERMEASUREMENT_PERIOD, (uint32_t)(clockPLL * period * 1.075));

	double data = (clockPLL * period * 1075) % 1000;
	while (data > 1) {
		data /= 10.0;
	}
	this->decimal = data;
}

uint16_t VL53L1X::getInterMeasurementPeriod() {
	uint16_t clockPLL = 0x03FF & this->i2cBus->read16(this->address, VL53L1_RESULT_OSC_CALIBRATE_VAL);
	uint32_t period = this->i2cBus->read32(this->address, VL53L1_SYSTEM_INTERMEASUREMENT_PERIOD);

	return (uint16_t)((period + this->decimal) / (clockPLL * 1.075));
}

uint16_t VL53L1X::getDistance() {
	uint16_t distance = this->i2cBus->read16(this->address, VL53L1_RESULT_FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0);
	this->clearInterrupt();
	if (distance > 4000) {
		distance = 65535;
	}
	return distance;
}

uint16_t VL53L1X::getSignalRate() {
	return 8 * this->i2cBus->read16(this->address, VL53L1_RESULT_DSS_ACTUAL_EFFECTIVE_SPADS_SD0);
}

void VL53L1X::setOffset(int16_t offset) {
	this->i2cBus->write16(this->address, ALGO_PART_TO_PART_RANGE_OFFSET_MM, (uint16_t)(offset * 4));
	this->i2cBus->write16(this->address, MM_CONFIG_INNER_OFFSET_MM, 0x0);
	this->i2cBus->write16(this->address, MM_CONFIG_OUTER_OFFSET_MM, 0x0);
}

int16_t VL53L1X::getOffset() {
	uint16_t tmp = this->i2cBus->read16(this->address, ALGO_PART_TO_PART_RANGE_OFFSET_MM);
	// adjust
	if (tmp & 0x1000) {
		tmp |= 0xE000;
	}
	// cast to signed and shift
	return (int16_t)(tmp) / 4;
}

void VL53L1X::setCrosstalk(uint16_t crosstalkValue) {
	this->i2cBus->write16(this->address, ALGO_CROSSTALK_COMPENSATION_X_PLANE_GRADIENT_KCPS, 0x0000);
	this->i2cBus->write16(this->address, ALGO_CROSSTALK_COMPENSATION_Y_PLANE_GRADIENT_KCPS, 0x0000);
	this->i2cBus->write16(this->address, ALGO_CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS, (crosstalkValue << 9) / 1000);
}

uint16_t VL53L1X::getCrosstalk() {
	uint16_t crosstalk = this->i2cBus->read16(this->address, ALGO_CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS);
	return (crosstalk * 1000) >> 9;
}

uint16_t VL53L1X::getDistanceThresholdLow() {
	return this->i2cBus->read16(this->address, SYSTEM_THRESH_LOW);
}

uint16_t VL53L1X::getDistanceThresholdHigh() {
	return this->i2cBus->read16(this->address, SYSTEM_THRESH_HIGH);
}

int8_t VL53L1X::calibrateOffset(uint16_t targetDistance) {
	constexpr uint8_t numberOfMeasurements = 50;

	this->i2cBus->write16(this->address, ALGO_PART_TO_PART_RANGE_OFFSET_MM, 0x0);
	this->i2cBus->write16(this->address, MM_CONFIG_INNER_OFFSET_MM, 0x0);
	this->i2cBus->write16(this->address, MM_CONFIG_OUTER_OFFSET_MM, 0x0);

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
	this->i2cBus->write16(this->address, ALGO_PART_TO_PART_RANGE_OFFSET_MM, offset * 4);
	return offset;
}

int8_t VL53L1X::calibrateCrosstalk(uint16_t targetDistance) {
	constexpr uint8_t numberOfMeasurements = 50;

	this->i2cBus->write16(this->address, ALGO_CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS, 0);

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
	this->i2cBus->write16(this->address, ALGO_CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS, crosstalkU16);
	return crosstalkU16;
}
