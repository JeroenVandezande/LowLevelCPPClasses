//
// Created by danan on 2024-07-18.
//

#ifndef PCA9685_H
#define PCA9685_H

#include "../../Base/LLE_I2C.h"

#define BASE_ADDRESS 0b10000000
#define CHIPCLK 25000000

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace LedControllers
        {
            struct __attribute__((__packed__)) pca9685datareg {
                uint8_t registeraddress;
                uint16_t start;
                uint16_t stop;
            };

            struct __attribute__((__packed__)) pca9685setupreg {
                uint8_t registeraddress;
                uint8_t registerdata;
            };

            enum PCA9685_OutputMode_t
            {
                PCA9685_OutputMode_OpenDrain,
                PCA9685_OutputMode_TotemPole
            };

            class PCA9685
            {
            private:
                II2CAccess *_I2CAccess;
                uint8_t _SlaveAddress;
                uint16_t _PWMfrequency;
                enum PCA9685_OutputMode_t _OutputMode;
                bool _InvertOutputs;
            public:
                /// Constructor of the PCA9685 I2C LED Controller
                /// \param i2CAccess The I2C access object with method points for abstracted I2C Calls
                /// \param address The I2C Slave address
                /// \param pwmfrequency The desired PWM Frequency (Between 25 - 1526Hz) (PRE_SCALE)
                /// \param outputMode Sets up Open Drain vs Totem Pole output types (OUTDRV)
                /// \param invertOutputs Inverts the output logic state (INVRT)
                PCA9685(II2CAccess* i2CAccess,
                    uint8_t address,
                    uint16_t pwmfrequency,
                    enum PCA9685_OutputMode_t outputMode,
                    bool invertOutputs);
                void WritePWM(uint8_t channel, uint16_t pwm);
            };
        }
    }
}

#endif //PCA9685_H
