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
        inline std::function<void(uint32_t delay)> Delay_ms;
        inline std::function<void(uint32_t delay)> Delay_us;
    }
}

