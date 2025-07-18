//
// Created by danan on 2024-07-18.
//

#ifndef PCA6408_H
#define PCA6408_H

#include "../../Base/LLE_I2C.h"

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace PIOs
        {
            class PCA6408
            {
            private:
                uint8_t i2cAddress;
                II2CAccess* i2cAccess;
            public:
                PCA6408(II2CAccess *i2cAccess, bool addrPinHigh)
                {
                    this->i2cAccess = i2cAccess;

                    // Calculate I2C address using the status of the address pin
                    const uint8_t fixedpartofaddress = 0b01000000; //defined by the device manufacturer
                    i2cAddress = addrPinHigh ? (fixedpartofaddress | 2) : fixedpartofaddress;
                }
                bool WritePort(uint8_t data);
                bool ReadPort(uint8_t* data);
                bool ConfigurePort(uint8_t config);
                bool ConfigurePortPolarity(uint8_t polarityConfig);
            };

            enum PCA6408A_Registers_t
            {
                PCA6408A_Registers_InputPort = 0,
                PCA6408A_Registers_OutputPort = 1,
                PCA6408A_Registers_PolarityInversion = 2,
                PCA6408A_Registers_Configuration = 3
            };
        }
    }
}





#endif //PCA6408_H
