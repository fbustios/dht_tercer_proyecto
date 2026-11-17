cmake_minimum_required(VERSION 4.2.3)
project(dht_tercer_proyecto)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(dht_tercer_proyecto src/main.cpp
        src/network.cpp
        src/utils.cpp
        src/dht/ChordNode.cpp
)
find_package(OpenSSL REQUIRED)

target_link_libraries(dht_tercer_proyecto
        PRIVATE
        OpenSSL::Crypto
)

target_include_directories(dht_tercer_proyecto PRIVATE include)
