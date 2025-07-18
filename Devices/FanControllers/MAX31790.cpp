//
// Created by Jeroen Vandezande on 02/12/2022.
//

#include "MAX31790.h"
#include "../../Base/LLE_tools.h"
#include <cmath>

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
					_NumberOfTachoPulsesPerRevolution[i] = 2;

					uint8_t tempBuffer = FAN1_CONFIGURATION_ADDRESS + i;
					_I2CAccess->I2C_WriteMethod(_SlaveAddress, &tempBuffer, 1); //set address in device for next read
					_I2CAccess->I2C_ReadMethod(_SlaveAddress, &_FanConfigurationRegisters[i], 1);

					tempBuffer = FAN1_DYNAMICS_ADDRESS + i;
					_I2CAccess->I2C_WriteMethod(_SlaveAddress, &tempBuffer, 1); //set address in device for next read
					_I2CAccess->I2C_ReadMethod(_SlaveAddress, &_FanDynamicsRegisters[i], 1);
				}
			}

			bool MAX31790::_writeToRegister(uint8_t reg, uint8_t data)
			{
				uint8_t tempBuffer[2] = {reg, data};
				return _I2CAccess->I2C_WriteMethod(_SlaveAddress, &tempBuffer[0], 2);
			}

			uint8_t MAX31790::_readFromRegister(uint8_t reg)
			{
				uint8_t result;
				_I2CAccess->I2C_WriteMethod(_SlaveAddress, &reg, 1); //set address in device for next read
				_I2CAccess->I2C_ReadMethod(_SlaveAddress, &result, 1);
				return result;
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
				return _writeToRegister(FAN1_CONFIGURATION_ADDRESS + fanID, _FanConfigurationRegisters[fanID]);
			}

			void  MAX31790::setFanPulsesPerRevolution(uint8_t fanID, uint8_t numberOfPulsesPerRevolution)
			{
				_NumberOfTachoPulsesPerRevolution[fanID] = numberOfPulsesPerRevolution;
			}

			bool MAX31790::setFanSpeedRange(uint8_t fanID, MAX31790_NumberOfTachPeriods numberOfTachPeriodsCounted)
			{
				uint8_t speedRange = (uint32_t)numberOfTachPeriodsCounted;
				_FanConfigurationRegisters[fanID] = (_FanConfigurationRegisters[fanID] & 0b00011111) | (speedRange << 5);
				return _writeToRegister(FAN1_DYNAMICS_ADDRESS + fanID, _FanConfigurationRegisters[fanID]);
			}

			bool MAX31790::setFanRateOfChange(uint8_t fanID, MAX31790_RateOfChange rateOfChange)
			{
				uint8_t rot = (uint8_t)rateOfChange;
				_FanConfigurationRegisters[fanID] = (_FanConfigurationRegisters[fanID] & 0b11100011) | (rot << 2);
				return _writeToRegister(FAN1_DYNAMICS_ADDRESS + fanID, _FanConfigurationRegisters[fanID]);
			}

			bool MAX31790::setFanTargetRPM(uint8_t fanID, uint16_t rpm)
			{
				uint16_t SR = _getFanSpeedRange(fanID);

				float speedAsFloat = (60.0 / (_NumberOfTachoPulsesPerRevolution[fanID] * rpm)) * SR * 8192;
				uint16_t speedAsInt = round(speedAsFloat);
				if(speedAsInt > 2047) speedAsInt = 2047;
				uint8_t lowerByte = (uint8_t) (speedAsInt << 5);
				uint8_t higherByte = (uint8_t) (speedAsInt >> 3);
				if(!_writeToRegister(TACH1_TARGET_COUNT_MSB_ADDRESS + (fanID * 2), higherByte)) return false;
				if(!_writeToRegister(TACH1_TARGET_COUNT_LSB_ADDRESS + (fanID * 2), lowerByte)) return false;
				return true;
			}

			bool MAX31790::setFanTargetPWM(uint8_t fanID, float pwm)
			{
				uint16_t pwmAsInt = round(pwm * 511);
				uint8_t lowerByte = (uint8_t) (pwmAsInt << 7);
				uint8_t higherByte = (uint8_t) (pwmAsInt >> 1);
				if(!_writeToRegister(PWMOUT1_TARGET_DUTYCYCLE_MSB_ADDRESS + (fanID * 2), higherByte)) return false;
				return _writeToRegister(PWMOUT1_TARGET_DUTYCYCLE_LSB_ADDRESS + (fanID * 2), lowerByte);
			}

			bool MAX31790::setFanWindow(uint8_t fanID, uint8_t window)
			{
				return _writeToRegister(WINDOW1_MSB_ADDRESS + fanID, window);
			}

			uint16_t MAX31790::_getFanSpeedRange(uint8_t fanID) const
			{
				uint16_t SR;
				switch ((MAX31790_NumberOfTachPeriods)(_FanConfigurationRegisters[fanID] >> 5))
				{
					case MAX31790_NumberOfTachPeriods::one:
						SR = 1;
						break;
					case MAX31790_NumberOfTachPeriods::two:
						SR = 2;
						break;
					case MAX31790_NumberOfTachPeriods::four:
						SR = 4;
						break;
					case MAX31790_NumberOfTachPeriods::eight:
						SR = 8;
						break;
					case MAX31790_NumberOfTachPeriods::sixteen:
						SR = 16;
						break;
					case MAX31790_NumberOfTachPeriods::thirtyTwo:
						SR=32;
						break;
					default:
						SR = 4;
				}
				return SR;
			}

			uint16_t MAX31790::getFanRPM(uint8_t fanID)
			{
				uint16_t SR = _getFanSpeedRange(fanID);
				uint16_t TC_MSB = _readFromRegister(TACH1_COUNT_MSB_ADDRESS + (fanID * 2));
				uint16_t TC_LSB = _readFromRegister(TACH1_COUNT_LSB_ADDRESS + (fanID * 2));
				uint16_t TC = (TC_LSB >> 5) | (TC_MSB << 3);
				float rpmAsFloat = (60 * SR * 8192) / (TC * _NumberOfTachoPulsesPerRevolution[fanID]);
				return round(rpmAsFloat);
			}

		}
	}
}