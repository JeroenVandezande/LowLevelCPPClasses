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
                    humidity = -6 + 125 * (humidityRAW / 65535.0f);
                    temperatureC = -45 + 175 * (temperatureRAW / 65535.0f);
                    return true;
                }

                return false;
            }

            void SHT4x::GetTemperature(float& temperature)
            {
                float humidity = 0;
                ReadTemperatureAndHumidity(temperature, humidity, SHT4x_Precision::HIGH);
            }


            /// CAUTIION: Use this feature sparingly (<10% of Operation Time)
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