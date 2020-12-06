# VL53L1X library for GNU/Linux

[![pipeline status](https://gitlab.com/mjbogusz/vl53l1x-linux/badges/master/pipeline.svg)](https://gitlab.com/mjbogusz/vl53l1x-linux/-/commits/master)
<!-- [![coverage report](https://gitlab.com/mjbogusz/vl53l1x-linux/badges/master/coverage.svg)](https://gitlab.com/mjbogusz/vl53l1x-linux/-/commits/master) -->

* Version: 0.1.0
* Status: minimal mostly working example for 1 sensor
* Homepage: https://gitlab.com/mjbogusz/vl53l1x-linux
* Mirror: https://github.com/mjbogusz/vl53l1x-linux

## Table of Contents:
<!-- MarkdownTOC -->

* [About](#about)
* [Supported platforms](#supported-platforms)
* [Getting started](#getting-started)
	* [Hardware](#hardware)
		* [Connections](#connections)
	* [Software](#software)
* [Examples](#examples)
* [API](#api)
* [Credits](#credits)

<!-- /MarkdownTOC -->

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
TBD

## Examples
Several examples are available that show how to use the library.

TBD

## API
See docs [TBD]

## Credits
* based upon [`DFRobot_VL53L1X Library for Arduino`](https://github.com/DFRobot/DFRobot_VL53L1X) by [luoyufeng](yufeng.luo@dfrobot.com)
