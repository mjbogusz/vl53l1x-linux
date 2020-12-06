/*!
   file getDistance.ino
   SEN0302 Distance Ranging Sensor
   The module is connected with Arduino Uno to read the distance
   This demo output the distance between sensor and target
   Copyright    [DFRobot](http://www.dfrobot.com), 2016
   Copyright    GNU Lesser General Public License
   version  V0.1
   date  2019-4-22
 */
#include "VL53L1X.hpp"

#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

int main() {
	VL53L1X sensor;

	while (sensor.begin() != true) {
		std::cout << "Sensor init failed!\n";
		std::this_thread::sleep_for(1s);
	}

	while (true) {
		sensor.startRanging();
		uint16_t distance = sensor.getDistance();
		sensor.stopRanging();
		std::cout << distance << std::endl;
	}

	return 0;
}
