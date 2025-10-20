//
// Created by Jeroen Vandezande on 09/12/2022.
//

#include "SerialLeds.h"

// Shift from LSB to MSB in color, assign an 4bit logic high or low and
// increment buffer pointer
#define ColorByte_FillBuffer(COLOR)\
{\
    uint8_t bits = COLOR;\
    bool byteFilled = false;\
    uint8_t logicData = 0;\
    for (int pos = 7 ; pos >= 0 ; pos--) \
    {\
        if (bits & (1 << pos)) \
        {\
            logicData = LOGIC_LED_1;\
        }\
        else\
        {\
            logicData = LOGIC_LED_0;\
        }\
        if (byteFilled)\
        {\
            *Ptr = logicData << 4;\
            *Ptr++;\
        }\
        else\
        {\
            *Ptr = logicData;\
        }\
        byteFilled = !byteFilled;\
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
                // 4-bit representation for 0 and 1
                const uint8_t LOGIC_LED_0 = 0x8;  // Binary 1000
                const uint8_t LOGIC_LED_1 = 0xE;  // Binary 1110
                
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
                        case COLOR_BGRW:
                            _colorProfile = new BGRWProfile();
                            break;
                        default:
                            _colorProfile = new RGBProfile();
                    }
                    _spiAccess = spiAccess;
                    
                    // Original buffer size
                    _originalBufferSize = _colorProfile->GetBytesPerLED() * amountOfLeds + LED_RESET_PULSE;
                    _numberOfLeds = amountOfLeds;
                    
                    // Allocate original buffer
                    _buffer = new uint8_t[_originalBufferSize];
                    
                    // Calculate converted buffer size
                    // Each bit becomes 4 bits, so 8 bits -> 32 bits (4 bytes)
                    // Total new size is 4 times the original size
                    _bufferSize = _originalBufferSize * 4;
                    
                    // Allocate converted buffer
                    _convertedBuffer = new uint8_t[_bufferSize];
                    
                    TurnOffAll();
                }
                
                SerialLED::~SerialLED()
                {
                    delete[] _buffer;
                    delete[] _convertedBuffer;
                    delete _colorProfile;
                }

                void SerialLED::ClearLEDBuffer()
                {
                    for (int i = 0; i < _originalBufferSize; ++i)
                    {
                        _buffer[i] = 0;
                    }
                    
                    for (int i = 0; i < _bufferSize; ++i)
                    {
                        _convertedBuffer[i] = 0;
                    }
                }
                
                // Convert the regular buffer to the 4-bit representation
                // Each original bit becomes a 4-bit pattern (0x8 or 0xE)
                // Two 4-bit patterns are packed into one byte
                void SerialLED::ConvertBuffer()
                {
                    size_t convertedIndex = 0;
                    uint8_t currentByte = 0;
                    bool isFirstNibble = true;
                    
                    // Process each byte in the original buffer
                    for (size_t i = 0; i < _originalBufferSize; i++)
                    {
                        uint8_t byte = _buffer[i];
                        
                        // Process each bit in the byte
                        for (int bitPos = 7; bitPos >= 0; bitPos--)
                        {
                            // Get the bit value (0 or 1)
                            bool isBitSet = (byte & (1 << bitPos)) != 0;
                            
                            // Choose the 4-bit pattern based on the bit value
                            uint8_t pattern = isBitSet ? LOGIC_LED_1 : LOGIC_LED_0;
                            
                            if (isFirstNibble)
                            {
                                // For the first nibble (upper 4 bits), shift left by 4
                                currentByte = pattern << 4;
                                isFirstNibble = false;
                            }
                            else
                            {
                                // For the second nibble (lower 4 bits), just OR it with the current byte
                                currentByte |= pattern;
                                
                                // Store the completed byte (which contains two 4-bit patterns)
                                _convertedBuffer[convertedIndex++] = currentByte;
                                
                                // Reset for the next byte
                                isFirstNibble = true;
                            }
                        }
                    }
                    
                    // If we ended with a half-filled byte (only upper nibble set),
                    // we need to store it too
                    if (!isFirstNibble)
                    {
                        _convertedBuffer[convertedIndex] = currentByte;
                    }
                }

                void SerialLED::TurnOffAll()
                {
                    // Fill buffer with 0s
                    ClearLEDBuffer();
                    
                    // Convert buffer to 4-bit format
                    ConvertBuffer();

                    // Send the converted buffer to SPI
                    _spiAccess->WriteSPI(_convertedBuffer, _bufferSize, 0, LowLevelEmbedded::SPIMode::Mode0);
                }

                void SerialLED::SetAndWriteAllToColor(const Color& color)
                {
                    for (int i = 0; i < _numberOfLeds; i++)
                    {
                        _colorProfile->SetColor(_buffer, i, color);
                    }
                    
                    // Convert buffer to 4-bit format
                    ConvertBuffer();

                    _spiAccess->WriteSPI(_convertedBuffer, _bufferSize, 0, LowLevelEmbedded::SPIMode::Mode0);
                }

                void SerialLED::SetAndWriteLED(uint16_t index, const Color& color)
                {
                    SetLED(index, color);
                    
                    // Convert buffer to 4-bit format
                    ConvertBuffer();
                    
                    _spiAccess->WriteSPI(_convertedBuffer, _bufferSize, 0, LowLevelEmbedded::SPIMode::Mode0);
                }

                void SerialLED::SetLED(uint16_t index, const Color& color)
                {
                    _colorProfile->SetColor(_buffer, index, color);
                }

                void SerialLED::WriteBufferToSPI()
                {
                    // Convert buffer to 4-bit format
                    ConvertBuffer();
                    
                    _spiAccess->WriteSPI(_convertedBuffer, _bufferSize, 0, LowLevelEmbedded::SPIMode::Mode0);
                }
            }
        }
    }
} // LowLevelEmbedded
