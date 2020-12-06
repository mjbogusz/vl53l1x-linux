cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(VL53L1X_Linux
	LANGUAGES CXX C
	VERSION 0.1.0
	DESCRIPTION "A VL53L1X sensor interfacing library for GNU/Linux"
)

option(BUILD_EXAMPLES "Whether to build examples library" ON)

# Set C++17, with GNU extensions
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

include(GNUInstallDirs)

add_library(vl53l1x SHARED
	src/VL53L1X.cpp
	src/VL53L1X_default_config.cpp
	src/I2CBus.cpp
)
target_include_directories(vl53l1x
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	PRIVATE
		src
)
set_target_properties(vl53l1x PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION 1
)

# Workaround for older GCCs
if (CMAKE_COMPILER_IS_GNUCXX)
	execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
	if (NOT GCC_VERSION VERSION_GREATER 8.0)
		message(STATUS "g++ <= 8 detected, adding explicit link to stdc++fs")
		target_link_libraries(vl53l1x stdc++fs)
	endif()
endif()

install(TARGETS vl53l1x
	EXPORT VL53L1XConfig
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vl53l1x)
install(EXPORT VL53L1XConfig DESTINATION share/VL53L1X/cmake)
export(TARGETS vl53l1x FILE VL53L1XConfig.cmake)

if(BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()
