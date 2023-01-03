//
// Created by Jeroen Vandezande on 09/12/2022.
//

#ifndef _SERIALLEDS_H_
#define _SERIALLEDS_H_

#include <stdint.h>

namespace LowLevelEmbedded
{
	namespace Devices
	{
		namespace LedControllers
		{
			enum SerialLeds_Colors
			{
				Red = 0x00FF00,
				Green = 0xFF0000,
				Blue = 0x0000FF
			};

			class SerialLeds
			{
			 private:
				const int LED_BYTE_SIZE = 24;
				const int LED_RESET_PULSE = 10;
				const uint8_t LOGIC_LED_0 = 0xC0;
				const uint8_t LOGIC_LED_1 = 0xF0;
			 public:
				SerialLeds(uint16_t amountOfLeds)
				{

				}
			};
		}
	}

} // LowLevelEmbedded

#endif //_SERIALLEDS_H_
