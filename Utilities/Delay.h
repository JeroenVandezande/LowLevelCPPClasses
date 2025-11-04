//
// Created by DanaNatov on 2025-10-20.
//

#pragma once
#include <cstdint>
#include <functional>

namespace LowLevelEmbedded
{
    namespace Utility
    {
        /// Usage: LowLevelEmbedded::Utility::Delay_ms = HAL_Delay;
        inline std::function<void(uint32_t delay)> Delay_ms;

        /// Usage: LowLevelEmbedded::Utility::Delay_us = DelayUs_DWT;
        inline std::function<void(uint32_t delay)> Delay_us;

       /// returns the number of milliseconds since the system started
        inline std::function<uint32_t()> millis;

    }
}

