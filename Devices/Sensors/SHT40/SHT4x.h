//
// Created by DanaNatov on 2025-10-20.
//

#ifndef HOODCONNECTIONMASTER_SHT40_H
#define HOODCONNECTIONMASTER_SHT40_H
#include "LLE_I2C.h"
#include "LLE_Temp.h"

#include <cstdint>

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace Sensors
        {
            typedef enum {
                LOW,
                MEDIUM,
                HIGH,
            } SHT4x_Precision;

            typedef enum {
                H20mW_100ms,
                H20mW_1000ms,
                H110mW_100ms,
                H110mW_1000ms,
                H200mW_100ms,
                H200mW_1000ms,
            } SHT4x_HeaterPreset;

            class SHT4x : ITempSensor
            {
                private:
                    bool CheckCRC(uint16_t data, uint8_t crc);
                    II2CAccess* _i2cAccess;
                    uint8_t _address;
                public:
                    static constexpr uint8_t DEFAULT_I2C_ADDRESS = 0x88;
                    SHT4x(II2CAccess* i2c, uint8_t address = DEFAULT_I2C_ADDRESS);
                    void Reset();
                    bool ReadTemperatureAndHumidity(float& temperatureC, float& humidity, SHT4x_Precision precision = HIGH);
                    bool ActivateHeater(float& temperatureC, float& humidity, SHT4x_HeaterPreset preset);
                    void GetTemperature(float& temperature) override;
                    uint32_t GetSerialNumber();
            };
        }
    }
}


#endif //HOODCONNECTIONMASTER_SHT40_H