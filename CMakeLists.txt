cmake_minimum_required(VERSION 3.21)
project(LowLevelCPPClasses)

set(CMAKE_CXX_STANDARD 14)

add_library(LowLevelCPPClasses SPI.cpp SPI.h)

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})