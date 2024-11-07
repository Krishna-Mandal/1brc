cmake_minimum_required(VERSION 3.29)
project(1brc)

set(CMAKE_CXX_STANDARD 17)

include_directories(src/cpp)

add_executable(1brc
    src/cpp/CalculateAverage_KrishnaMandal.cpp
)
