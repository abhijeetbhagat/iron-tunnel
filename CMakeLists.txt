cmake_minimum_required(VERSION 3.5)
project(iron-tunnel)

include_directories(include)

file(GLOB CLIENT "src/client.cpp")
file(GLOB SERVER "src/server.cpp")
add_executable(client ${CLIENT})
add_executable(server ${SERVER})
target_link_libraries(client ssh)
target_link_libraries(server ssh)
