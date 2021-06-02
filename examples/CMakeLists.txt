# Simple continuous distance reading
add_executable(getDistance
	getDistance.cpp
)
target_link_libraries(getDistance
	PRIVATE VL53L1X sbc-linux-interfaces
)

# Minimal multiple sensors example
add_executable(multipleSensors
	multipleSensors.cpp
)
target_link_libraries(multipleSensors
	PRIVATE VL53L1X sbc-linux-interfaces
)
