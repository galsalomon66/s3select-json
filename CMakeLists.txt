cmake_minimum_required(VERSION 3.0)

project(json)

set(CMAKE_CXX_FLAGS "-ggdb -std=gnu++17 -lgtest -lpthread")

find_package(GTest REQUIRED)

add_executable(json test_json.cpp dom.cpp)
