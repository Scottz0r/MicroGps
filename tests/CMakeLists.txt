# This will build a test executable for testing MicroGps.
cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(MicroGpsTests)

add_executable(MicroGpsTests
    MicroGps_tests.cpp
    MicroGpsFormat_tests.cpp
    test_main.cpp
    # Don't forget the source files from the root!
    ${PROJECT_SOURCE_DIR}/../MicroGps.cpp
    ${PROJECT_SOURCE_DIR}/../MicroGpsFormat.cpp
    )

# Need to add the git repo root as include for the MicroGps headers.
target_include_directories(MicroGpsTests PUBLIC ${PROJECT_SOURCE_DIR}/..)
