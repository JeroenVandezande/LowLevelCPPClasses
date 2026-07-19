//
// Created by Dana Natov on 2025-10-20.
//

#pragma once
#include <cstdint>
#include <functional>
#include <Duration.hpp>

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

        /// Delays in a UnitsNet-CPP Duration
        /// @param duration a UnitsNet-CPP Duration
        inline void Delay(const unitsnet_cpp::Duration duration)
        {
            if (duration < unitsnet_cpp::Duration::from_milliseconds(10))
            {
                Delay_us(static_cast<uint32_t>(duration.microseconds()));
            }
            else
            {
                Delay_ms(static_cast<uint32_t>(duration.milliseconds()));
            }
        }

        inline unitsnet_cpp::Duration TimeSinceSystemStart()
        {
            return unitsnet_cpp::Duration::from_milliseconds(static_cast<float>(millis()));
        }

    }
}

