cmake_minimum_required(VERSION 4.2.3)
project(dht_tercer_proyecto)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
target_include_directories(dht_tercer_proyecto PRIVATE include)
add_executable(dht_tercer_proyecto src/main.cpp)
