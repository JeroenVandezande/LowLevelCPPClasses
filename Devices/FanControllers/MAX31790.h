//
// Created by Jeroen Vandezande on 02/12/2022.
//

#ifndef _MAX31790_H_
#define _MAX31790_H_

#include "../../Base/LLE_I2C.h"
#include <stdint.h>

namespace LowLevelEmbedded
{
	namespace Devices
	{
		namespace FanControllers
		{
			enum class MAX31790_FanMode{PWMMode, RPMMode};

			enum class MAX31790_RateOfChange{Two_ms, Four_ms, Eight_ms, Fifteen_ms, ThirtyOne_ms, SixtyTwo_ms, HundredAndTwentyFive_ms};

			enum class MAX31790_NumberOfTachPeriods{four = 0b010, one = 0b000, two = 0b001, eight = 0b011, sixteen = 0b100, thirtyTwo = 0b111 };

			class MAX31790
			{
			 private:
				const uint8_t GLOBAL_CONFIGURATION_ADDRESS = 0x0;
				const uint8_t GC_I2CWATCHDOGSTATUS_SHIFT = 0;
				const uint8_t GC_I2CWATCHDOGPERIOD_SHIFT = 1; //two bits
				const uint8_t GC_OSC_SELECTION_SHIFT = 3;
				const uint8_t GC_BUS_TIMEOUT_SHIFT = 5;
				const uint8_t GC_RESET_SHIFT = 6;
				const uint8_t GC_STANDBY_SHIFT = 7;

				const uint8_t PWM_FREQUENCY_ADDRESS = 0x1;
				const uint8_t PWM13_FREQ_SHIFT = 0;
				const uint8_t PWM46_FREQ_SHIFT = 4;

				const uint8_t FAN1_CONFIGURATION_ADDRESS = 0x2;
				const uint8_t FAN2_CONFIGURATION_ADDRESS = 0x3;
				const uint8_t FAN3_CONFIGURATION_ADDRESS = 0x4;
				const uint8_t FAN4_CONFIGURATION_ADDRESS = 0x5;
				const uint8_t FAN5_CONFIGURATION_ADDRESS = 0x6;
				const uint8_t FAN6_CONFIGURATION_ADDRESS = 0x7;
				const uint8_t FC_TACH_SHIFT = 0;
				const uint8_t FC_LOCKED_ROTOR_POLARITY_SHIFT = 1;
				const uint8_t FC_LOCKED_ROTOR_SHIFT = 2;
				const uint8_t FC_TACH_INPUT_ENABLE_SHIFT = 3;
				const uint8_t FC_MONITOR_SHIFT = 4;
				const uint8_t FC_SPINUP_BEHAVIOR_SHIFT = 5;
				const uint8_t FC_RPM_MODE_SHIFT = 7;

				const uint8_t FAN1_DYNAMICS_ADDRESS = 0x8;
				const uint8_t FAN2_DYNAMICS_ADDRESS = 0x9;
				const uint8_t FAN3_DYNAMICS_ADDRESS = 0xA;
				const uint8_t FAN4_DYNAMICS_ADDRESS = 0xB;
				const uint8_t FAN5_DYNAMICS_ADDRESS = 0xC;
				const uint8_t FAN6_DYNAMICS_ADDRESS = 0xD;
				const uint8_t FD_ASYMETRIC_RATE_OF_CHANGE_SHIFT = 1;
				const uint8_t FD_PWM_RATE_OF_CHANGE_SHIFT = 2;
				const uint8_t FD_SPEED_RANGE_SHIFT = 5;

				const uint8_t FAN_FAULT_STATUS1_ADDRESS = 0x11;
				const uint8_t FAN_FAULT_STATUS2_ADDRESS = 0x10;
				const uint8_t FAN_FAULT_MASK1_ADDRESS = 0x13;
				const uint8_t FAN_FAULT_MASK2_ADDRESS = 0x12;
				const uint8_t FAILED_FAN_OPTIONS_ADDRESS = 0x14;
				const uint8_t FF_NUMBER_OF_QUALIFIED_FAULTS_SHIFT = 0;
				const uint8_t FF_SET_DUTY_CYCLE_ON_FAILURE_SHIFT = 2;
				const uint8_t FF_TIME_BETWEEN_SEQUENTIAL_FAN_STARTUP_SHIFT = 5;

