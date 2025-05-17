//
// Created by DanaNatov on 2025-05-16.
//

#ifndef TSD305_H
#define TSD305_H

#include "LLE_I2C.h"
#include "TSD305_Constants.h"

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace Thermopiles
        {
            class TSD305 {
            private:
                uint8_t i2cAddress;
                II2CAccess* i2cAccess;

                TSD305_Constants::ErrorCode WriteByte(uint8_t data_address, uint8_t data);
                TSD305_Constants::ErrorCode ReadWord(uint8_t data_address, uint16_t& data);
                TSD305_Constants::ErrorCode ReadLongWord(uint8_t data_address, uint32_t& data);
                TSD305_Constants::ErrorCode RecalcCRC();
                TSD305_Constants::ErrorCode CheckStatusByte(uint8_t statusByte);

            public:
                TSD305(II2CAccess *i2cAccess, uint8_t i2cAddress = TSD305_Constants::DEFAULT_I2C_ADDRESS);

                TSD305_Constants::ErrorCode ReadTemperatureCoefficient(float& tc);
                TSD305_Constants::ErrorCode ReadReferenceTemperature(float& tref);
                TSD305_Constants::ErrorCode ReadSensorTempRange(int16_t& tmin, int16_t& tmax);
                TSD305_Constants::ErrorCode ReadObjectTempRange(int16_t& tmin, int16_t& tmax);
                TSD305_Constants::ErrorCode ReadCompensationCoefficients(float coeffs[5]);
                TSD305_Constants::ErrorCode ReadObjectTempCoefficients(float coeffs[5]);
                TSD305_Constants::ErrorCode ChangeI2CAddress(uint8_t newAddress);
                TSD305_Constants::ErrorCode PerformMeasurement(uint32_t& objectADC, uint32_t& sensorADC,
                    TSD305_Constants::MeasurementType type = TSD305_Constants::MeasurementType::SAMPLES_1);
            };
        }
    }
}



#endif //TSD305_H
