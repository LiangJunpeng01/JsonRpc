cmake_minimum_required(VERSION 3.10)
project(ClientProject)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/test.cpp
)

add_executable(test ${SOURCES})

target_include_directories(test PRIVATE
    ${PROJECT_SOURCE_DIR}/home/dio/Begin/JsonRpcFramework/build/
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 