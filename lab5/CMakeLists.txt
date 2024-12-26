cmake_minimum_required(VERSION 3.22)
project(mai_os)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

include_directories(src)

add_library(common
        src/common/message.h
        src/common/Node.cpp
        src/common/messaging.h
        src/common/NodeSwarm.h
        src/common/message.cpp
        src/common/messaging.cpp
)
# Set header include directories for the library
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(master
        src/master/main.cpp
        src/master/MasterNodeSwarm.cpp
        src/master/MasterNodeSwarm.h
)

add_executable(slave
        src/slave/main.cpp
        src/slave/SlaveNodeSwarm.cpp
        src/slave/SlaveNodeSwarm.h
)

target_link_libraries(master PRIVATE common zmq)
target_link_libraries(slave PRIVATE common zmq)
