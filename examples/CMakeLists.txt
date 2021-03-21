# Simple continuous distance reading
add_executable(getDistance
	getDistance.cpp
)
target_link_libraries(getDistance
	PRIVATE vl53l1x
)

# Minimal multiple sensors example
add_executable(multipleSensors
	multipleSensors.cpp
)
target_link_libraries(multipleSensors
	PRIVATE vl53l1x
)
