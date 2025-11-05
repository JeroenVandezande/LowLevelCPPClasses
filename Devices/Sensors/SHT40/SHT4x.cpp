//
// Created by DanaNatov on 2025-10-20.
//

#include "SHT4x.h"

#include "../../../Utilities/Delay.h"

#include <iterator>

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace Sensors
        {
            SHT4x::SHT4x(II2CAccess* i2c, uint8_t address)
            {
                _i2cAccess = i2c;
                _address = address;
            }

            bool SHT4x::CheckCRC(uint16_t data, uint8_t crc)
            {
                uint8_t calc = 0xFF;                // Initialization
                uint8_t bytes[2] = {
                    static_cast<uint8_t>(data >> 8),   // MSB first
                    static_cast<uint8_t>(data & 0xFF)  // LSB
                };

                for (uint8_t b : bytes) {
                    calc ^= b;
                    for (int i = 0; i < 8; ++i) {       // MSB-first, no reflection
                        if (calc & 0x80) {
                            calc = static_cast<uint8_t>((calc << 1) ^ 0x31);
                        } else {
                            calc <<= 1;
                        }
                    }
                }

                return (calc == crc);
            }

            void SHT4x::Reset()
            {
                uint8_t command = 0x94;
                _i2cAccess->I2C_WriteMethod(_address, &command, 1);
            }

           
            ///
            /// @brief Read temperature and relative humidity from the SHT4x sensor.
            ///
            /// This function triggers a single-shot measurement on the sensor using the specified
            /// precision, waits for the measurement to complete, reads back the measurement
            /// results, validates them using the CRC bytes provided by the sensor, and converts
            /// the raw values to engineering units (degrees Celsius and relative humidity).
            ///
            /// The function performs a single I2C write to request a measurement and then a single
            /// I2C read to obtain 6 bytes (temperature MSB, temperature LSB, temperature CRC,
            /// humidity MSB, humidity LSB, humidity CRC). If either the write or read operation
            /// fails, or if CRC validation fails for either measurement, the function returns false.
            ///
            /// Temperature conversion:
            ///   temperatureC = -45 + 175 * (temperatureRAW / 65535.0f)
            ///
            /// Relative humidity conversion:
            ///   humidity = (-6 + 125 * (humidityRAW / 65535.0f)) / 100.0f
            ///
            /// Note: The relative humidity return value is given as a fraction (e.g. 0.45 = 45% RH).
            ///
            /// @param[out] temperatureC Reference to float where the measured temperature in
            ///                         degrees Celsius will be stored on success.
            /// @param[out] humidity     Reference to float where the measured relative humidity
            ///                         (fraction, 0.0 to 1.0) will be stored on success.
            /// @param[in]  precision    Measurement precision/power trade-off selector. The
            ///                         precision determines which sensor command is used and
            ///                         affects measurement duration and power consumption:
            ///                           - HIGH   : highest precision (longest measurement time)
            ///                           - MEDIUM : medium precision
            ///                           - LOW    : lowest precision (shortest measurement time)
            ///
            /// @return true on success (measurements read and CRC validated), false on failure
            ///         (I2C write/read error or CRC mismatch).
            ///
            /// @remarks
            /// - The method performs an internal blocking delay to allow the sensor to complete
            ///   the measurement. Callers should expect this method to block for the sensor's
            ///   measurement time (the code issues Utility::Delay_ms(9) after the write).
            /// - Both CRC bytes provided by the sensor are validated; a failed CRC for either
            ///   measurement causes the function to return false.
            /// - This function alters the I2C bus state by performing a write followed by a read
            ///   and thus should not be called concurrently with other I2C operations on the
            ///   same bus without external synchronization.
            /// - Units:
            ///     temperatureC -> degrees Celsius
            ///     humidity     -> fraction (0.0 .. 1.0), where 1.0 == 100% RH
            ///
            /// @example
            /// float tC = 0.0f, rh = 0.0f;
            /// if (sensor.ReadTemperatureAndHumidity(tC, rh, SHT4x_Precision::HIGH)) {
            ///     // Use tC (°C) and rh (fraction)
            /// } else {
            ///     // Handle read/CRC/I2C failure
            /// }
            ///
            bool SHT4x::ReadTemperatureAndHumidity(float& temperatureC, float& humidity, SHT4x_Precision precision)
            {
                uint8_t data[6];
                uint8_t command = 0xFD;
                switch (precision)
                {
                    case SHT4x_Precision::HIGH:
                        command = 0xFD;
                        break;
                    case SHT4x_Precision::MEDIUM:
                        command = 0xF6;
                        break;
                    case SHT4x_Precision::LOW:
                        command = 0xE0;
                }

                bool writeSuccess = _i2cAccess->I2C_WriteMethod(_address, &command, 1);
                if (!writeSuccess) return false;
                Utility::Delay_ms(9);
                bool readSuccess = _i2cAccess->I2C_ReadMethod(_address, &data[0], 6);
                if (!readSuccess) return false;

                uint32_t temperatureRAW = static_cast<uint16_t>(data[0]) << 8 | data[1];
                uint32_t humidityRAW = static_cast<uint16_t>(data[3]) << 8 | data[4];
                // Check both CRC calculations
                if (CheckCRC(temperatureRAW, data[2]) && CheckCRC(humidityRAW, data[5]))
                {
                    // Get Relative Humidity
                    humidity = (-6 + 125 * (humidityRAW / 65535.0f)) / 100.0f;
                    temperatureC = -45 + 175 * (temperatureRAW / 65535.0f);
                    return true;
                }

                return false;
            }

                /**
                 * @brief Convenience wrapper that returns the current temperature (°C).
                 *
                 * This method performs a single-shot measurement using the sensor at HIGH
                 * precision and returns only the temperature component. Internally it calls
                 * ReadTemperatureAndHumidity(...) with SHT4x_Precision::HIGH and discards the
                 * humidity result.
                 *
                 * Behavior and failure modes:
                 * - The call is blocking because the underlying read function performs an
                 *   I2C write/read sequence and waits for the measurement to complete.
                 * - If the underlying measurement/read fails (I2C error or CRC mismatch),
                 *   the returned value will be the default-initialized temperature value
                 *   (0.0f in the current implementation). Callers that need to detect
                 *   read failures should call ReadTemperatureAndHumidity(...) directly.
                 *
                 * @return Measured temperature in degrees Celsius on success; default (0.0f)
                 *         if the underlying read fails.
                 */
                float SHT4x::GetTemperature()
                {
                    float humidity, temperature = 0;
                    ReadTemperatureAndHumidity(temperature, humidity, SHT4x_Precision::HIGH);
                    return temperature;
                }

                /**
                 * @brief Convenience wrapper that returns the current relative humidity.
                 *
                 * This method triggers a single-shot high-precision measurement on the sensor
                 * and returns only the humidity component. It internally calls
                 * ReadTemperatureAndHumidity(..., SHT4x_Precision::HIGH) and discards the
                 * temperature result.
                 *
                 * Behavior and guarantees:
                 * - The returned value is normalized/clamped to the valid range [0.0, 1.0],
                 *   where 1.0 corresponds to 100% relative humidity.
                 * - The call is blocking: the underlying read performs an I2C write/read
                 *   sequence and waits for the sensor measurement to complete.
                 * - If the underlying ReadTemperatureAndHumidity call fails (I2C error or
                 *   CRC mismatch), this method returns the default humidity value (0.0f).
                 *
                 * Usage notes:
                 * - If the caller needs to distinguish between a valid 0% reading and an
                 *   error, call ReadTemperatureAndHumidity(...) directly and check its
                 *   boolean return value.
                 * - The humidity value returned is a fraction in the range [0.0 .. 1.0].
                 *
                 * @return Relative humidity as a fraction (0.0 .. 1.0). Values outside the
                 *         physical range are clamped to that range.
                 */
                float SHT4x::GetHumidity()
                {
                    float humidity, temperature = 0;
                    ReadTemperatureAndHumidity(temperature, humidity, SHT4x_Precision::HIGH);
                    if (humidity < 0.0f)
                    {
                        humidity = 0.0f;
                    }
                    if (humidity > 1.0f)
                    {
                        humidity = 1.0f;
                    }
                    return humidity;
                }

            /// CAUTION: Use this feature sparingly (<10% of Operation Time)
            /// Returns temperature and humidity right before heater deactivation
            bool SHT4x::ActivateHeater(float& temperatureC, float& humidity, SHT4x_HeaterPreset preset)
            {
                uint8_t data[6];
                uint8_t command = 0xFD;
                switch (preset)
                {
                    case SHT4x_HeaterPreset::H20mW_100ms:
                        command = 0x15;
                    case SHT4x_HeaterPreset::H20mW_1000ms:
                        command = 0x1E;
                    case SHT4x_HeaterPreset::H110mW_100ms:
                        command = 0x24;
                    case SHT4x_HeaterPreset::H110mW_1000ms:
                        command = 0x2F;
                    case SHT4x_HeaterPreset::H200mW_100ms:
                        command = 0x32;
                    case SHT4x_HeaterPreset::H200mW_1000ms:
                        command = 0x39;
                }

                bool writeSuccess = _i2cAccess->I2C_WriteMethod(_address, &command, 1);
                if (!writeSuccess) return false;
                Utility::Delay_ms(9);
                bool readSuccess = _i2cAccess->I2C_ReadMethod(_address, &data[0], 6);
                if (!readSuccess) return false;
                uint32_t temperatureRAW = data[0] << 8 | data[1];
                uint32_t humidityRAW = data[3] << 8 | data[4];
                // Check both CRC calculations
                if (CheckCRC(temperatureRAW, data[2]) && CheckCRC(humidityRAW, data[5]))
                {
                    // Get Relative Humidity
                    humidity = -6 + 125 * (humidityRAW / 65535.0f);
                    temperatureC = -45 + 175 * (temperatureRAW / 65535.0f);
                    return true;
                }

                return false;
            }

            uint32_t SHT4x::GetSerialNumber()
            {
                uint8_t data[6];
                uint8_t command = 0x89;
                bool writeSuccess = _i2cAccess->I2C_WriteMethod(DEFAULT_I2C_ADDRESS, &command, 1);
                if (!writeSuccess) return 0;
                Utility::Delay_ms(9);
                bool readSuccess = _i2cAccess->I2C_ReadMethod(DEFAULT_I2C_ADDRESS, &data[0], 6);
                if (!readSuccess) return 0;
                uint32_t serialMSBs = data[0] << 8 | data[1];
                uint32_t serialLSBs = data[3] << 8 | data[4];
                // Check both CRC calculations
                if (CheckCRC(serialMSBs, data[2]) && CheckCRC(serialLSBs, data[5]))
                {
                    return serialMSBs << 16 | serialLSBs;
                }
                return 0;
            }
        }
    }
}