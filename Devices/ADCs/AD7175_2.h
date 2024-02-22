//
// Created by Jeroe on 2024-02-22.
//

#ifndef AD7175_2_H
#define AD7175_2_H

//CRC constants
#define AD7175_CRC_POLYNOMIAL 0x07 // x^8 + x^2 + x +1 (MSB first)
#define AD7175_CRC_CHECK_CODE 0xF3
#include "IOPIN.h"
#include "SPI.h"

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace ADCs
        {
            /**
             * @class AD7175_2
             * @brief This class represents the AD7175_2 ADC device.
             */
            class AD7175_2
            {
            public:
                /**
                 * @brief Represents a GPIO pin.
                 *
                 * The GPIO0 variable is an instance of the IOPIN class that represents a GPIO pin.
                 * This variable can be used to manipulate the state of the GPIO pin, such as setting
                 * it high or low, toggling its state, or reading its current value.
                 */
                IOPIN *GPIO0;
                /**
                 * @brief Represents a GPIO pin.
                 *
                 * The GPIO0 variable is an instance of the IOPIN class that represents a GPIO pin.
                 * This variable can be used to manipulate the state of the GPIO pin, such as setting
                 * it high or low, toggling its state, or reading its current value.
                 */
                IOPIN *GPIO1;

                /**
                 * @brief Constructor for the AD7175_2 class.
                 * @param cs_ID The chip select ID.
                 * @param spi_access Pointer to an object implementing the ISPIAccess interface
                 */
                AD7175_2(uint8_t cs_ID, ISPIAccess *spi_access);
            };
        }
    }
}


#endif //AD7175_2_H
