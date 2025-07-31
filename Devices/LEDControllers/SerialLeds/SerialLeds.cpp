//
// Created by Jeroen Vandezande on 09/12/2022.
//

#include "SerialLeds.h"

// Shift from LSB to MSB in color, assign an 8bit logic high or low byte and
// increment buffer pointer
#define ColorByte_FillBuffer(COLOR)\
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
}

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace LedControllers
        {
            namespace SerialLeds
            {
                const int LED_RESET_PULSE = 10;
                const uint8_t LOGIC_LED_0 = 0xC0;
                const uint8_t LOGIC_LED_1 = 0xF0;

                SerialLED::SerialLED(LowLevelEmbedded::ISPIAccess* spiAccess, uint16_t amountOfLeds, ColorType colorType)
                {
                    switch (colorType)
                    {
                        case COLOR_RGB:
                            _colorProfile = new RGBProfile();
                            break;
                        case COLOR_GRB:
                            _colorProfile = new GRBProfile();
                            break;
                        case COLOR_RGBW:
                            _colorProfile = new RGBWProfile();
                            break;
                        case COLOR_GRBW:
                            _colorProfile = new GRBWProfile();
                            break;
                        default:
                            _colorProfile = new RGBProfile();
                    }
                    _spiAccess = spiAccess;
                    _bufferSize = _colorProfile->GetBytesPerLED() * amountOfLeds + LED_RESET_PULSE;
                    _numberOfLeds = amountOfLeds;
                    // Allocate 192 OR 256 bits per LED address. 8 spi bits per LED protocol bit. 24bits x 8 OR 32bits x 8
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
                    _spiAccess->WriteSPI(_buffer, _bufferSize, 0, LowLevelEmbedded::SPIMode::Mode0);
                }

                void SerialLED::SetAndWriteAllToColor(const Color& color)
                {
                    for (int i = 0; i < _numberOfLeds; i++)
                    {
                        _colorProfile->SetColor(_buffer, i, color);
                    }

                    _spiAccess->WriteSPI(_buffer, _bufferSize, 0, LowLevelEmbedded::SPIMode::Mode0);
                }

                void SerialLED::SetAndWriteLED(uint16_t index, const Color& color)
                {
                    SetLED(index, color);
                    _spiAccess->WriteSPI(_buffer, _bufferSize, 0, LowLevelEmbedded::SPIMode::Mode0);
                }

                void SerialLED::SetLED(uint16_t index, const Color& color)
                {
                    _colorProfile->SetColor(_buffer, index, color);
                }

                void SerialLED::WriteBufferToSPI()
                {
                    _spiAccess->WriteSPI(_buffer, _bufferSize, 0, LowLevelEmbedded::SPIMode::Mode0);
                }
            }
        }
    }
} // LowLevelEmbedded
