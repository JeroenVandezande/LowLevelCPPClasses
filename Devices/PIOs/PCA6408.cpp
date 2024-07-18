//
// Created by danan on 2024-07-18.
//

#include "PCA6408.h"

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace PIOs
        {
            bool PCA6408::WritePort(uint8_t data)
            {
                uint8_t buffer[2] = { PCA6408A_Registers_OutputPort, data };
                return i2cAccess->I2C_WriteMethod(i2cAddress, &buffer[0], 2);
            };

            bool PCA6408::ReadPort(uint8_t* data)
            {
                uint8_t buffer = { PCA6408A_Registers_InputPort };
                bool success = i2cAccess->I2C_WriteMethod(i2cAddress, &buffer, 1);
                if (!success)
                { return false; }
                return i2cAccess->I2C_ReadMethod(i2cAddress, data, 1);
            };

            bool PCA6408::ConfigurePort(uint8_t config)
            {
                uint8_t buffer[2] = { PCA6408A_Registers_Configuration, config };
                return i2cAccess->I2C_WriteMethod(i2cAddress, &buffer[0], 2);
            }

            bool PCA6408::ConfigurePortPolarity(uint8_t polarityConfig)
            {
                uint8_t buffer[2] = { PCA6408A_Registers_PolarityInversion, polarityConfig };
                return i2cAccess->I2C_WriteMethod(i2cAddress, &buffer[0], 2);
            }
        }
    }
}