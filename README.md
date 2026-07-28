# LowLevelCPPClasses 📕

![Build](https://github.com/JeroenVandezande/LowLevelCPPClasses/actions/workflows/cmake-multi-platform.yml/badge.svg?branch=master)
[![CPM.cmake ready](https://img.shields.io/badge/CPM.cmake-ready-blue.svg)](https://github.com/cpm-cmake/CPM.cmake)

A small CMake package containing platform-independent C++ interfaces, device
drivers, and utilities for embedded systems.

MCU-specific adapters live in separate companion packages:

* [LowLevelCPP_STM32](https://github.com/JeroenVandezande/LowLevelCPP_STM32)
* [LowLevelCPP_EFM32](https://github.com/JeroenVandezande/LowLevelCPP_EFM32)

## Requirements

* A C++20-compatible compiler
* CMake 3.31.6 or newer when using the supplied CMake project

## Add with CPM.cmake

The intended way to consume `LowLevelCPPClasses` is with
[CPM.cmake](https://github.com/cpm-cmake/CPM.cmake):

```cmake
if (NOT COMMAND CPMAddPackage)
    set(CPM_VERSION 0.43.1)
    message("Downloading CPM Package Manager version ${CPM_VERSION}")
    file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_VERSION}/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/CPM_${CPM_VERSION}.cmake
    )
    include("${CMAKE_CURRENT_BINARY_DIR}/CPM_${CPM_VERSION}.cmake")
endif()

CPMAddPackage("gh:JeroenVandezande/LowLevelCPPClasses@3.9.0")

target_link_libraries(your_target
    PRIVATE
        LowLevelEmbedded
)
```

CPM downloads and configures the library and its dependencies during CMake
configuration. The `@3.9.0` suffix selects the `v3.9.0` Git tag, keeping
dependency resolution reproducible.

## Local CMake integration

When developing against a local checkout, add the repository directly and link
the same target:

```cmake
add_subdirectory(path/to/LowLevelCPPClasses)

target_link_libraries(your_target
    PRIVATE
        LowLevelEmbedded
)
```

## Current Generic Interfaces 🧩

* SPI
* I2C
* Serial LED Controllers
* 8-Bit I/O Expander
* Basic IO Pins
* I2C Muliplexer
* RTT (Segger)
* PWM DAC

## Current Device Implementations 🏗️

* **ADCs**:
	* AD7175
* **DACs**:
	* DAC7578
* **Displays**:
	* SSD1306
* **EEProms**:
	* 24AA08
* **Encoders**:
	* AS5311
* **Fan Controllers**:
	* MAX31790
* **I2C Muxs**:
	* TCA9548A
* **Led Controller**:
	* PCA9685
* **Monitoring**:
	* INA228
* **Motor Controller**:
	* TMC5130
* **Power**:
	* MPQ4262
* **Sensors**:
	* MPL3115A2
	* SHT4x
* **Thermopiles**
	* MLX90614
	* TSD305
* **USB**:
	* TCPP02
	* TUSB544
	* TUSB546
* **8-Bit I/O Expanders**:
	* MCP23S08
	* PCA6408

## Utilities

* LookupTable
