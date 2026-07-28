# LowLevelCPPClasses

[![Build](https://github.com/JeroenVandezande/LowLevelCPPClasses/actions/workflows/cmake-multi-platform.yml/badge.svg?branch=master)](https://github.com/JeroenVandezande/LowLevelCPPClasses/actions/workflows/cmake-multi-platform.yml)
[![CPM.cmake ready](https://img.shields.io/badge/CPM.cmake-ready-blue.svg)](https://github.com/cpm-cmake/CPM.cmake)

Platform-independent C++ interfaces, peripheral abstractions, reusable device
drivers, and embedded utilities.

Version 4 uses
[UnitsNet-CPP](https://github.com/JeroenVandezande/unitsnet-cpp) types at
physical-quantity API boundaries. Voltages, currents, temperatures, humidity,
pressure, durations, frequencies, angles, fan speeds, and similar values can no
longer be accidentally mixed as unitless `float`, `double`, or integer values.
Raw register values, ADC counts, channel numbers, and device-specific bitfields
remain integral.

Applications provide an MCU-specific implementation of interfaces such as
`IOPIN`, `II2CAccess`, `ISPIAccess`, `IADC`, and `IDAC`. Device drivers depend
only on those interfaces, allowing the same driver to be used across supported
microcontroller families.

## MCU companion packages

MCU-specific implementations are maintained separately:

| Package | CMake target | Implementations |
|---|---|---|
| [LowLevelCPP_STM32](https://github.com/JeroenVandezande/LowLevelCPP_STM32) | `LowLevelCPP::STM32` | STM32 HAL/LL GPIO, ADC, I2C, and SPI |
| [LowLevelCPP_EFM32](https://github.com/JeroenVandezande/LowLevelCPP_EFM32) | `LowLevelCPP::EFM32` | Silicon Labs EMLIB GPIO, classic ADC, IADC, DAC, VDAC, I2C, and USART-SPI |
| [LowLevelCPP_XMC](https://github.com/JeroenVandezande/LowLevelCPP_XMC) | `LowLevelCPP::XMC` | Infineon XMCLib GPIO, USIC I2C/SPI, VADC, and XMC4 DAC |

The application remains responsible for selecting the MCU, initializing
clocks and peripherals, configuring pins and routing, and linking the vendor
HAL or peripheral library.

## Requirements

* A C++20-compatible compiler
* CMake 3.31.6 or newer when building the supplied CMake project
* CPM.cmake when dependencies are not already provided by the parent project

The CMake project resolves
[unitsnet-cpp](https://github.com/JeroenVandezande/unitsnet-cpp) and
[microlog](https://github.com/an-dr/microlog) through CPM.cmake. SEGGER RTT
sources are included directly in this repository.

## Add with CPM.cmake

```cmake
if(NOT COMMAND CPMAddPackage)
    set(CPM_VERSION 0.43.1)
    message("Downloading CPM Package Manager version ${CPM_VERSION}")
    file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_VERSION}/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/CPM_${CPM_VERSION}.cmake
    )
    include("${CMAKE_CURRENT_BINARY_DIR}/CPM_${CPM_VERSION}.cmake")
endif()

CPMAddPackage("gh:JeroenVandezande/LowLevelCPPClasses@4.0.0")

target_link_libraries(your_target PRIVATE LowLevelEmbedded)
```

The `@4.0.0` suffix selects Git tag `v4.0.0`, keeping dependency resolution
reproducible. Add the appropriate MCU companion package after the core package
and link its target as documented in that repository.

## Version 4 units migration

Construct quantities explicitly at application and hardware boundaries:

```cpp
using namespace unitsnet_cpp;

auto supply = adc.ReadVoltage(0);
dac.WriteDACVoltage(0, ElectricPotential::from_volts(1.8f));
pwm.SetPWMPercentage(Ratio::from_percent(35.0f));

Temperature ambient = sensor.GetTemperature();
fan.setFanTargetSpeed(
    0,
    RotationalSpeed::from_revolutions_per_minute(2400.0f));
```

Read a value in the unit needed by an algorithm or protocol with accessors such
as `.volts()`, `.degrees_celsius()`, `.percent()`, `.milliseconds()`, or
`.revolutions_per_minute()`. Scalar extraction should normally stay inside an
implementation; application-facing APIs should continue to pass the quantity
type.

Version 4 is intentionally source-incompatible with version 3. Typical
migrations replace arguments such as `3.3f`, `25.0f`, or `1500` with
`ElectricPotential::from_volts(3.3f)`,
`Temperature::from_degrees_celsius(25.0f)`, or
`RotationalSpeed::from_revolutions_per_minute(1500.0f)`.

## Local CMake integration

```cmake
add_subdirectory(path/to/LowLevelCPPClasses)

target_link_libraries(your_target PRIVATE LowLevelEmbedded)
```

When developing against a companion package, add the core repository first:

```cmake
add_subdirectory(path/to/LowLevelCPPClasses)
add_subdirectory(path/to/LowLevelCPP_STM32)

target_link_libraries(your_target PRIVATE LowLevelCPP::STM32)
```

Use the corresponding repository path and CMake target from the companion
package table for EFM32 or XMC.

## Generic interfaces

The `Base` directory provides:

| Area | Interfaces and helpers |
|---|---|
| ADC | `ISimpleADCChannel`, `IADCChannel`, `IADC`, `ADCChannel_base` |
| DAC | `ISimpleDACChannel`, `IDACChannel`, `IDAC`, `DACChannel_base` |
| Display | `IDisplay` |
| GPIO and parallel I/O | `IOPIN`, `IPIO`, `IPIO_8`, `IOPIN_PIO8` |
| I2C | `II2CAccess`, `II2CDevice`, `I2CMultiplexer_base`, `I2CMultiplexer_channel` |
| PWM | `ISimplePWMChannel`, `IPWMChannel`, `IPWMController`, `PWMChannel_base` |
| Sensors | `ITemperatureSensor`, `IHumiditySensor`, `IPressureSensor` |
| SPI | `ISPIAccess` and `SPIMode` |

`II2CAccess` uses left-adjusted 7-bit addresses: bit 0 is reserved for the
read/write direction. `ISPIAccess` uses a chip-select ID so one bus adapter can
serve multiple devices.

## Device drivers

The following drivers are currently present under `Devices`:

| Category | Driver | Interface | Purpose |
|---|---|---|---|
| ADC | AD7175 | SPI | Precision analog-to-digital converter, including its GPIO pins |
| DAC | DAC7578 | I2C | Multi-channel digital-to-analog converter |
| DAC | PWM_DAC | PWM | Adapts a PWM channel to the generic DAC interface |
| Display | SSD1306 | I2C or SPI | Monochrome OLED display and bundled font data |
| EEPROM | 24AA08 | I2C | EEPROM with block/page buffering helpers |
| Encoder | AS5311 | SPI | Magnetic position encoder |
| Fan control | MAX31790 | I2C | Multi-channel fan controller |
| I2C multiplexer | TCA9548A | I2C | Eight-channel I2C bus switch |
| LED control | PCA9685 | I2C | Multi-channel PWM/LED controller |
| LED control | SerialLED | SPI | Buffered RGB/RGBW serial LEDs with selectable color order |
| Monitoring | INA228 | I2C | Current, voltage, power, and energy monitor |
| Motor control | TMC5130 | SPI | Stepper-motor controller and motion driver |
| Parallel I/O | MCP23S08 | SPI | Eight-bit GPIO expander |
| Parallel I/O | PCA6408 | I2C | Eight-bit GPIO expander |
| Power | MPQ4262 | I2C | Configurable power-converter controller |
| Sensor | MPL3115A2 | I2C | Pressure and temperature sensor |
| Sensor | SHT4x | I2C | Humidity and temperature sensor |
| Thermopile | MLX90614 | I2C | Contactless infrared temperature sensor |
| Thermopile | TSD305 | I2C | Calibrated object and sensor temperature measurements |
| USB Type-C | TCPP02-M18 | I2C | Source-port protection and VCONN/VBUS control |
| USB Type-C | TUSB544 | I2C | USB-C and DisplayPort signal conditioning |
| USB Type-C | TUSB546 | I2C | USB-C and DisplayPort lane configuration |

Some drivers expose additional configuration types, register definitions, or
channel objects alongside the primary class. See the corresponding header for
device-specific behavior and supported features.

## Utilities and support code

| Component | Purpose |
|---|---|
| `Delay.h` | Application-provided millisecond, microsecond, and system-time callbacks with unitsnet duration helpers |
| `LL_Math.h` | Constrained rounding, casting, and numeric helpers |
| `LookupTable.h` | Compile-time-sized lookup tables and interpolation |
| `Logging/RTTLogAppenders` | microlog appenders for SEGGER RTT |
| `Segger_RTT` | Bundled SEGGER Real-Time Transfer implementation |

## Repository layout

```text
Base/        Generic interfaces and reusable base classes
Devices/     Platform-independent device drivers
Logging/     Logging integrations
Segger_RTT/  SEGGER RTT sources
Utilities/   General embedded helpers
```
