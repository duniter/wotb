cmake_minimum_required(VERSION 3.3)
project(wotc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES wot.h)
set(SOURCE_FILES wot_struct.h)
set(SOURCE_FILES wot.cpp)
set(SOURCE_FILES main.cpp)
add_executable(wotc ${SOURCE_FILES})
