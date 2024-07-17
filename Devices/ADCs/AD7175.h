//
// Created by Jeroen on 2024-02-22.
//

#ifndef AD7175_H
#define AD7175_H

//CRC constants
#include <functional>

#include "LLE_IOPIN.h"
#include "LLE_SPI.h"

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace ADCs
        {
            /**
             * @class AD7175
             * @brief This class represents the AD7175 ADC device.
             */
            class AD7175
            {
            private:
                ISPIAccess* SPIAccess;
                uint8_t csID;
            public:
                /**
                 * @brief Runs during the constructor allowing for control of configuration.
                 */
                const std::function<void>* ConfigurationMethod;

                /**
                 * @brief Represents a ADC GPIO pin.
                 *
                 * The GPIO0 variable is an instance of the IOPIN class that represents a GPIO pin on the ADC.
                 * This variable can be used to manipulate the state of the GPIO pin, such as setting
                 * it high or low, toggling its state, or reading its current value.
                 */
                IOPIN *GPIO0;
                /**
                 * @brief Represents a ADC GPIO pin.
                 *
                 * The GPIO0 variable is an instance of the IOPIN class that represents a GPIO pin on the ADC.
                 * This variable can be used to manipulate the state of the GPIO pin, such as setting
                 * it high or low, toggling its state, or reading its current value.
                 */
                IOPIN *GPIO1;

                /**
                 * @brief Constructor for the AD7175 class.
                 * @param spi_access Pointer to an object implementing the ISPIAccess interface
                 */
                AD7175(ISPIAccess *spi_access, uint8_t cs_ID, const std::function<void>* configureMethod);

                void WriteRegister8(uint8_t reg, uint8_t value);
                void WriteRegister16(uint8_t reg, uint16_t value);
                void WriteRegister24(uint8_t reg, uint32_t value);
                int8_t ReadRegister8(uint8_t reg);
                int16_t ReadRegister16(uint8_t reg);
                int32_t ReadRegister24(uint8_t reg);

                uint32_t GetADCValue(uint8_t channelIndex);
            };
        }
    }
}


#endif //AD7175_H
