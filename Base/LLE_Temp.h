//
// Created by DanaNatov on 2025-10-20.
//

#pragma once

#include <stdint.h>
#include <stddef.h>

namespace LowLevelEmbedded
{
    class ITemperatureSensor
    {
    public:
        virtual ~ITemperatureSensor() = default;
        virtual float GetTemperature() = 0;
    };
}
