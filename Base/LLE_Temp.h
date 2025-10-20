//
// Created by DanaNatov on 2025-10-20.
//

#pragma once

#include <stdint.h>
#include <stddef.h>

namespace LowLevelEmbedded
{
    class ITempSensor
    {
    public:
        virtual ~ITempSensor()
        {
        };
        virtual void GetTemperature(float& temperature) = 0;
    };
}
