cmake_minimum_required(VERSION 3.10)

project(DicServer)

set(INCLUDE_DIR ../../build/release-install-cpp11/include/)

set(LIB_DIR ../../build/release-install-cpp11/lib)

include_directories(${INCLUDE_DIR})

link_directories(${LIB_DIR})

set(SERVER_SOURCES server.cpp)

# set(CLIENT_SOURCES server.cpp)

set(CLIENT_SOURCES client.cpp)

add_executable(server ${SERVER_SOURCES})

add_executable(client ${CLIENT_SOURCES})

target_link_libraries(server muduo_net muduo_base pthread)

target_link_libraries(client muduo_net muduo_base pthread)



