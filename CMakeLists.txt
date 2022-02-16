cmake_minimum_required(VERSION 3.0)

project(json)

set(CMAKE_CXX_FLAGS "-std=gnu++17 -ggdb -lgtest -lpthread -lstdc++fs")

find_package(GTest REQUIRED)

add_executable(json test_json.cpp dom.cpp)

target_link_libraries(json stdc++fs)