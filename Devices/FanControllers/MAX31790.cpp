//
// Created by Jeroen Vandezande on 02/12/2022.
//

#include "MAX31790.h"
#include "tools.h"

namespace LowLevelEmbedded
{
	namespace Devices
	{
		namespace FanControllers
		{

			//Constructor
			MAX31790::MAX31790(II2CAccess *i2cAccess, uint8_t slaveAddres)
			{
				_I2CAccess = i2cAccess;
				_SlaveAddress = slaveAddres;
				//read shadow registers from device
				for (int i = 0; i < 6; ++i)
				{
					uint8_t tempBuffer = FAN1_CONFIGURATION_ADDRESS + i;
					_I2CAccess->I2C_WriteMethod(_SlaveAddress, &tempBuffer, 1); //set address in device for next read
					_I2CAccess->I2C_ReadMethod(_SlaveAddress, &_FanConfigurationRegisters[i], 1);

					tempBuffer = FAN1_DYNAMICS_ADDRESS + i;
					_I2CAccess->I2C_WriteMethod(_SlaveAddress, &tempBuffer, 1); //set address in device for next read
					_I2CAccess->I2C_ReadMethod(_SlaveAddress, &_FanDynamicsRegisters[i], 1);
				}
			}

			bool MAX31790::writeToRegister(uint8_t reg, uint8_t data)
			{
				uint8_t tempBuffer[2] = {reg, data};
				return _I2CAccess->I2C_WriteMethod(_SlaveAddress, &tempBuffer[0], 2);
			}

			bool MAX31790::setFanMode(uint8_t fanID, MAX31790_FanMode fanmode)
			{
				switch (fanmode)
				{
					case MAX31790_FanMode::PWMMode:
						CLEAR_BIT(_FanConfigurationRegisters[fanID], FC_RPM_MODE_SHIFT);
						break;
					case MAX31790_FanMode::RPMMode:
						SET_BIT(_FanConfigurationRegisters[fanID], FC_RPM_MODE_SHIFT);
						break;
				}
				return writeToRegister(FAN1_CONFIGURATION_ADDRESS + fanID, _FanConfigurationRegisters[fanID]);
			}

			bool MAX31790::setFanSpeedRange(uint8_t fanID, uint8_t speedRange)
			{
				_FanConfigurationRegisters[fanID] = (_FanConfigurationRegisters[fanID] & 0b00011111) | (speedRange << 5);
				return writeToRegister(FAN1_DYNAMICS_ADDRESS + fanID, _FanConfigurationRegisters[fanID]);
			}

			bool MAX31790::setFanRateOfChange(uint8_t fanID, MAX31790_RateOfChange rateOfChange)
			{
				uint8_t rot = (u_int8_t)rateOfChange;
				_FanConfigurationRegisters[fanID] = (_FanConfigurationRegisters[fanID] & 0b11100011) | (rot << 2);
				return writeToRegister(FAN1_DYNAMICS_ADDRESS + fanID, _FanConfigurationRegisters[fanID]);
			}

		}
	}
}