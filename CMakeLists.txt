cmake_minimum_required(VERSION 3.28)
project(spsc_queue)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(spsc_queue main.cpp
        SpscQueue.h)