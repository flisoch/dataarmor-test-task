cmake_minimum_required(VERSION 3.16)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
project(pgproxy)

add_executable(pgproxy src/main.cpp)
set_property(TARGET pgproxy PROPERTY CXX_STANDARD 17)

find_package(Boost 1.40.0 REQUIRED system thread)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(pgproxy ${Boost_LIBRARIES})
