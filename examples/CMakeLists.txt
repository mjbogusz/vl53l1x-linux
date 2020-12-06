# add_executable(rpitest
# 	src/rpi-test.cpp
# )
# target_link_libraries(rpitest
# 	PRIVATE vl53l1x
# )

add_executable(getDistance
	getDistance.cpp
)
target_link_libraries(getDistance
	PRIVATE vl53l1x
)
