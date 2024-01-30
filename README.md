# VL53L1X library for GNU/Linux

[![pipeline status](https://gitlab.com/mjbogusz/vl53l1x-linux/badges/master/pipeline.svg)](https://gitlab.com/mjbogusz/vl53l1x-linux/-/commits/master)
<!-- [![coverage report](https://gitlab.com/mjbogusz/vl53l1x-linux/badges/master/coverage.svg)](https://gitlab.com/mjbogusz/vl53l1x-linux/-/commits/master) -->

* Version: 0.2.1
* Status: should-be-working, 2 examples provided
* Homepage: https://gitlab.com/mjbogusz/vl53l1x-linux
* Mirror: https://github.com/mjbogusz/vl53l1x-linux
* Documentation/API: https://mjbogusz.gitlab.io/vl53l1x-linux/

[[_TOC_]]

## About
This is a library for GNU/Linux-based SBCs that helps interfacing with the ST's [VL53L1X time-of-flight distance sensor](https://www.pololu.com/product/3415).
The library aims to make it simple to configure the sensor and read range data from it via I&sup2;C.
Additionally multiple sensors on the same bus are supported.

## Supported platforms
This library is designed to work with most GNU/Linux-based single board computers, including Raspberry Pi, BeagleBone and others, exporting the `i2cdev` interface.

## Getting started
### Hardware
A [VL53L1X carrier](https://www.pololu.com/product/3415) can be purchased from Pololu's website.
Before continuing, careful reading of the [product page](https://www.pololu.com/product/3415) as well as the VL53L1X datasheet is recommended.

#### Connections
TBD

### Software
This library depends on the [sbc-linux-interfaces library](https://gitlab.com/mjbogusz/sbc-linux-interfaces/).
It can be accesses either as a git submodule or as an [`ament`](https://design.ros2.org/articles/ament.html) dependency in a [`colcon`](http://design.ros2.org/articles/build_tool.html) (ROS2) workspace.

SBC-linux-interfaces options:
* git submodule: `git clone --recursive` or `git submodule update --init --recursive`
* colcon workspace: have the `sbc-linux-interfaces` package available in `colcon_ws/src`

No further action is required - the interfaces library should be picked up by CMake regardless of the selected method.

## Examples
Several examples are available that show how to use the library:
* `getDistance` is a minimal working example for a single sensor;
* `multipleSensors` is an example of interfacing with multiple sensors on the same bus.

To build the examples, run `cmake` with the flag: `-DBUILD_EXAMPLES=On` and compile the project.
Then, the examples can be executed as:
```sh
build/examples/getDistance.cpp
build/examples/multipleSensors.cpp
```

## Credits
* based upon [`DFRobot_VL53L1X Library for Arduino`](https://github.com/DFRobot/DFRobot_VL53L1X) by [luoyufeng](yufeng.luo@dfrobot.com)
