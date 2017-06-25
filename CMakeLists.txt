cmake_minimum_required(VERSION 3.5)
project(hello)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")
add_executable(hello ${SOURCES})
target_link_libraries(hello ssh)
