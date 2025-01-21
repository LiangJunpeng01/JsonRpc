
cmake_minimum_required(VERSION 3.10)

project(JsonTest)

set(CMAKE_CXX_STANDARD 14)

add_executable(a.out test.cpp)

target_link_libraries(a.out jsoncpp)
