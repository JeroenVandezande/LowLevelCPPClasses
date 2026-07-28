//
// Created by Jeroen Vandezande on 05/11/2025.
//

#pragma once

#include <RelativeHumidity.hpp>

namespace LowLevelEmbedded
{
    /**
     * @class IHumiditySensor
     * @brief An interface for sensing humidity levels.
     *
     * This interface defines a contract for humidity sensors to implement. It provides
     * the ability to read the relative humidity as a percentage.
     */
    class IHumiditySensor
    {
    public:
        virtual ~IHumiditySensor() = default;
        /**
         * @brief Retrieves the current relative humidity measurement.
         *
         * This method returns the relative humidity as a percentage. Sensors implementing this
         * method should provide real-time or near real-time data for the current environmental
         * conditions.
         *
         * @return The relative humidity measurement.
         */
        virtual unitsnet_cpp::RelativeHumidity GetHumidity() = 0;
    };
}

