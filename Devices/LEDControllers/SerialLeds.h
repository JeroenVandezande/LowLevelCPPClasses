//
// Created by Jeroen Vandezande on 09/12/2022.
//

#ifndef _SERIALLEDS_H_
#define _SERIALLEDS_H_

#include "../../Base/LLE_SPI.h"
#include <stddef.h>
#include <stdint.h>

namespace LowLevelEmbedded
{
	namespace Devices
	{
		namespace LedControllers
		{
			class SerialLED
			{
			 private:
				size_t _bufferSize;
				uint8_t* _buffer;
				uint16_t _numberOfLeds;
				LowLevelEmbedded::ISPIAccess* _spiAccess;
				void ClearLEDBuffer();
				void FillBufferForLED(uint16_t index, uint8_t color[3]);
			 public:
				/// constructor
				SerialLED(LowLevelEmbedded::ISPIAccess* spiAccess, uint16_t amountOfLeds);

				/// Turns off all LEDs in the string and resets the byte buffer
				void TurnOffAll();

				/// Sets all of the LEDs to a certain color and writes the buffer to SPI
				/// \param color 24 bit uint for representing color (GRB)
				void SetAndWriteAllToColor(uint8_t color[3]);

				/// Sets a particular LED to a certain color and writes the entire buffer to SPI (SLOW)
				/// \param index The index of the LED in the chain
				/// \param color 24 bit uint for representing color (GRB)
				void SetAndWriteLED(uint16_t index, uint8_t color[3]);

				/// Sets a particular LED to a certain color without writing to SPI. Use with WriteBufferToSPI
				/// \param index The index of the LED in the chain
				/// \param color 24 bit uint for representing color (GRB)
				void SetLED(uint16_t index, uint8_t color[3]);

				/// Writes the buffer to SPI
				void WriteBufferToSPI();
			};
		}
	}

} // LowLevelEmbedded

#endif //_SERIALLEDS_H_
