# LowLevelCPPClasses 📕

![Build](https://github.com/JeroenVandezande/LowLevelCPPClasses/actions/workflows/cmake-multi-platform.yml/badge.svg?branch=master)


A small CPM Package containing C++ objects for common embedded interfaces/devices. Tailored to STM32 MCUs but can be used generically on other MCUs.

## Current Generic Interfaces 🧩

* SPI
* I2C
* Serial LED Controllers
* 8-Bit I/O Expander
* Basic IO Pins
* I2C Muliplexer
* RTT (Segger)
* PWM DAC
* STM32ADC Generic

## Current Device Implementations 🏗️

* **ADCs**:
	* AD7175
* **DACs**:
	* DAC7578
* **EEProms**:
	* 24AA08
* **Encoders**:
	* AS5311
* **Fan Controllers**:
	* MAX31790
* **I2C Controllers**:
	* STM32I2CAccess
* **I2C Muxs**:
	* TCA9548A
* **Led Controller**:
	* PCA9685
* **Motor Controller**:
	* TMC5130
* **Power**:
	* MPQ4262
* **Thermopiles**
	* MLX90614
	* TSD305
* **USB**:
	* TCPP02
	* TUSB544
* **8-Bit I/O Expanders**:
	* MCP23S08
	* PCA6408

## Utilities

* LookupTable