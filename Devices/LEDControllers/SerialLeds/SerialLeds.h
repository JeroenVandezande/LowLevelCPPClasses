//
// Created by Jeroen Vandezande on 09/12/2022.
//

#ifndef _SERIALLEDS_H_
#define _SERIALLEDS_H_

#include "../../../Base/LLE_SPI.h"
#include <stdint.h>

#include "ColorProfiles.h"

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace LedControllers
        {
            namespace SerialLeds
            {
                struct Color;

                typedef enum {
                    COLOR_RGB,   // Red-Green-Blue
                    COLOR_BGR,   // Blue-Green-Red
                    COLOR_GRB,   // Green-Red-Blue
                    COLOR_RGBW,  // Red-Green-Blue-White
                    COLOR_BGRW,  // Blue-Green-Red-White
                    COLOR_GRBW,  // Green-Red-Blue-White
                    COLOR_TYPE_COUNT // Useful for bounds checking
                } ColorType;

                class SerialLED
                {
                private:
                    size_t _bufferSize;          // Size of the converted buffer
                    size_t _originalBufferSize;  // Size of the original buffer
                    uint8_t* _buffer;            // Original buffer
                    uint8_t* _convertedBuffer;   // Buffer with 4-bit representation
                    uint16_t _numberOfLeds;
                    IColorProfile* _colorProfile = nullptr;
                    LowLevelEmbedded::ISPIAccess* _spiAccess;

                    void ClearLEDBuffer();
                    void ConvertBuffer();  // Convert standard buffer to 4-bit representation

                public:
                    /**
                     * Constructor for the SerialLED class.
                     *
                     * Initializes the SerialLED instance with the provided SPI access, the number
                     * of LEDs, and the color type. Allocates the required buffer size for controlling
                     * the LEDs based on the provided color profile and clears the LED buffer.
                     *
                     * @param spiAccess Pointer to the SPI interface used for low-level communication with LEDs.
                     * @param amountOfLeds The number of LEDs to be managed by this instance.
                     * @param colorType The color type defining the order or configuration of color channels (e.g., RGB, GRB).
                     */
                    SerialLED(LowLevelEmbedded::ISPIAccess* spiAccess, uint16_t amountOfLeds, ColorType colorType);
                    ~SerialLED();

                    /**
                     * Turns off all LEDs by clearing the internal LED buffer and writing it to the SPI interface.
                     *
                     * This method clears the LED buffer by filling it with zero values, representing all LEDs turned off.
                     * The cleared buffer is then written to the SPI interface using the configured SPI access object.
                     * It ensures the proper SPI mode (Mode 0) is used during the write operation.
                     *
                     * The method interacts with low-level SPI hardware to effectively turn off all LEDs connected
                     * to the SerialLED controller.
                     */
                    void TurnOffAll();

                    /**
                     * Sets all LEDs in the strip to the specified color and writes the updated data to the SPI buffer for immediate output.
                     *
                     * This method iterates through all LEDs in the strip, applies the specified color using the associated color profile,
                     * and sends the prepared buffer to the SPI interface for rendering.
                     *
                     * @param color The color to set for all LEDs, represented as an instance of the Color struct.
                     */
                    void SetAndWriteAllToColor(const Color& color);

                    /**
                     * Sets the color of a specific LED at the given index and immediately sends the updated buffer
                     * data to the SPI device for rendering.
                     *
                     * @param index
                     *        The index of the LED to set (0-based).
                     *        Must be within the bounds of the configured number of LEDs.
                     *
                     * @param color
                     *        The desired Color object containing RGB(W) values for the specified LED.
                     *        The color is formatted and stored into the internal buffer for output.
                     *
                     * @throws std::out_of_range
                     *         Throws if the index is out of the configured range of LEDs.
                     *
                     * @note
                     *       This function internally calls SetLED to update the buffer and then immediately
                     *       writes the buffer to the SPI using WriteSPI with predefined SPI mode (Mode0).
                     */
                    void SetAndWriteLED(uint16_t index, const Color& color);

                    /**
                     * @brief Sets the color of a specific LED in the buffer.
                     *
                     * This function updates the LED buffer with the specified color for the LED at the given index.
                     * The changes made to the buffer will take effect only after writing the buffer to the SPI interface
                     * using the WriteBufferToSPI() or a function that writes the buffer.
                     *
                     * @param index The zero-based index of the LED to be updated. It specifies which LED in the chain
                     *              should have its color changed.
                     * @param color The color to assign to the specified LED, represented by an instance of the Color
                     *              struct. It contains RGB (and optionally white) values.
                     *
                     * @note The index must be within the valid range of LEDs configured for this instance of the
                     *       SerialLED class. If the index is out of range, the behavior is undefined.
                     *
                     * @attention This function does not send the changes to the physical LEDs. It only updates
                     *            the buffer. To apply the changes, the buffer must be written to the SPI bus.
                     */
                    void SetLED(uint16_t index, const Color& color);

                    /**
                     * @brief Transmits the LED data buffer to the SPI device.
                     *
                     * This method sends the internal LED data buffer to the SPI interface for controlling the
                     * connected LED strip. It uses the SPI interface provided during the object's creation
                     * to write the buffer data.
                     *
                     * The SPI communication mode used for this operation is SPIMode::Mode0.
                     * The buffer includes data for all LEDs as per the configured color profile along with
                     * any necessary reset pulses.
                     *
                     * Note:
                     * - The buffer must be properly populated with valid LED color data before calling this method.
                     * - The SPI interface used is expected to be implemented by an object conforming to the
                     *   ISPIAccess interface.
                     */
                    void WriteBufferToSPI();
                };
            }
        }
    }
} // LowLevelEmbedded

#endif //_SERIALLEDS_H_
