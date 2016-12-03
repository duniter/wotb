cmake_minimum_required(VERSION 3.3)
project(wotcpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")

include_directories(include/ include/simulation)

file(GLOB SOURCE_FILES *.cpp simulation/*.cpp)

add_executable(wotcpp ${SOURCE_FILES})
