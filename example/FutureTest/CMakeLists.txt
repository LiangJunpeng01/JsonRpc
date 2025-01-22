cmake_minimum_required(VERSION 3.10)

project(Future)

add_executable(async asynctest.cpp)

add_executable(pacaged packagedtest.cpp)

add_executable(promise promisetest.cpp)

target_link_libraries(async pthread)

target_link_libraries(pacaged pthread)

target_link_libraries(promise pthread)


