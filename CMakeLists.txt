cmake_minimum_required(VERSION 3.21)
project(LowLevelEmbedded CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} LLE_I2C.h LLE_SPI.h LLE_PIO.cpp LLE_PIO.h Devices/PIOs/MCP23S08.cpp
        Devices/PIOs/MCP23S08.h LLE_IOPIN.cpp LLE_IOPIN.h
        Devices/FanControllers/MAX31790.cpp Devices/FanControllers/MAX31790.h LLE_tools.h Devices/LEDControllers/SerialLeds.cpp
        Devices/LEDControllers/SerialLeds.h Devices/DACs/DAC7578.cpp Devices/DACs/DAC7578.h
        Devices/ADCs/AD7175.cpp
        Devices/ADCs/AD7175.h
        Devices/LEDControllers/PCA9685.cpp
        Devices/LEDControllers/PCA9685.h
        Devices/PIOs/PCA6408.cpp
        Devices/PIOs/PCA6408.h)

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} Devices/PIOs Devices/FanControllers Devices/LEDControllers Devices/DACs Devices/ADCs)

