//
// Created by Jeroen Vandezande on 02/12/2022.
//

#ifndef _MAX31790_H_
#define _MAX31790_H_

#include <stdint.h>

namespace LowLevelEmbedded
{
	namespace Devices
	{
		namespace PIOs
		{
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
				const uint8_t WINDOW1_MSB_ADDRESS = 0x60;
				const uint8_t WINDOW2_MSB_ADDRESS = 0x61;
				const uint8_t WINDOW3_MSB_ADDRESS = 0x62;
				const uint8_t WINDOW4_MSB_ADDRESS = 0x63;
				const uint8_t WINDOW5_MSB_ADDRESS = 0x64;
				const uint8_t WINDOW6_MSB_ADDRESS = 0x65;
			};
		}
	}
}

#endif //_MAX31790_H_
