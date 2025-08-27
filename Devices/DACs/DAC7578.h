//
// Created by Jeroen Vandezande on 03/01/2023.
//

#ifndef _DAC7578_H_
#define _DAC7578_H_

#include "LLE_I2C.h"
#include <stdint.h>

#include "LLE_DAC.h"

namespace LowLevelEmbedded::Devices::DACs
{
    /**
     *
     */
    class DAC7578 : public LowLevelEmbedded::IDAC<uint16_t>
    {
    private:
        II2CAccess* _I2CAccess;
        uint8_t _SlaveAddress;
        float _ReferenceVoltage;
        float _VoltagePerBit;

    public:
        /// The constructor of the DAC
				/// \param i2cAccess a Pointer to a II2CAccess implementation
				/// \param slaveAddres the 7 bit slave address of the device
				/// \param referenceVoltage the reference voltage the DAC uses
        DAC7578(II2CAccess* i2cAccess, uint8_t slaveAddres, float referenceVoltage);

        /// The method will write data to a DAC channel and update the output
				/// \param channel a value between 0 and 7 to indicate the DAC channel
				/// \param value a value between 0 and 4095
				/// \return true of no errors
        bool WriteDAC(uint8_t channel, uint16_t value) override;

        /**
         * @brief Retrieves the maximum digital-to-analog (DA) value supported by the DAC.
         *
         * This method provides the maximum value that can be written to the DAC, determined
         * by its hardware resolution and implementation (e.g., 12-bit or 16-bit DACs).
         *
         * @return The maximum digital-to-analog value supported by the DAC as an unsigned short.
         */
        unsigned short GetMaxDAValue() override;
        /**
         * @brief Retrieves the maximum number of channels supported by the DAC.
         *
         * This method returns the total number of output channels available on the DAC,
         * which can be used to select and configure the appropriate channel for output.
         *
         * @return The maximum number of channels supported as an unsigned 8-bit integer.
         */
        uint8_t GetMaxChannels() override;
        /**
         * @brief Writes a specific voltage value to a DAC channel.
         *
         * This method converts the requested voltage into a digital value
         * and writes it to the specified channel of the DAC, updating the output.
         *
         * @param channel The DAC channel to write to, typically in the range of 0 to the maximum number of channels supported by the device.
         * @param value The voltage value to write, as a float, within the supported range of the DAC.
         * @return True if the operation was successful, false otherwise.
         */
        bool WriteDACVoltage(uint8_t channel, float value) override;
        /**
         * @brief Creates a channel object for the specified DAC channel.
         *
         * This method initializes and returns a pointer to an IDACChannel object
         * that represents the specified channel of the DAC. The channel can then
         * be used for further operations such as writing values or querying
         * properties.
         *
         * @param channel The DAC channel number to create the object for, typically in the range supported by the DAC device.
         * @return A pointer to an IDACChannel object of type uint16_t representing the specified channel.
         */
        IDACChannel<uint16_t>* CreateChannelObject(uint8_t channel) override;

        /// Initializes the DAC over the I2C bus
				/// \return true if no errors
        bool InitDAC();
    };
}

#endif //_DAC7578_H_
