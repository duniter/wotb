cmake_minimum_required(VERSION 3.3)
project(wotcpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include/)

file(GLOB SOURCE_FILES *.cpp)

add_executable(wotcpp ${SOURCE_FILES})
