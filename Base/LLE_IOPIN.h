//
// Created by Jeroen Vandezande on 29/03/2022.
//

#pragma once

#include "LLE_PIO.h"

namespace LowLevelEmbedded
{
    class IOPIN
    {
    public:
        virtual ~IOPIN() = default;
        virtual void Set() = 0;
        virtual void Clear() = 0;
        virtual bool GetValue() = 0;
        virtual void Toggle() = 0;
    };

    class IOPIN_PIO8 : public IOPIN
    {
    private:
        IPIO* pio;
        uint8_t pin;

    public:
        IOPIN_PIO8(IPIO* pio, uint8_t pin)
        {
            this->pio = pio;
            this->pin = pin;
        }

        void Set();
        void Clear();
        bool GetValue();
        void Toggle();
    };
}
