cmake_minimum_required(VERSION 3.6)

project(hello)

file(GLOB SOURCES "src/*.cpp")
add_executable(hello ${SOURCES})
