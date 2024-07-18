//
// Created by danan on 2024-07-18.
//

#include "PCA9685.h"

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace LedControllers
        {
            void PCA9685::WritePWM(uint8_t channel, uint16_t pwm)
            {
                struct pca9685datareg outdata;
                uint16_t start = channel * 256;
                uint16_t stop = start + pwm;
                if (stop > 4095)
                {
                    start = start - (stop - 4096);
                    stop = 4095;
                }
                outdata.start = start;
                outdata.stop = stop;
                if (pwm == 0)
                {
                    outdata.start = 0;
                    outdata.stop = 0b0001000000000000;
                }
                if (pwm > 4094)
                {
                    outdata.start = 0b0001000000000000;
                    outdata.stop = 0;
                }
                outdata.registeraddress = 6 + (4 * channel);
                _I2CAccess->I2C_WriteMethod(_SlaveAddress, &outdata, 5);
            }
        }
    }
}