cmake_minimum_required(VERSION 3.22)
project(mai_os)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

add_library(LibImpl1 SHARED
        src/GCF_Euclid.cpp
        src/Square_Rectangle.cpp
)

add_library(LibImpl2 SHARED
        src/GCF_Naive.cpp
        src/Square_Triangle.cpp
)

add_executable(Program1
        src/Program1.cpp)

target_link_libraries(Program1 LibImpl1)

add_executable(Program2
        src/Program2.cpp)
