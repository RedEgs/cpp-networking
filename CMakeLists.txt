cmake_minimum_required(VERSION 4.0)
project(network_testing)

set(CMAKE_CXX_STANDARD 20)

add_executable(network_testing main.cpp
        src/server.cpp
        src/server.h
        src/server.h
        src/client.cpp
        src/client.h)
target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)