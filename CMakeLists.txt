cmake_minimum_required(VERSION 3.21)
project(LowLevelCPPClasses CXX)

set(CMAKE_CXX_STANDARD 14)

add_library(${PROJECT_NAME} SPI.cpp SPI.hpp PIO.cpp PIO.hpp)

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

