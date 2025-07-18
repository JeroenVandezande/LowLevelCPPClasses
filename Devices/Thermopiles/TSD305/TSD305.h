//
// Created by DanaNatov on 2025-05-16.
//

#ifndef TSD305_H
#define TSD305_H

#include "../../../Base/LLE_I2C.h"
#include "TSD305_Constants.h"

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace Thermopiles
        {
            class TSD305 {
            private:
                // Calibration Values
                int16_t minSenTemp;
                int16_t maxSenTemp;
                int16_t minObjTemp;
                int16_t maxObjTemp;
                float tc;
                float tref;
                float coeffs[5] = {0,0,0,0,0};
                float objCoeffs[5] = {0,0,0,0,0};
                bool calibrationValuesSet;

                uint8_t i2cAddress;
                II2CAccess* i2cAccess;
                // Function pointer for microcontroller-specific wait function
                static void (*s_waitMs)(uint32_t milliseconds);
                TSD305_Constants::ErrorCode WriteWord(uint8_t data_address, uint16_t data);
                TSD305_Constants::ErrorCode ReadWord(uint8_t data_address, uint16_t& data);
                TSD305_Constants::ErrorCode ReadLongWord(uint8_t data_address, uint32_t& data);
                TSD305_Constants::ErrorCode RecalcCRC();
                TSD305_Constants::ErrorCode CheckStatusByte(uint8_t statusByte);

            public:
                int16_t MinSenTemp() const { return minSenTemp; }
                int16_t MaxSenTemp() const { return maxSenTemp; }
                int16_t MinObjTemp() const { return minObjTemp; }
                int16_t MaxObjTemp() const { return maxObjTemp; }
                float Tc() const { return tc; }
                float Tref() const { return tref; }
                float Coeffs(int index) const { return coeffs[index]; }
                float ObjCoeffs(int index) const { return objCoeffs[index]; }

                TSD305(II2CAccess *i2cAccess, uint8_t i2cAddress = TSD305_Constants::DEFAULT_I2C_ADDRESS);

                // Setter for the wait function pointer (Optional, needed for I2C Address Programming)
                static void SetWaitFunction(void (*waitFunctionPtr)(uint32_t));
                // Optional static wait helper method
                static void Wait(uint32_t milliseconds);

                TSD305_Constants::ErrorCode ReadTemperatureCoefficient(float& tc);
                TSD305_Constants::ErrorCode ReadReferenceTemperature(float& tref);
                TSD305_Constants::ErrorCode ReadSensorTempRange(int16_t& tmin, int16_t& tmax);
                TSD305_Constants::ErrorCode ReadObjectTempRange(int16_t& tmin, int16_t& tmax);
                TSD305_Constants::ErrorCode ReadCompensationCoefficients(float coeffs[5]);
                TSD305_Constants::ErrorCode ReadObjectTempCoefficients(float coeffs[5]);
                TSD305_Constants::ErrorCode ChangeI2CAddress(uint8_t newAddress);
                TSD305_Constants::ErrorCode GetMeasurement(uint32_t& objectADC, uint32_t& sensorADC);
                TSD305_Constants::ErrorCode RequestMeasurement(TSD305_Constants::MeasurementType type = TSD305_Constants::MeasurementType::SAMPLES_1);
                void ConvertMeasurement(uint32_t objectADC, uint32_t sensorADC, float& tSen, float& tObj);
            };
        }
    }
}



#endif //TSD305_H
