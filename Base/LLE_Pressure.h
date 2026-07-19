//
// Created by Dana Natov on 2026-06-30.
//

#pragma once

#include <Pressure.hpp>

namespace LowLevelEmbedded
{
    /**
     * @class IPressureSensor
     * @brief An interface for sensing barometric (absolute) pressure.
     *
     * This interface defines a contract for pressure sensors to implement. It provides
     * the ability to read the absolute pressure in unitsnet_cpp::Pressure.
     */
    class IPressureSensor
    {
    public:
        virtual ~IPressureSensor() = default;
        /**
         * @brief Retrieves the current absolute pressure measurement.
         *
         * This method returns the absolute pressure in unitsnet_cpp::Pressure. Sensors implementing
         * this method should provide real-time or near real-time data for the current
         * environmental conditions.
         *
         * @return A unitsnet_cpp::Pressure representing the absolute pressure.
         */
        virtual unitsnet_cpp::Pressure GetPressure() = 0;
    };
}