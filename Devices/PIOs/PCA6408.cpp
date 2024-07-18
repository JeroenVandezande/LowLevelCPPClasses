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
                
            };
        }
    }
}