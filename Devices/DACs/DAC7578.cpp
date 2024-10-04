//
// Created by Jeroen Vandezande on 03/01/2023.
//

#include "DAC7578.h"


namespace LowLevelEmbedded
{
	namespace Devices
	{
		namespace DACs
		{
			const uint8_t CA_WRITEANDUPDATE = 0b00110000;

			DAC7578::DAC7578(II2CAccess* i2cAccess, uint8_t slaveAddres)
			{
				_I2CAccess = i2cAccess;
				_SlaveAddress = slaveAddres;
			}

			bool DAC7578::InitDAC()
			{
				return true;
			}

			bool DAC7578::WriteDAC(uint8_t channel, uint16_t value)
			{
				uint8_t data[3];
				data[0] = CA_WRITEANDUPDATE | channel;
				data[1] = (value & 0x0FF0) >> 4;
				data[2] = (value & 0x000F) << 4;
 				return _I2CAccess->I2C_WriteMethod(_SlaveAddress, &data[0], 3);
			}

		}
	}
}