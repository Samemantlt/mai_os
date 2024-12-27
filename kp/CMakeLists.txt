cmake_minimum_required(VERSION 3.22)
project(mai_os)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

include_directories(src)

add_executable(lab
        src/main.cpp
        src/AllocatorBase.h
        src/tests/random_buffers_tests.h
        src/tests/concrete_buffers_tests.h
        src/tests/iterative_buffers_tests.h
        src/tests/result.h
)
