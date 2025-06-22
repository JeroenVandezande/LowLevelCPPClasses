cmake_minimum_required(VERSION 3.31.6)
project(LowLevelEmbedded CXX)

set(CMAKE_CXX_STANDARD 17)
enable_language(C)

add_library(${PROJECT_NAME} LLE_I2C.h LLE_SPI.h LLE_PIO.cpp LLE_PIO.h Devices/PIOs/MCP23S08.cpp
        Devices/PIOs/MCP23S08.h LLE_IOPIN.cpp LLE_IOPIN.h
        Devices/FanControllers/MAX31790.cpp Devices/FanControllers/MAX31790.h LLE_tools.h Devices/LEDControllers/SerialLeds.cpp
        Devices/LEDControllers/SerialLeds.h Devices/DACs/DAC7578.cpp Devices/DACs/DAC7578.h
        Devices/ADCs/AD7175.cpp
        Devices/ADCs/AD7175.h
        Devices/LEDControllers/PCA9685.cpp
        Devices/LEDControllers/PCA9685.h
        Devices/PIOs/PCA6408.cpp
        Devices/PIOs/PCA6408.h
        Devices/EEProms/EEProm24AA08.cpp
        Devices/EEProms/EEProm24AA08.h
        Devices/Thermopiles/TSD305.cpp
        Devices/Thermopiles/TSD305.h
        Devices/Thermopiles/TSD305_Constants.h
        Logging/RTTLogAppenders.h
        Logging/RTTLogAppenders.c
        Segger_RTT/SEGGER_RTT.c
        Segger_RTT/SEGGER_RTT.h
        Devices/MotorControllers/TMC5130/TMC5130.h
        Devices/MotorControllers/TMC5130/TMC5130_Utils.h
        Devices/MotorControllers/TMC5130/TMC5130_Config.h
        Devices/MotorControllers/TMC5130/TMC5130.cpp
        Devices/MotorControllers/TMC5130/TMC5130_Macros.h
        Devices/MotorControllers/TMC5130/TMC5130_Register.h
        Devices/MotorControllers/TMC5130/TMC5130_Fields.h
        Devices/MotorControllers/TMC5130/TMC5130_Utils.c
        Devices/MotorControllers/TMC5130/TMC5130_API_Header.h
        Devices/MotorControllers/TMC5130/TMC5130_CRC.c
        LLE_Analog.h
        Devices/I2CMultiplexers/TCA9548A.cpp
        Devices/I2CMultiplexers/TCA9548A.h
)

include(cpm/CPM.cmake)
CPMAddPackage("gh:an-dr/microlog@6.4.5")

target_compile_definitions( microlog
        INTERFACE
        ULOG_EXTRA_OUTPUTS=1
        FEATURE_COLOR=1)

target_link_libraries(${PROJECT_NAME} PUBLIC microlog)

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} Segger_RTT Logging Devices/PIOs Devices/FanControllers Devices/LEDControllers Devices/DACs Devices/ADCs Devices/EEProms Devices/MotorControllers/TMC5130)

