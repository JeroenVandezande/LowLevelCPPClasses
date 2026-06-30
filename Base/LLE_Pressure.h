//
// Created by DanaNatov on 2026-06-30.
//

#pragma once

namespace LowLevelEmbedded
{
    /**
     * @class IPressureSensor
     * @brief An interface for sensing barometric (absolute) pressure.
     *
     * This interface defines a contract for pressure sensors to implement. It provides
     * the ability to read the absolute pressure in Pascals (Pa).
     */
    class IPressureSensor
    {
    public:
        virtual ~IPressureSensor() = default;
        /**
         * @brief Retrieves the current absolute pressure measurement.
         *
         * This method returns the absolute pressure in Pascals (Pa). Sensors implementing
         * this method should provide real-time or near real-time data for the current
         * environmental conditions.
         *
         * @return A float representing the absolute pressure in Pascals (Pa).
         */
        virtual float GetPressure() = 0;
    };
}