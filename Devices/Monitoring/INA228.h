#pragma once
#include <cstdint>
#include "LLE_I2C.h"


namespace LowLevelEmbedded::Devices::Monitoring
{
    /**
     * @class INA228
     * @brief Represents an abstraction for the INA228 power monitor device.
     *
     * This class provides functionality to interface with the INA228 power monitor
     * over a specified I2C connection. It includes capabilities to measure voltage,
     * current, and power over the connected channel using a defined sense resistor.
     */
    class INA228
    {
    private:
        II2CAccess* _I2CAccess;
        uint8_t _SlaveAddress;
        float _SenseResistance;

        bool Write16bitWord(uint8_t reg, uint16_t value);
        bool Read16bitWord(uint8_t reg, uint16_t& value);
        bool Write24bitWord(uint8_t reg, uint32_t value);
        bool Read24bitWord(uint8_t reg, uint32_t& value);
        bool Write40bitWord(uint8_t reg, uint64_t value);
        bool Read40bitWord(uint8_t reg, uint64_t& value);

    public:
        INA228(II2CAccess* i2cAccess, const uint8_t slaveAddress, const float senseResistance)
        {
            _I2CAccess = i2cAccess;
            _SlaveAddress = slaveAddress;
            _SenseResistance = senseResistance;
        }

        bool Init();

        /**
         * @brief Sets the power over-limit threshold for the INA228 device.
         *
         * Configures the power monitoring device to trigger an alert or take action
         * when the measured power exceeds the specified limit.
         *
         * @param limitInWatts The power limit in watts. If the measured power exceeds
         * this value, the configured response is triggered.
         * @return True if the limit is successfully set, false otherwise.
         */
        bool SetPowerOverLimit(const float limitInWatts);

        /**
         * @brief Sets the temperature over-limit threshold for the INA228 device.
         *
         * Configures the INA228 device to monitor temperature and perform the necessary
         * action when the temperature exceeds the specified limit.
         *
         * @param limitInCelsius The temperature limit in degrees Celsius. If the measured
         * temperature exceeds this value, the configured response is triggered.
         * @return True if the temperature limit is successfully set, false otherwise.
         */
        bool SetTemperatureOverLimit(const float limitInCelsius);
    };

}
