//
// Created by Jeroen Vandezande on 03/01/2023.
//

#ifndef _DAC7578_H_
#define _DAC7578_H_

#include <stdint.h>
#include "../../LLE_I2C.h"

namespace LowLevelEmbedded
{
	namespace Devices
	{
		namespace DACs
		{
			class DAC7578
			{
			 private:
				II2CAccess *_I2CAccess;
				uint8_t _SlaveAddress;
			 public:
				/// The constructor of the DAC
				/// \param i2cAccess a Pointer to a II2CAccess implementation
				/// \param slaveAddres the 7 bit slave address of the device
				DAC7578(II2CAccess* i2cAccess, uint8_t slaveAddres);

				/// The method will write data to a DAC channel and update the output
				/// \param channel a value between 0 and 7 to indicate the DAC channel
				/// \param value a value between 0 and 4095
				/// \return true of no errors
				bool WriteDAC(uint8_t channel, uint16_t value);

				/// Initializes the DAC over the I2C bus
				/// \return true if no errors
				bool InitDAC();
			};
		}

	}
}

#endif //_DAC7578_H_
