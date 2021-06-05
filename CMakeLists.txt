cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(vl53l1x-linux
  LANGUAGES CXX C
  VERSION 0.2.1
  DESCRIPTION "A VL53L1X sensor interfacing library for GNU/Linux"
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

###
# Project options
###
option(BUILD_EXAMPLES "Whether to build examples library" ON)

# Set C++17, with GNU extensions
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

###
# Project targets
###
add_library(${PROJECT_NAME} SHARED
  src/VL53L1X.cpp
  src/VL53L1X_default_config.cpp
)
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    src
)

# Link against sbc-linux-interfaces
find_package(ament_cmake QUIET)
find_package(sbc-linux-interfaces QUIET)
if(ament_cmake_FOUND AND sbc-linux-interfaces_FOUND)
  message(STATUS "Ament found, linking against ament package")
  ament_target_dependencies(${PROJECT_NAME} sbc-linux-interfaces)
else()
  message(STATUS "Ament not found, linking against submodule")
  add_subdirectory(libs/sbc-linux-interfaces)
endif()
target_link_libraries(${PROJECT_NAME} sbc-linux-interfaces)

# Set the library object version
set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION 1
)

# Workaround for older GCCs
if (CMAKE_COMPILER_IS_GNUCXX)
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
  if (NOT GCC_VERSION VERSION_GREATER 8.0)
    message(STATUS "g++ <= 8 detected, adding explicit link to stdc++fs")
    target_link_libraries(${PROJECT_NAME} stdc++fs)
  endif()
endif()

###
# Install targets
###
# Install the main target (the library itself)
install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install the headers and package.xml
install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
  FILES ${PROJECT_SOURCE_DIR}/package.xml
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
)

# Install the CMake targets
install(
  EXPORT ${PROJECT_NAME}-targets
  FILE ${PROJECT_NAME}-targets.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
)

# Prepare and install the package version file
write_basic_package_version_file(
  "${PROJECT_NAME}-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)
install(
  FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
)

# Prepare and install the package config file
set(INCLUDE_INSTALL_DIRS ${CMAKE_INSTALL_INCLUDEDIR})
configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
  PATH_VARS INCLUDE_INSTALL_DIRS
)
install(
  FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
)

###
# (Optionally) include examples
###
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
