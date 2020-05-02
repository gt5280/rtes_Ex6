cmake_minimum_required (VERSION 2.8)

project (morse_decoder)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(morse_decoder main.cpp decode.c decode_task.c)

target_Link_Libraries(morse_decoder ${OpenCV_LIBS} rt pthread)