				const uint8_t TACH1_COUNT_MSB_ADDRESS = 0x18;
				const uint8_t TACH1_COUNT_LSB_ADDRESS = 0x19;
				const uint8_t TACH2_COUNT_MSB_ADDRESS = 0x1A;
				const uint8_t TACH2_COUNT_LSB_ADDRESS = 0x1B;
				const uint8_t TACH3_COUNT_MSB_ADDRESS = 0x1C;
				const uint8_t TACH3_COUNT_LSB_ADDRESS = 0x1D;
				const uint8_t TACH4_COUNT_MSB_ADDRESS = 0x1E;
				const uint8_t TACH4_COUNT_LSB_ADDRESS = 0x1F;
				const uint8_t TACH5_COUNT_MSB_ADDRESS = 0x20;
				const uint8_t TACH5_COUNT_LSB_ADDRESS = 0x21;
				const uint8_t TACH6_COUNT_MSB_ADDRESS = 0x22;
				const uint8_t TACH6_COUNT_LSB_ADDRESS = 0x23;
				const uint8_t TACH7_COUNT_MSB_ADDRESS = 0x24;
				const uint8_t TACH7_COUNT_LSB_ADDRESS = 0x25;
				const uint8_t TACH8_COUNT_MSB_ADDRESS = 0x26;
				const uint8_t TACH8_COUNT_LSB_ADDRESS = 0x27;
				const uint8_t TACH9_COUNT_MSB_ADDRESS = 0x28;
				const uint8_t TACH9_COUNT_LSB_ADDRESS = 0x29;
				const uint8_t TACH10_COUNT_MSB_ADDRESS = 0x2A;
				const uint8_t TACH10_COUNT_LSB_ADDRESS = 0x2B;
				const uint8_t TACH11_COUNT_MSB_ADDRESS = 0x2C;
				const uint8_t TACH11_COUNT_LSB_ADDRESS = 0x2D;
				const uint8_t TACH12_COUNT_MSB_ADDRESS = 0x2E;
				const uint8_t TACH12_COUNT_LSB_ADDRESS = 0x2F;
				const uint8_t TACH_COUNT_SHIFT = 5;

				const uint8_t PWMOUT1_DUTYCYCLE_MSB_ADDRESS = 0x30;
				const uint8_t PWMOUT1_DUTYCYCLE_LSB_ADDRESS = 0x31;
				const uint8_t PWMOUT2_DUTYCYCLE_MSB_ADDRESS = 0x32;
				const uint8_t PWMOUT2_DUTYCYCLE_LSB_ADDRESS = 0x33;
				const uint8_t PWMOUT3_DUTYCYCLE_MSB_ADDRESS = 0x34;
				const uint8_t PWMOUT3_DUTYCYCLE_LSB_ADDRESS = 0x35;
				const uint8_t PWMOUT4_DUTYCYCLE_MSB_ADDRESS = 0x36;
				const uint8_t PWMOUT4_DUTYCYCLE_LSB_ADDRESS = 0x37;
				const uint8_t PWMOUT5_DUTYCYCLE_MSB_ADDRESS = 0x38;
				const uint8_t PWMOUT5_DUTYCYCLE_LSB_ADDRESS = 0x39;
				const uint8_t PWMOUT6_DUTYCYCLE_MSB_ADDRESS = 0x3A;
				const uint8_t PWMOUT6_DUTYCYCLE_LSB_ADDRESS = 0x3B;
				const uint8_t PWMOUT_DUTYCYCLE_SHIFT = 5;

