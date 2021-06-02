#include "VL53L1X.hpp"
#include <GPIOPin.hpp>
#include <I2CBus.hpp>

#include <iostream>
#include <csignal>

static bool exitFlag = false;

void signalHandler(int signalNumber) {
	if (signalNumber == SIGINT) {
		exitFlag = true;
	}
}

int main() {
	auto i2c = I2CBus::makeShared("/dev/i2c-3");
	auto gpio6 = GPIOPin::makeShared("/sys/class/gpio/gpio6");
	auto gpio16 = GPIOPin::makeShared("/sys/class/gpio/gpio16");
	auto gpio19 = GPIOPin::makeShared("/sys/class/gpio/gpio19");

	VL53L1X sensor1(i2c, gpio6);
	VL53L1X sensor2(i2c, gpio16);
	VL53L1X sensor3(i2c, gpio19);

	std::signal(SIGINT, signalHandler);

	sensor1.powerOff();
	sensor2.powerOff();
	sensor3.powerOff();

	sensor1.powerOn();
	sensor1.setAddress(0x29 + 1);
	sensor2.powerOn();
	sensor2.setAddress(0x29 + 2);
	sensor3.powerOn();
	sensor3.setAddress(0x29 + 3);

	// This MAY throw
	sensor1.initialize();
	sensor2.initialize();
	sensor3.initialize();

	sensor1.startRanging();
	sensor2.startRanging();
	sensor3.startRanging();
	while (!exitFlag) {
		uint16_t distance1 = sensor1.getDistance();
		uint16_t distance2 = sensor2.getDistance();
		uint16_t distance3 = sensor3.getDistance();
		std::cout << distance1 << " " << distance2 << " " << distance3 << std::endl;
	}

	sensor1.stopRanging();
	sensor2.stopRanging();
	sensor3.stopRanging();

	return 0;
}
