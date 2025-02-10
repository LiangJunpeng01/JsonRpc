cmake_minimum_required(VERSION 3.10)

project(JsonRPC)

set(CMAKE_CXX_STANDARD 17)

set(INCLUDE_DIR ../../build/release-install-cpp11/include/)

set(LIB_DIR ../../build/release-install-cpp11/lib)

set(CMAKE_BUILD_TYPE "Debug")

include_directories(${INCLUDE_DIR})

link_directories(${LIB_DIR})

add_executable(server server.cpp)

target_link_libraries(server  muduo_net muduo_base pthread jsoncpp)

add_executable(client client.cpp)

target_link_libraries(client  muduo_net muduo_base pthread jsoncpp)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