				const uint8_t PWMOUT1_TARGET_DUTYCYCLE_MSB_ADDRESS = 0x40;
				const uint8_t PWMOUT1_TARGET_DUTYCYCLE_LSB_ADDRESS = 0x41;
				const uint8_t PWMOUT2_TARGET_DUTYCYCLE_MSB_ADDRESS = 0x42;
				const uint8_t PWMOUT2_TARGET_DUTYCYCLE_LSB_ADDRESS = 0x43;
				const uint8_t PWMOUT3_TARGET_DUTYCYCLE_MSB_ADDRESS = 0x44;
				const uint8_t PWMOUT3_TARGET_DUTYCYCLE_LSB_ADDRESS = 0x45;
				const uint8_t PWMOUT4_TARGET_DUTYCYCLE_MSB_ADDRESS = 0x46;
				const uint8_t PWMOUT4_TARGET_DUTYCYCLE_LSB_ADDRESS = 0x47;
				const uint8_t PWMOUT5_TARGET_DUTYCYCLE_MSB_ADDRESS = 0x48;
				const uint8_t PWMOUT5_TARGET_DUTYCYCLE_LSB_ADDRESS = 0x49;
				const uint8_t PWMOUT6_TARGET_DUTYCYCLE_MSB_ADDRESS = 0x4A;
				const uint8_t PWMOUT6_TARGET_DUTYCYCLE_LSB_ADDRESS = 0x4B;
				const uint8_t PWMOUT_TARGET_DUTYCYCLE_SHIFT = 7;

				const uint8_t TACH1_TARGET_COUNT_MSB_ADDRESS = 0x50;
				const uint8_t TACH1_TARGET_COUNT_LSB_ADDRESS = 0x51;
				const uint8_t TACH2_TARGET_COUNT_MSB_ADDRESS = 0x52;
				const uint8_t TACH2_TARGET_COUNT_LSB_ADDRESS = 0x53;
				const uint8_t TACH3_TARGET_COUNT_MSB_ADDRESS = 0x54;
				const uint8_t TACH3_TARGET_COUNT_LSB_ADDRESS = 0x55;
				const uint8_t TACH4_TARGET_COUNT_MSB_ADDRESS = 0x56;
				const uint8_t TACH4_TARGET_COUNT_LSB_ADDRESS = 0x57;
				const uint8_t TACH5_TARGET_COUNT_MSB_ADDRESS = 0x58;
				const uint8_t TACH5_TARGET_COUNT_LSB_ADDRESS = 0x59;
				const uint8_t TACH6_TARGET_COUNT_MSB_ADDRESS = 0x5A;
				const uint8_t TACH6_TARGET_COUNT_LSB_ADDRESS = 0x5B;
				const uint8_t TACH_TARGET_COUNT_SHIFT = 5;

				const uint8_t WINDOW1_MSB_ADDRESS = 0x60;
				const uint8_t WINDOW2_MSB_ADDRESS = 0x61;
				const uint8_t WINDOW3_MSB_ADDRESS = 0x62;
				const uint8_t WINDOW4_MSB_ADDRESS = 0x63;
				const uint8_t WINDOW5_MSB_ADDRESS = 0x64;
				const uint8_t WINDOW6_MSB_ADDRESS = 0x65;

				II2CAccess *_I2CAccess;

				uint8_t _SlaveAddress;
				uint8_t _NumberOfTachoPulsesPerRevolution[6];

				bool _writeToRegister(uint8_t reg, uint8_t data);
				uint8_t _readFromRegister(uint8_t reg);
				uint16_t _getFanSpeedRange(uint8_t fanID) const;

				//shadow registers
				uint8_t _FanConfigurationRegisters[6];
				uint8_t _FanDynamicsRegisters[6];

			 public:
				/// The constructor of the Fan Controller
				/// \param i2cAccess a Pointer to a II2CAccess implementation
				/// \param slaveAddres the 7 bit slave address of the device
				MAX31790(II2CAccess *i2cAccess, uint8_t slaveAddres);

				///
				/// \param fanID the zero based fan ID (0..5)
				/// \param fanmode the mode the controller operates in.
				/// \return true if no errors occurred.
				bool setFanMode(uint8_t fanID, MAX31790_FanMode fanmode);

