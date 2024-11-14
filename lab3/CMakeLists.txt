cmake_minimum_required(VERSION 3.29)
project(mai_os)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

add_executable(mai_os
        main/main.cpp)

add_executable(child
        child/main.cpp)