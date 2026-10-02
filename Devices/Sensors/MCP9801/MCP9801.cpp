//
// Created by Dana Natov on 2026-10-02.
//

#include "MCP9801.h"

#include "../../../Utilities/Delay.h"

#include <cmath>

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace Sensors
        {
            namespace
            {
                // Register pointer values (Microchip MCP9800/1/2/3 datasheet, DS21909D).
                constexpr uint8_t REG_TA     = 0x00; // Ambient temperature, 16-bit, read-only
                constexpr uint8_t REG_CONFIG = 0x01; // Configuration, 8-bit
                constexpr uint8_t REG_THYST  = 0x02; // Temperature hysteresis, 16-bit
                constexpr uint8_t REG_TSET   = 0x03; // Temperature limit-set, 16-bit

                // CONFIG bit fields.
                constexpr uint8_t CONFIG_SHUTDOWN         = 0x01;
                constexpr uint8_t CONFIG_INTERRUPT_MODE   = 0x02; // 0 = comparator
                constexpr uint8_t CONFIG_ALERT_ACTIVE_HIGH = 0x04; // 0 = active-low
                constexpr uint8_t CONFIG_FAULT_QUEUE_SHIFT = 3;    // bits 4:3
                constexpr uint8_t CONFIG_RESOLUTION_SHIFT  = 5;    // bits 6:5
                constexpr uint8_t CONFIG_ONE_SHOT         = 0x80; // self-clearing

                constexpr uint8_t POLL_INTERVAL_MS = 5;
            }

            MCP9801::MCP9801(II2CAccess* i2c, uint8_t address)
            {
                _i2cAccess = i2c;
                _address = address;
            }

            // ---------------------------------------------------------------------
            // Pure helpers
            // ---------------------------------------------------------------------

            float MCP9801::RawToCelsius(uint16_t raw)
            {
                // Sign in bit 15, integer part in bits 14:8, fraction in bits 7:4
                // (unused fraction bits read as 0). Equivalent to int16 / 256.
                return static_cast<float>(static_cast<int16_t>(raw)) / 256.0f;
            }

            uint16_t MCP9801::CelsiusToLimitRaw(float celsius)
            {
                // Limit registers hold 9 bits: integer in the MSB, 0.5 °C in LSB bit 7.
                float halves = std::round(celsius * 2.0f);
                if (halves > 255.0f) halves = 255.0f;   // +127.5 °C
                if (halves < -256.0f) halves = -256.0f; // -128 °C
                auto raw = static_cast<int16_t>(halves) * 128; // halves << 7, sign preserved
                return static_cast<uint16_t>(raw);
            }

            uint8_t MCP9801::EncodeConfiguration(const Configuration& config)
            {
                uint8_t value = 0;
                value |= static_cast<uint8_t>(static_cast<uint8_t>(config.resolution) << CONFIG_RESOLUTION_SHIFT);
                value |= static_cast<uint8_t>(static_cast<uint8_t>(config.faultQueue) << CONFIG_FAULT_QUEUE_SHIFT);
                if (config.alertPolarity == AlertPolarity::ActiveHigh) value |= CONFIG_ALERT_ACTIVE_HIGH;
                if (config.alertMode == AlertMode::Interrupt) value |= CONFIG_INTERRUPT_MODE;
                if (config.shutdown) value |= CONFIG_SHUTDOWN;
                return value;
            }

            MCP9801::Configuration MCP9801::DecodeConfiguration(uint8_t value)
            {
                Configuration config;
                config.resolution = static_cast<Resolution>((value >> CONFIG_RESOLUTION_SHIFT) & 0x03);
                config.faultQueue = static_cast<FaultQueue>((value >> CONFIG_FAULT_QUEUE_SHIFT) & 0x03);
                config.alertPolarity = (value & CONFIG_ALERT_ACTIVE_HIGH) ? AlertPolarity::ActiveHigh : AlertPolarity::ActiveLow;
                config.alertMode = (value & CONFIG_INTERRUPT_MODE) ? AlertMode::Interrupt : AlertMode::Comparator;
                config.shutdown = (value & CONFIG_SHUTDOWN) != 0;
                return config;
            }

            uint16_t MCP9801::MaxConversionTimeMs(Resolution resolution)
            {
                // Datasheet tCONV maximum: 75, 150, 300, 600 ms for 9..12 bits.
                return static_cast<uint16_t>(75u << static_cast<uint8_t>(resolution));
            }

            // ---------------------------------------------------------------------
            // Register access
            // ---------------------------------------------------------------------

            bool MCP9801::ReadRegister8(uint8_t reg, uint8_t& value)
            {
                return _i2cAccess->I2C_Mem_Read(_address, reg, 1, &value, 1);
            }

            bool MCP9801::WriteRegister8(uint8_t reg, uint8_t value)
            {
                return _i2cAccess->I2C_Mem_Write(_address, reg, 1, &value, 1);
            }

            bool MCP9801::ReadRegister16(uint8_t reg, uint16_t& value)
            {
                uint8_t data[2];
                if (!_i2cAccess->I2C_Mem_Read(_address, reg, 1, data, 2)) return false;
                value = static_cast<uint16_t>((static_cast<uint16_t>(data[0]) << 8) | data[1]); // MSB first
                return true;
            }

            bool MCP9801::WriteRegister16(uint8_t reg, uint16_t value)
            {
                uint8_t data[2] = {
                    static_cast<uint8_t>(value >> 8),
                    static_cast<uint8_t>(value & 0xFF)
                };
                return _i2cAccess->I2C_Mem_Write(_address, reg, 1, data, 2);
            }

            bool MCP9801::ReadTemperatureRegister(uint8_t reg, unitsnet_cpp::Temperature& temperature)
            {
                uint16_t raw;
                if (!ReadRegister16(reg, raw)) return false;
                temperature = unitsnet_cpp::Temperature::from_degrees_celsius(RawToCelsius(raw));
                return true;
            }

            bool MCP9801::WriteLimitRegister(uint8_t reg, unitsnet_cpp::Temperature value)
            {
                return WriteRegister16(reg, CelsiusToLimitRaw(value.degrees_celsius()));
            }

            // ---------------------------------------------------------------------
            // Configuration
            // ---------------------------------------------------------------------

            bool MCP9801::Initialize(const Configuration& config)
            {
                if (!Configure(config)) return false;
                uint8_t readback;
                if (!ReadRegister8(REG_CONFIG, readback)) return false;
                // The ONE-SHOT bit is written as 0 and self-clears, so a full compare is valid.
                return readback == EncodeConfiguration(config);
            }

            bool MCP9801::Initialize()
            {
                return Initialize(Configuration{});
            }

            bool MCP9801::Configure(const Configuration& config)
            {
                return WriteRegister8(REG_CONFIG, EncodeConfiguration(config));
            }

            bool MCP9801::ReadConfiguration(Configuration& config)
            {
                uint8_t value;
                if (!ReadRegister8(REG_CONFIG, value)) return false;
                config = DecodeConfiguration(value);
                return true;
            }

            bool MCP9801::SetShutdown(bool enabled)
            {
                Configuration config;
                if (!ReadConfiguration(config)) return false;
                config.shutdown = enabled;
                return Configure(config);
            }

            // ---------------------------------------------------------------------
            // Measurements
            // ---------------------------------------------------------------------

            bool MCP9801::ReadTemperature(unitsnet_cpp::Temperature& temperature)
            {
                return ReadTemperatureRegister(REG_TA, temperature);
            }

            ///
            /// @brief Take a single on-demand measurement with the device otherwise shut down.
            ///
            /// Follows the datasheet sequence (section 5.3.4.2): the device must first be in
            /// shutdown (SHUTDOWN = 1, ONE-SHOT = 0), then CONFIG is written again with both
            /// SHUTDOWN and ONE-SHOT set. The device clears ONE-SHOT when TA has been updated.
            /// This method polls CONFIG for that, bounded by the maximum conversion time of the
            /// configured resolution, and then reads TA.
            ///
            /// @return true on success, false on I2C error or conversion timeout.
            ///
            bool MCP9801::ReadTemperatureOneShot(unitsnet_cpp::Temperature& temperature)
            {
                Configuration config;
                if (!ReadConfiguration(config)) return false;
                if (!config.shutdown)
                {
                    config.shutdown = true;
                    if (!Configure(config)) return false;
                }
                uint8_t base = EncodeConfiguration(config);
                if (!WriteRegister8(REG_CONFIG, static_cast<uint8_t>(base | CONFIG_ONE_SHOT))) return false;

                uint16_t maxWaitMs = MaxConversionTimeMs(config.resolution);
                uint16_t waitedMs = 0;
                uint8_t status;
                do
                {
                    Utility::Delay_ms(POLL_INTERVAL_MS);
                    waitedMs = static_cast<uint16_t>(waitedMs + POLL_INTERVAL_MS);
                    if (!ReadRegister8(REG_CONFIG, status)) return false;
                } while ((status & CONFIG_ONE_SHOT) && waitedMs < maxWaitMs);

                if (status & CONFIG_ONE_SHOT) return false; // timed out

                return ReadTemperature(temperature);
            }

            unitsnet_cpp::Temperature MCP9801::GetTemperature()
            {
                auto temperature = unitsnet_cpp::Temperature::from_degrees_celsius(0.0f);
                ReadTemperature(temperature);
                return temperature;
            }

            // ---------------------------------------------------------------------
            // Alert limits
            // ---------------------------------------------------------------------

            bool MCP9801::SetLimit(unitsnet_cpp::Temperature limit)
            {
                return WriteLimitRegister(REG_TSET, limit);
            }

            bool MCP9801::GetLimit(unitsnet_cpp::Temperature& limit)
            {
                return ReadTemperatureRegister(REG_TSET, limit);
            }

            bool MCP9801::SetHysteresis(unitsnet_cpp::Temperature hysteresis)
            {
                return WriteLimitRegister(REG_THYST, hysteresis);
            }

            bool MCP9801::GetHysteresis(unitsnet_cpp::Temperature& hysteresis)
            {
                return ReadTemperatureRegister(REG_THYST, hysteresis);
            }
        }
    }
}
