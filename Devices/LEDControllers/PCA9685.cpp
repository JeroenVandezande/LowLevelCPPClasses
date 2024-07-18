//
// Created by danan on 2024-07-18.
//

#include "PCA9685.h"
#include "math.h"

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace LedControllers
        {
            PCA9685::PCA9685(II2CAccess* i2CAccess,
                    uint8_t address,
                    uint16_t pwmfrequency,
                    enum PCA9685_OutputMode_t outputMode,
                    bool invertOutputs)
            {
                _I2CAccess = i2CAccess;
                _SlaveAddress = address;
                _PWMfrequency = pwmfrequency;
                _OutputMode = outputMode;
                _InvertOutputs = invertOutputs;

                _SlaveAddress = BASE_ADDRESS | (address << 1); //shift one bit to the left for R/W bit
                struct pca9685setupreg rd;
                rd.registeraddress = 0xFE; //prescaler
                rd.registerdata = (uint8_t)round(CHIPCLK / (4096 * pwmfrequency));
                _I2CAccess->I2C_WriteMethod(_SlaveAddress, reinterpret_cast<uint8_t*>(&rd), 2);
                rd.registeraddress = 0x00; //MODE1
                rd.registerdata = 0b00100000;
                _I2CAccess->I2C_WriteMethod(_SlaveAddress, reinterpret_cast<uint8_t*>(&rd), 2);
                rd.registeraddress = 0x01; //MODE2
                rd.registerdata = invertOutputs ? 1 << 4 : 0; //set output invert bit
                rd.registerdata |= (outputMode == PCA9685_OutputMode_TotemPole) ? 1 << 2 : 0; //set output drive bit
                _I2CAccess->I2C_WriteMethod(_SlaveAddress, reinterpret_cast<uint8_t*>(&rd), 2);
            }

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
                _I2CAccess->I2C_WriteMethod(_SlaveAddress, reinterpret_cast<uint8_t*>(&outdata), 5);
            }
        }
    }
}