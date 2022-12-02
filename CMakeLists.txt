cmake_minimum_required(VERSION 3.21)
project(LowLevelEmbedded CXX)

set(CMAKE_CXX_STANDARD 14)

add_library(${PROJECT_NAME} I2C.cpp I2C.h SPI.cpp SPI.h PIO.cpp PIO.h Devices/PIOs/MCP23S08.cpp
        Devices/PIOs/MCP23S08.h IOPIN.cpp IOPIN.h
        Devices/FanControllers/MAX31790.cpp Devices/FanControllers/MAX31790.h)

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} Devices/PIOs)

