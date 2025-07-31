//
// Created by DanaNatov on 2025-07-31.
//

#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace LedControllers
        {
            namespace SerialLeds
            {
                struct Color {
                    uint8_t r = 0;
                    uint8_t g = 0;
                    uint8_t b = 0;
                    uint8_t w = 0; // Optional, only used for RGBW/GRBW

                    Color() = default;

                    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t white = 0)
                        : r(red), g(green), b(blue), w(white) {}
                };
            }
        }
    }
}

#endif //COLOR_H