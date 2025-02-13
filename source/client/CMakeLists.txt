cmake_minimum_required(VERSION 3.10)

project(client_test)

set(CMAKE_CXX_STANDARD 17)


include_directories(../../build/release-install-cpp11/include)

link_directories(../../build/release-install-cpp11/lib/)

add_executable(client test.cpp)

target_link_libraries(client
    pthread
    muduo_base
    muduo_net
    jsoncpp
    )