				/// The device determines fan speed by counting the number of internal 8192Hz (fTOSC/4) clock cycles (using an 11-bit counter) during one or more fan tachometer periods.
				/// Three bits set the nominal RPM range for the fan, as shown in the table below.
				/// For example, a setting of 010b causes the device to count the number of 8192Hz (fTOSC/4) clock cycles that occur during four complete tachometer periods.
				/// If the fan has a nominal speed of 2000 RPM and two tachometer pulses per revolution, one tachometer period is nominally 15ms, and four tachometer periods are 60ms.
				/// With an 8192Hz (fTOSC/4) clock, the TACH count is therefore equal to 491. With a fan speed of 1/3 the nominal value, the count is 1474.
				/// If the fan’s nominal speed is 1000 RPM, the full-speed TACH count is 983.
				/// At 1/3 the nominal speed, there are 2948 clock cycles in four tachometer periods.
				/// This is greater than the maximum 11-bit count of 2047, so four tachometer periods is too many for this fan; a setting of 001 (two clock cycles) is recommended instead.
				/// \param fanID the zero based fan ID (0..5)
				/// \param numberOfTachPeriodsCounted a value between 1 and 32
				/// \return true if no errors occurred.
				bool setFanSpeedRange(uint8_t fanID, MAX31790_NumberOfTachPeriods numberOfTachPeriodsCounted);

				/// PWM duty cycle rate of change.
				/// The rate-of-change bits determine the time interval between duty cycle output increments/decrements.
				/// Regardless of the settings, there are a few cases for which the rate-of-change is always 0:
				/// In RPM mode, when a TACH target count of 2047 (7FFh) is selected, the duty cycle immediately goes to 0%. A full-scale target count is assumed to mean that the intent is to shut down the fan, and going directly to 0% avoids the possibility of loss of control-loop feedback at high TACH counts.
				/// If a slow-speed decrease toward 0% is desired, select a TACH target count at the slowest practical value for the fan. Once that count has been reached, selecting a count of 2047 (7FF) then takes the drive immediately to 0%.
				/// In PWM mode, when a target duty cycle of 0% is selected, the duty cycle goes to 0%. Again, it is assumed that the intent is to shut down the fan. If a slow-speed decrease toward 0% is desired, a target duty cycle of the slowest practical value for the fan in question should be chosen.
				/// Once that duty cycle has been reached, selecting a target value of 0% then takes the drive immediately to 0%.
				/// When the current duty cycle is 0% in PWM mode, selecting a new target duty cycle immediately takes the duty cycle to that value.
				/// The fan spins up first if spin-up is enabled.
				/// When the current duty cycle is 0% in RPM mode, selecting a new TACH target count that is less than 2047 (7FFh) immediately takes the duty cycle to the value in the PWMOUT Target Duty Cycle register. From this value, the duty cycle increments as needed to achieve the desired TACH target count. The fan spins up first if spin-up is enabled.
				/// \param fanID the zero based fan ID (0..5)
				/// \param rateOfChange The time between duty cycle increments
				/// \return true if no errors occurred.
				bool setFanRateOfChange(uint8_t fanID, MAX31790_RateOfChange rateOfChange);

				/// Sets the target speed of a fan.
				/// \param fanID the zero based fan ID (0..5)
				/// \param targetCount speed of the fan in rpm
				/// \return true if no errors occurred.
				bool setFanTargetRPM(uint8_t fanID, uint16_t rpm);

				/// When in PWM mode this method sets the PWM output of a fan
				/// \param fanID the zero based fan ID (0..5)
				/// \param pwm a number between 0 and 1, 0 = 0% 1 = 100%
				/// \return true if no errors occurred.
				bool setFanTargetPWM(uint8_t fanID, float pwm);

				/// sets the number of tacho pulses per revolution of a fan
				/// \param fanID the zero based fan ID (0..5)
				/// \param numberOfPulsesPerRevolution the number of pulses per revolution of the fan, typical 2 for most fans
				/// \return true if no errors occurred.
				void setFanPulsesPerRevolution(uint8_t fanID, uint8_t numberOfPulsesPerRevolution);

				/// If the absolute value of the speed difference is less than the value in the window register, then the update rate of the PWM duty cycle is slowed to 1 LSB per second
				/// \param fanID he zero based fan ID (0..5)
				/// \param window the speed delta below the regulator runs slower
				/// \return true if no errors occurred.
				bool setFanWindow(uint8_t fanID, uint8_t window);

				/// Get the actual speed of the fan.
				/// \param fanID the zero based fan ID (0..5)
				/// \return The fan speed in rpm.
				uint16_t getFanRPM(uint8_t fanID);
			};
		}
	}
}

#endif //_MAX31790_H_
