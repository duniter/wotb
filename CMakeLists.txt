cmake_minimum_required(VERSION 3.6)
project(wotb)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    wotcpp/include/AbstractWoT.h
    wotcpp/include/distanceResult.h
    wotcpp/include/FileWoT.h
    wotcpp/include/log.h
    wotcpp/include/MemoryWoT.h
    wotcpp/include/misc.h
    wotcpp/include/node.h
    wotcpp/include/webOfTrust.h
    wotcpp/include/wotSet.h
    wotcpp/include/wotStep.h
    wotcpp/FileWoT.cpp
    wotcpp/log.cpp
    wotcpp/main.cpp
    wotcpp/MemoryWoT.cpp
    wotcpp/misc.cpp
    wotcpp/node.cpp
    wotcpp/webOfTrust.cpp
    functions.cc
    functions.h
    wotb.cc)

add_executable(wotb ${SOURCE_FILES})