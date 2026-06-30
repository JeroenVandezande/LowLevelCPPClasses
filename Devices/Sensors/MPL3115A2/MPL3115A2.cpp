//
// Created by DanaNatov on 2026-06-30.
//

#include "MPL3115A2.h"

#include "../../../Utilities/Delay.h"

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace Sensors
        {
            namespace
            {
                // Register map (NXP MPL3115A2 datasheet, Rev 8.1).
                constexpr uint8_t REG_STATUS      = 0x00; // DR_STATUS (auto-incr alias)
                constexpr uint8_t REG_OUT_P_MSB   = 0x01; // start of the 5-byte P/T burst
                constexpr uint8_t REG_WHO_AM_I    = 0x0C;
                constexpr uint8_t REG_PT_DATA_CFG = 0x13;
                constexpr uint8_t REG_CTRL_REG1   = 0x26;

                // CTRL_REG1 bit fields.
                constexpr uint8_t CTRL1_SBYB = 0x01; // 1 = active, 0 = standby
                constexpr uint8_t CTRL1_OST  = 0x02; // one-shot trigger (auto-clears)
                constexpr uint8_t CTRL1_RST  = 0x04; // software reset
                constexpr uint8_t CTRL1_OS_SHIFT = 3; // OS[2:0] at bits 5:3
                constexpr uint8_t CTRL1_ALT  = 0x80; // 1 = altimeter, 0 = barometer

                // PT_DATA_CFG: enable data-ready event flags for both channels.
                constexpr uint8_t PT_DATA_CFG_TDEFE = 0x01;
                constexpr uint8_t PT_DATA_CFG_PDEFE = 0x02;
                constexpr uint8_t PT_DATA_CFG_DREM  = 0x04;

                // STATUS: new pressure/altitude OR temperature data available.
                constexpr uint8_t STATUS_PTDR = 0x08;
            }

            MPL3115A2::MPL3115A2(II2CAccess* i2c, uint8_t address, MPL3115A2_Oversample oversample)
            {
                _i2cAccess = i2c;
                _address = address;
                _oversample = oversample;
            }

            bool MPL3115A2::ReadRegister(uint8_t reg, uint8_t* data, size_t length)
            {
                return _i2cAccess->I2C_Mem_Read(_address, reg, 1, data, length);
            }

            bool MPL3115A2::WriteRegister(uint8_t reg, uint8_t value)
            {
                return _i2cAccess->I2C_Mem_Write(_address, reg, 1, &value, 1);
            }

            uint16_t MPL3115A2::MaxConversionTimeMs(MPL3115A2_Oversample os)
            {
                // Minimum conversion time per datasheet ~= (2^OS * 4 ms) + 2 ms.
                // Add a margin so polling reliably outlasts the conversion.
                return static_cast<uint16_t>((1u << static_cast<uint8_t>(os)) * 4u + 10u);
            }

            uint8_t MPL3115A2::WhoAmI()
            {
                uint8_t id = 0;
                if (!ReadRegister(REG_WHO_AM_I, &id, 1)) return 0;
                return id;
            }

            bool MPL3115A2::Initialize()
            {
                if (WhoAmI() != DEVICE_ID) return false;
                // Place the device in standby (SBYB = 0) before changing configuration.
                if (!WriteRegister(REG_CTRL_REG1, 0x00)) return false;
                // Enable data-ready event flags so STATUS reflects fresh samples.
                return WriteRegister(REG_PT_DATA_CFG,
                    PT_DATA_CFG_DREM | PT_DATA_CFG_PDEFE | PT_DATA_CFG_TDEFE);
            }

            void MPL3115A2::Reset()
            {
                // The device immediately resets and does not ACK this write, so the
                // I2C transaction is expected to fail; ignore the result.
                WriteRegister(REG_CTRL_REG1, CTRL1_RST);
            }

            bool MPL3115A2::MeasureOnce(bool altimeter, uint8_t* outData)
            {
                // Trigger a single conversion from standby: SBYB stays 0, OST initiates
                // one measurement and self-clears when complete.
                uint8_t ctrl = static_cast<uint8_t>(_oversample) << CTRL1_OS_SHIFT;
                ctrl |= CTRL1_OST;
                if (altimeter) ctrl |= CTRL1_ALT;
                if (!WriteRegister(REG_CTRL_REG1, ctrl)) return false;

                // Poll STATUS until the new sample is ready (or we exceed the worst-case
                // conversion time).
                uint16_t maxWaitMs = MaxConversionTimeMs(_oversample);
                uint16_t waitedMs = 0;
                uint8_t status = 0;
                do
                {
                    Utility::Delay_ms(2);
                    waitedMs += 2;
                    if (!ReadRegister(REG_STATUS, &status, 1)) return false;
                } while (!(status & STATUS_PTDR) && waitedMs < maxWaitMs);

                if (!(status & STATUS_PTDR)) return false; // timed out

                // Burst-read OUT_P_MSB, OUT_P_CSB, OUT_P_LSB, OUT_T_MSB, OUT_T_LSB.
                return ReadRegister(REG_OUT_P_MSB, outData, 5);
            }

            ///
            /// @brief Read the absolute barometric pressure in Pascals.
            ///
            /// Triggers a one-shot measurement in barometer mode and converts the 20-bit
            /// result. The raw pressure is an unsigned Q18.2 fixed-point value (18 integer
            /// bits, 2 fractional bits) expressed in Pascals:
            ///   pressurePa = ((P_MSB << 12) | (P_CSB << 4) | (P_LSB >> 4)) / 4.0
            ///
            /// @param[out] pressurePa Absolute pressure in Pascals on success.
            /// @return true on success, false on I2C error or measurement timeout.
            ///
            bool MPL3115A2::ReadPressure(float& pressurePa)
            {
                uint8_t data[5];
                if (!MeasureOnce(false, data)) return false;

                uint32_t raw = (static_cast<uint32_t>(data[0]) << 12)
                             | (static_cast<uint32_t>(data[1]) << 4)
                             | (static_cast<uint32_t>(data[2]) >> 4);
                pressurePa = static_cast<float>(raw) / 4.0f;
                return true;
            }

            ///
            /// @brief Read altitude in meters.
            ///
            /// Triggers a one-shot measurement in altimeter mode and converts the 20-bit
            /// result. The raw altitude is a signed Q16.4 fixed-point value (16 integer
            /// bits, 4 fractional bits) in meters, referenced to the barometric input
            /// (default 101326 Pa):
            ///   altitudeM = (int32_t)((P_MSB << 24) | (P_CSB << 16) | (P_LSB << 8)) / 65536.0
            ///
            /// @param[out] altitudeM Altitude in meters on success.
            /// @return true on success, false on I2C error or measurement timeout.
            ///
            bool MPL3115A2::ReadAltitude(float& altitudeM)
            {
                uint8_t data[5];
                if (!MeasureOnce(true, data)) return false;

                int32_t raw = static_cast<int32_t>(
                      (static_cast<uint32_t>(data[0]) << 24)
                    | (static_cast<uint32_t>(data[1]) << 16)
                    | (static_cast<uint32_t>(data[2]) << 8));
                altitudeM = static_cast<float>(raw) / 65536.0f;
                return true;
            }

            ///
            /// @brief Read the die temperature in degrees Celsius.
            ///
            /// Triggers a one-shot measurement and converts the 12-bit temperature result,
            /// a signed Q8.4 fixed-point value (8 integer bits, 4 fractional bits):
            ///   temperatureC = (int16_t)((T_MSB << 8) | T_LSB) / 256.0
            ///
            /// @param[out] temperatureC Temperature in degrees Celsius on success.
            /// @return true on success, false on I2C error or measurement timeout.
            ///
            bool MPL3115A2::ReadTemperature(float& temperatureC)
            {
                uint8_t data[5];
                if (!MeasureOnce(false, data)) return false;

                int16_t raw = static_cast<int16_t>(
                    (static_cast<uint16_t>(data[3]) << 8) | data[4]);
                temperatureC = static_cast<float>(raw) / 256.0f;
                return true;
            }

            float MPL3115A2::GetTemperature()
            {
                float temperatureC = 0.0f;
                ReadTemperature(temperatureC);
                return temperatureC;
            }

            float MPL3115A2::GetPressure()
            {
                float pressurePa = 0.0f;
                ReadPressure(pressurePa);
                return pressurePa;
            }
        }
    }
}