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
        uint8_t _registerPointer = 0xFF;

        bool Write16bitWord(uint8_t reg, uint16_t value);
        bool Read16bitWord(uint8_t reg, uint16_t& value);
        bool Write24bitWord(uint8_t reg, uint32_t value);
        bool Read24bitWord(uint8_t reg, uint32_t& value);
        bool Write40bitWord(uint8_t reg, uint64_t value);
        bool Read40bitWord(uint8_t reg, uint64_t& value);
        void SetRegisterPointerOnRead(uint8_t reg);

        // INA228 Register Addresses
        enum Registers : uint8_t {
            CONFIG = 0x00,
            ADC_CONFIG = 0x01,
            SHUNT_CAL = 0x02,
            SHUNT_TEMPCO = 0x03,
            VSHUNT = 0x04,
            VBUS = 0x05,
            DIETEMP = 0x06,
            CURRENT = 0x07,
            POWER = 0x08,
            ENERGY = 0x09,
            CHARGE = 0x0A,
            DIAG_ALRT = 0x0B,
            SOVL = 0x0C,
            SUVL = 0x0D,
            BOVL = 0x0E,
            BUVL = 0x0F,
            TEMP_LIMIT = 0x10,
            PWR_LIMIT = 0x11,
            MANUFACTURER_ID = 0x3E,
            DEVICE_ID = 0x3F
        };


    public:
        /**
         * @brief Constructs an instance of the INA228 class.
         *
         * This constructor initializes the INA228 device by configuring the
         * I2C access, slave address, and sense resistor.
         *
         * @param i2cAccess Pointer to an object that provides I2C access.
         * @param slaveAddress The 7-bit I2C address of the INA228 device.
         * @param senseResistance The value of the sense resistor in ohms, used for calculations.
         */
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
        /**
         * @brief Reads the current flowing through the sense resistor.
         *
         * @return The current in amperes. If reading fails, returns 0.
         */
        float ReadCurrent();

        /**
         * @brief Reads the bus voltage.
         *
         * @return The bus voltage in volts. If reading fails, returns 0.
         */
        float ReadBusVoltage();

        /**
         * @brief Reads the shunt voltage.
         *
         * @return The shunt voltage in volts. If reading fails, returns 0.
         */
        float ReadShuntVoltage();

        /**
         * @brief Reads the power consumption.
         *
         * @return The power in watts. If reading fails, returns 0.
         */
        float ReadPower();

        /**
         * @brief Reads the die temperature of the INA228.
         *
         * @return The temperature in degrees Celsius. If reading fails, returns 0.
         */
        float ReadTemperature();

        /**
         * @brief Reads the accumulated energy.
         *
         * @return The energy in joules. If reading fails, returns 0.
         */
        float ReadEnergy();

        /**
         * @brief Reads the accumulated charge.
         *
         * @return The charge in coulombs. If reading fails, returns 0.
         */
        float ReadCharge();

        /**
         * @brief Calibrates the device for accurate current and power measurements.
         *
         * @param maxCurrentExpected The maximum expected current in amperes.
         * @return True if calibration is successful, false otherwise.
         */
        bool Calibrate(float maxCurrentExpected);

        /**
         * @brief Sets the bus voltage over-limit threshold.
         *
         * @param limitInVolts The voltage limit in volts.
         * @return True if the limit is successfully set, false otherwise.
         */
        bool SetBusVoltageOverLimit(const float limitInVolts);

        /**
         * @brief Sets the bus voltage under-limit threshold.
         *
         * @param limitInVolts The voltage limit in volts.
         * @return True if the limit is successfully set, false otherwise.
         */
        bool SetBusVoltageUnderLimit(const float limitInVolts);

        /**
         * @brief Sets the shunt voltage over-limit threshold.
         *
         * @param limitInVolts The voltage limit in volts.
         * @return True if the limit is successfully set, false otherwise.
         */
        bool SetShuntVoltageOverLimit(const float limitInVolts);

        /**
         * @brief Sets the shunt voltage under-limit threshold.
         *
         * @param limitInVolts The voltage limit in volts.
         * @return True if the limit is successfully set, false otherwise.
         */
        bool SetShuntVoltageUnderLimit(const float limitInVolts);

        /**
         * @brief Resets the energy and charge accumulators.
         *
         * @return True if reset is successful, false otherwise.
         */
        bool ResetAccumulators();

        /**
         * @brief Reads the alert status register.
         *
         * @return The raw alert status register value. 0 if reading fails.
         */
        uint16_t ReadAlertStatus();

        /**
         * @brief Checks if a device with the configured address is present on the I2C bus.
         *
         * @return True if device is present, false otherwise.
         */
        bool IsDevicePresent();
    };

}
