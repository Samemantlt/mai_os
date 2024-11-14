cmake_minimum_required(VERSION 3.22)
project(mai_os)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

add_executable(mai_os
        main/main.cpp
        main/common.h)

add_executable(child
        child/main.cpp
        main/common.h)