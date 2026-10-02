//
// Created by Dana Natov on 2026-10-02.
//

#pragma once

#include "LLE_I2C.h"
#include "LLE_Temp.h"
#include <Temperature.hpp>

#include <cstdint>

namespace LowLevelEmbedded::Devices::Sensors
{
    /// Microchip MCP9801 2-wire (I2C/SMBus) high-accuracy temperature sensor.
    ///
    /// The device has four pointer-addressed registers: a 16-bit read-only ambient
    /// temperature register (TA), an 8-bit configuration register (CONFIG) and two
    /// 16-bit read/write limit registers (THYST and TSET) that drive the ALERT pin.
    ///
    /// By default the sensor converts continuously, so `ReadTemperature` is a plain
    /// register read. `ReadTemperatureOneShot` uses the datasheet shutdown + one-shot
    /// sequence for low-power, on-demand sampling. All referenced interfaces must
    /// outlive this object. Calls must be serialized.
    class MCP9801 final : public ITemperatureSensor
    {
    public:
        /// ADC resolution. Finer resolutions take longer to convert (typ. 30 ms at
        /// 9 bits up to 240 ms at 12 bits; datasheet maximum is 2.5x the typical).
        enum class Resolution : uint8_t
        {
            Bits9  = 0, // 0.5 °C
            Bits10 = 1, // 0.25 °C
            Bits11 = 2, // 0.125 °C
            Bits12 = 3  // 0.0625 °C
        };

        /// Number of consecutive out-of-limit conversions before ALERT changes state.
        enum class FaultQueue : uint8_t
        {
            Faults1 = 0,
            Faults2 = 1,
            Faults4 = 2,
            Faults6 = 3
        };

        enum class AlertPolarity : uint8_t { ActiveLow = 0, ActiveHigh = 1 };

        /// Comparator: ALERT asserts above TSET and releases below THYST (thermostat).
        /// Interrupt: ALERT asserts on each crossing and is released by any register read.
        enum class AlertMode : uint8_t { Comparator = 0, Interrupt = 1 };

        /// Decoded CONFIG register. The default-constructed value matches the
        /// chip's power-up state except that it selects 12-bit resolution.
        struct Configuration
        {
            Resolution resolution = Resolution::Bits12;
            FaultQueue faultQueue = FaultQueue::Faults1;
            AlertPolarity alertPolarity = AlertPolarity::ActiveLow;
            AlertMode alertMode = AlertMode::Comparator;
            /// true stops continuous conversion (required before one-shot reads).
            bool shutdown = false;
        };

        /// 7-bit address 1001 A2 A1 A0 with all address pins low (0x48), stored
        /// left-adjusted (0x48 << 1) per the II2CAccess convention.
        static constexpr uint8_t DEFAULT_I2C_ADDRESS = 0x90;

        /// Left-adjusted address for the given A2/A1/A0 pin levels (0x90..0x9E).
        static constexpr uint8_t AddressFromPins(bool a2, bool a1, bool a0)
        {
            return static_cast<uint8_t>(0x90 | (a2 ? 0x08 : 0) | (a1 ? 0x04 : 0) | (a0 ? 0x02 : 0));
        }

        MCP9801(II2CAccess* i2c, uint8_t address = DEFAULT_I2C_ADDRESS);

        /// Write the configuration, read it back and verify the device answered.
        /// Use this as the presence check; the chip has no identification register.
        /// @return true if the device acknowledged and the read-back CONFIG matches.
        bool Initialize(const Configuration& config);
        /// Initialize with the default Configuration (12-bit, continuous, comparator).
        bool Initialize();
        /// Write CONFIG without verification. Clears the ONE-SHOT bit.
        bool Configure(const Configuration& config);
        /// Read and decode CONFIG. The ONE-SHOT bit is not reported.
        bool ReadConfiguration(Configuration& config);
        /// Change only the SHUTDOWN bit, preserving the rest of the configuration.
        bool SetShutdown(bool enabled);

        /// Read TA. In continuous mode this returns the latest completed
        /// conversion; in shutdown it returns the last value before shutdown.
        bool ReadTemperature(unitsnet_cpp::Temperature& temperature);
        /// Perform a single conversion from shutdown: enters shutdown if needed,
        /// sets ONE-SHOT, polls until the device clears it (bounded by the
        /// datasheet maximum conversion time), then reads TA. The device is left
        /// in shutdown. Requires Utility::Delay_ms to be assigned.
        bool ReadTemperatureOneShot(unitsnet_cpp::Temperature& temperature);

        /// TSET: ALERT asserts when TA exceeds this limit. 0.5 °C steps,
        /// -128 °C .. +127.5 °C, rounded to nearest and clamped. Power-up default 80 °C.
        bool SetLimit(unitsnet_cpp::Temperature limit);
        bool GetLimit(unitsnet_cpp::Temperature& limit);
        /// THYST: ALERT releases (comparator) or re-asserts (interrupt) when TA falls
        /// below this value. Same format as TSET. Power-up default 75 °C.
        bool SetHysteresis(unitsnet_cpp::Temperature hysteresis);
        bool GetHysteresis(unitsnet_cpp::Temperature& hysteresis);

        /// ITemperatureSensor: convenience wrapper around ReadTemperature,
        /// returns 0 °C on failure.
        unitsnet_cpp::Temperature GetTemperature() override;

        // Pure helpers, exposed for host testing.

        /// TA/TSET/THYST word -> °C. Two's complement integer MSB, fractional LSB
        /// (bit 7 = 0.5 °C ... bit 4 = 0.0625 °C), i.e. int16 / 256.
        static float RawToCelsius(uint16_t raw);
        /// °C -> 9-bit limit word (MSB integer, LSB bit 7 = 0.5 °C), rounded to the
        /// nearest 0.5 °C and clamped to the representable range.
        static uint16_t CelsiusToLimitRaw(float celsius);
        static uint8_t EncodeConfiguration(const Configuration& config);
        static Configuration DecodeConfiguration(uint8_t value);
        /// Datasheet maximum tCONV for the resolution (75/150/300/600 ms).
        static uint16_t MaxConversionTimeMs(Resolution resolution);

    private:
        II2CAccess* _i2cAccess;
        uint8_t _address;

        bool ReadRegister8(uint8_t reg, uint8_t& value);
        bool WriteRegister8(uint8_t reg, uint8_t value);
        bool ReadRegister16(uint8_t reg, uint16_t& value);
        bool WriteRegister16(uint8_t reg, uint16_t value);
        bool ReadTemperatureRegister(uint8_t reg, unitsnet_cpp::Temperature& temperature);
        bool WriteLimitRegister(uint8_t reg, unitsnet_cpp::Temperature value);
    };
}
