cmake_minimum_required(VERSION 3.1)
project(mai_os)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

add_executable(mai_os
        main/sort.cpp
        main/main.cpp)

add_executable(tests
        main/sort.cpp
        main/tests.cpp)

add_executable(benchmark
        main/sort.cpp
        main/benchmark.cpp)

add_executable(benchmark_csv
        main/sort.cpp
        main/benchmark_csv.cpp)
