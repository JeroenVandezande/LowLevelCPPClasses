//
// Created by Jeroen Vandezande on 09/12/2022.
//

#include "SerialLeds.h"

// Shift from LSB to MSB in color, assign an 8bit logic high or low byte and increment buffer pointer
#define IN_PI42TAS_FillBuffer(COLOR)\
{\
    uint8_t bits = COLOR;\
    for (int pos = 7 ; pos >= 0 ; pos--) \
    {\
        if (bits & (1 << pos)) \
        {\
            *Ptr++ = LOGIC_LED_1;   \
        } \
        else \
        {                           \
            *Ptr++ = LOGIC_LED_0;\
        }\
    }\
}\

namespace LowLevelEmbedded
{
	namespace Devices
	{
		namespace LedControllers
		{
			const int LED_BYTE_SIZE = 24;
			const int LED_RESET_PULSE = 10;
			const uint8_t LOGIC_LED_0 = 0xC0;
			const uint8_t LOGIC_LED_1 = 0xF0;

			enum IN_PI42TAS_Colors_t
			{
				Red = 0x00FF00,
				Green = 0xFF0000,
				Blue = 0x0000FF
			};

			SerialLED::SerialLED(LowLevelEmbedded::ISPIAccess* spiAccess, uint16_t amountOfLeds)
			{
				_spiAccess = spiAccess;
				_bufferSize = (LED_BYTE_SIZE * amountOfLeds) + LED_RESET_PULSE;
				_numberOfLeds = amountOfLeds;
				// Allocate 192 bits per LED address. 8 spi bits per LED protocol bit. 24bits x 8
				_buffer = new uint8_t[_bufferSize];
				TurnOffAll();
			}

			void SerialLED::ClearLEDBuffer()
			{
				for (int i = 0; i < _bufferSize; ++i)
				{
					_buffer[i] = 0;
				}
			}

			void SerialLED::TurnOffAll()
			{
				// Fill buffer with 0s
				ClearLEDBuffer();

				// Set SPI buffer
				_spiAccess->WriteSPI(_buffer,_bufferSize, 0, LowLevelEmbedded::SPIMode::Mode0);
			}

			void SerialLED::FillBufferForLED(uint16_t index, uint8_t color[3])
			{
				uint8_t *Ptr = _buffer + 24 * index;
				IN_PI42TAS_FillBuffer(color[0]); //g
				IN_PI42TAS_FillBuffer(color[1]); //r
				IN_PI42TAS_FillBuffer(color[2]); //b
			}

			void SerialLED::SetAndWriteAllToColor(uint8_t color[3])
			{
				for(int i = 0; i < _numberOfLeds; i++)
				{
					FillBufferForLED(i, color);
				}

				_spiAccess->WriteSPI(_buffer, _bufferSize, 0, LowLevelEmbedded::SPIMode::Mode0);
			}

			void SerialLED::SetAndWriteLED(uint16_t index, uint8_t color[3])
			{
				FillBufferForLED(index, color);
				_spiAccess->WriteSPI(_buffer, _bufferSize, 0, LowLevelEmbedded::SPIMode::Mode0);
			}

			void SerialLED::SetLED(uint16_t index, uint8_t color[3])
			{
				FillBufferForLED(index, color);
			}

			void SerialLED::WriteBufferToSPI()
			{
				_spiAccess->WriteSPI(_buffer, _bufferSize, 0, LowLevelEmbedded::SPIMode::Mode0);
			}
		}
	}
} // LowLevelEmbedded