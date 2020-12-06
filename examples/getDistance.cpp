#include "I2CBus.hpp"
#include "VL53L1X.hpp"

#include <iostream>
#include <csignal>

static bool exitFlag = false;

void signalHandler(int signalNumber) {
	if (signalNumber == SIGINT) {
		exitFlag = true;
	}
}

int main() {
	I2CBus i2c("/dev/i2c-5");
	VL53L1X sensor(&i2c);

	std::signal(SIGINT, signalHandler);

	// This MAY throw
	sensor.initialize();

	sensor.startRanging();
	while (!exitFlag) {
		uint16_t distance = sensor.getDistance();
		std::cout << distance << std::endl;
	}

	sensor.stopRanging();

	return 0;
}
