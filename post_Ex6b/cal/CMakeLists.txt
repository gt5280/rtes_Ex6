cmake_minimum_required (VERSION 2.8)

project (calibrate)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(calibrate calibrate.cpp)

target_Link_Libraries(calibrate ${OpenCV_LIBS} pthread)
