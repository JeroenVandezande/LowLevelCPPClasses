//
// Created by DanaNatov on 2025-07-31.
//

#pragma once

#include <stdint.h>
#include "Color.h"

namespace LowLevelEmbedded::Devices::LedControllers::SerialLeds
{
    struct Color;

    class IColorProfile {
    public:
        virtual ~IColorProfile() = default;

        // How many bytes per LED
        virtual uint8_t GetBytesPerLED() const = 0;

        // Set the color in the given buffer at a specific LED index
        virtual void SetColor(uint8_t* buffer, uint16_t index, const Color& color) const = 0;

        // Convert a logical color (e.g. RGB) to physical format (e.g. GRB)
        virtual void MapColor(uint8_t* dest, const Color& color) const = 0;
    };

    class RGBProfile : public IColorProfile {
    public:
        uint8_t GetBytesPerLED() const override { return 3; }

        void SetColor(uint8_t* buffer, uint16_t index, const Color& color) const override {
            uint8_t* led = buffer + index * 3;
            led[0] = color.r; // R
            led[1] = color.g; // G
            led[2] = color.b; // B
        }

        void MapColor(uint8_t* dest, const Color& color) const override {
            dest[0] = color.r;
            dest[1] = color.g;
            dest[2] = color.b;
        }
    };

    class GRBProfile : public IColorProfile {
    public:
        uint8_t GetBytesPerLED() const override { return 3; }

        void SetColor(uint8_t* buffer, uint16_t index, const Color& color) const override {
            uint8_t* led = buffer + index * 3;
            led[0] = color.g; // G
            led[1] = color.r; // R
            led[2] = color.b; // B
        }

        void MapColor(uint8_t* dest, const Color& color) const override {
            dest[0] = color.g;
            dest[1] = color.r;
            dest[2] = color.b;
        }
    };

    class RGBWProfile : public IColorProfile {
    public:
        uint8_t GetBytesPerLED() const override { return 4; }

        void SetColor(uint8_t* buffer, uint16_t index, const Color& color) const override {
            uint8_t* led = buffer + index * 4;
            led[0] = color.r; // R
            led[1] = color.g; // G
            led[2] = color.b; // B
            led[3] = color.w; // W
        }

        void MapColor(uint8_t* dest, const Color& color) const override {
            dest[0] = color.r;
            dest[1] = color.g;
            dest[2] = color.b;
            dest[3] = color.w;
        }
    };

    class GRBWProfile : public IColorProfile {
    public:
        uint8_t GetBytesPerLED() const override { return 4; }

        void SetColor(uint8_t* buffer, uint16_t index, const Color& color) const override {
            uint8_t* led = buffer + index * 4;
            led[0] = color.g; // G
            led[1] = color.r; // R
            led[2] = color.b; // B
            led[3] = color.w; // W
        }

        void MapColor(uint8_t* dest, const Color& color) const override {
            dest[0] = color.g;
            dest[1] = color.r;
            dest[2] = color.b;
            dest[3] = color.w;
        }
    };
}

