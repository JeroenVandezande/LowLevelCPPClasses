//
// Created by DanaNatov on 2025-10-20.
//

#pragma once

#include <Temperature.hpp>

namespace LowLevelEmbedded
{
    class ITemperatureSensor
    {
    public:
        virtual ~ITemperatureSensor() = default;
        virtual unitsnet_cpp::Temperature GetTemperature() = 0;
    };
}
