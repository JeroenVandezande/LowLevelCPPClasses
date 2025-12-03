#pragma once
#include <cstdint>
#include "LLE_I2C.h"


namespace LowLevelEmbedded::Devices::Monitoring
{
    struct INA228_Config
    {
        // Configuration (Table 7-5 in Datasheet)
        float ExpectedCurrent;
        bool AdcRange;
        uint8_t ConversionDelay; // Steps of 2ms each (0-510ms)
        bool UseTemperatureCompensation;
        uint16_t TemperatureCompensationPPM;

        // ADC Configuration (Table 7-6 in Datasheet)
        uint8_t Mode = 0xF; // Stored in Lower Nibble (Table 7-6 in Datasheet)
        uint8_t VBUSConversionTime = 0x0; // 50-4120us (0h-7h) (Table 7-6 in Datasheet)
        uint8_t ShuntVoltageConversionTime = 0x0; // 50-4120us (0h-7h) (Table 7-6 in Datasheet)
        uint8_t TemperatureConversionTime = 0x0; // 50-4120us (0h-7h) (Table 7-6 in Datasheet)
        uint8_t AverageCount = 0x0; // 1-1024 (0h-7h) (Table 7-6 in Datasheet)

        // Diagnostic Alert Configuration (Table 7-16)
        bool LatchAlert = false;
        bool ConversionReadyAssert = false;
        bool SlowAlert = false;
        bool AlertPolarity = false;
    };

    struct INA228_DiagAlertStatus
    {
        bool EnergyRegisterOverflow;
        bool ChargeRegisterOverflow;
        bool MathErrorOverflow;
        bool TemperatureOverLimit;
        bool ShuntVoltageOverLimit;
        bool ShuntVoltageUnderLimit;
        bool BusVoltageOverLimit;
        bool BusVoltageUnderLimit;
        bool PowerOverLimit;
        bool MemoryChecksumError;
    };

    /**
     * @class INA228
     * @brief Represents an abstraction for the INA228 power monitor device.
     *
     * This class provides functionality to interface with the INA228 power monitor
     * over a specified I2C connection. It includes capabilities to measure voltage,
     * current, and power over the connected channel using a defined sense resistor.
     */
    class INA228 : II2CDevice
    {
    private:
        II2CAccess* _i2CAccess;
        uint8_t _slaveAddress;
        float _senseResistance;
        float _currentLSB;
        uint8_t _registerPointer = 0xFF;
        INA228_Config _config;

        bool write16bitWord(uint8_t reg, uint16_t value);
        bool read16bitWord(uint8_t reg, uint16_t& value);
        bool write24bitWord(uint8_t reg, uint32_t value);
        bool read24bitWord(uint8_t reg, uint32_t& value);
        bool write40bitWord(uint8_t reg, uint64_t value);
        bool read40bitWord(uint8_t reg, uint64_t& value);
        void setRegisterPointerOnRead(uint8_t reg);
        bool calibrate(float maxCurrentExpected);

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
            _i2CAccess = i2cAccess;
            _slaveAddress = slaveAddress;
            _senseResistance = senseResistance;
        }

        /**
         * @brief Calibrates the device for accurate current and power measurements.
         *
         * @param config Configuration according to the datasheet
         * @return True if calibration is successful, false otherwise.
         */
        bool Init(INA228_Config config);

        /**
         * @brief Sets the temperature coefficient for temperature compensation of the shunt
         *
         * @param coefficient The PPM/C.
         * @return True if the limit is successfully set, false otherwise.
         */
        bool SetShuntTemperatureCoefficient(uint16_t coefficient);

        /**
         * @brief Reads the shunt voltage.
         *
         * @return The shunt voltage in volts. If reading fails, returns 0.
         */
        float ReadShuntVoltage();

        /**
         * @brief Reads the bus voltage.
         *
         * @return The bus voltage in volts. If reading fails, returns 0.
         */
        float ReadBusVoltage();

        /**
         * @brief Reads the die temperature of the INA228.
         *
         * @return The temperature in degrees Celsius. If reading fails, returns 0.
         */
        float ReadTemperature();

        /**
         * @brief Reads the current flowing through the sense resistor.
         *
         * @return The current in amperes. If reading fails, returns 0.
         */
        float ReadCurrent();

        /**
         * @brief Reads the power consumption.
         *
         * @return The power in watts. If reading fails, returns 0.
         */
        float ReadPower();

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
         * @brief Sets the shunt voltage over-limit threshold.
         *
         * @param limitInVolts The voltage limit in volts.
         * @return True if the limit is successfully set, false otherwise.
         */
        bool SetShuntVoltageOverLimit(float limitInVolts);

        /**
         * @brief Sets the shunt voltage under-limit threshold.
         *
         * @param limitInVolts The voltage limit in volts.
         * @return True if the limit is successfully set, false otherwise.
         */
        bool SetShuntVoltageUnderLimit(float limitInVolts);

        /**
         * @brief Sets the bus voltage over-limit threshold.
         *
         * @param limitInVolts The voltage limit in volts.
         * @return True if the limit is successfully set, false otherwise.
         */
        bool SetBusVoltageOverLimit(float limitInVolts);

        /**
         * @brief Sets the bus voltage under-limit threshold.
         *
         * @param limitInVolts The voltage limit in volts.
         * @return True if the limit is successfully set, false otherwise.
         */
        bool SetBusVoltageUnderLimit(float limitInVolts);

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
        bool SetTemperatureOverLimit(float limitInCelsius);

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
        bool SetPowerOverLimit(float limitInWatts);

        /**
         * @brief Resets the energy and charge accumulators.
         *
         * @return True if reset is successful, false otherwise.
         */
        bool ResetAccumulators();

        /**
         * @brief Reads the alert status register.
         *
         * @param alert_status out parameter for the alert status register
         * @return True if reset is successful, false otherwise.
         */
        bool ReadAlertStatus(INA228_DiagAlertStatus& alert_status);

        /**
         * @brief Reset.
         *
         * @param resetConfiguration reset configuration values in the config register
         * @return 0 if writing fails.
         */
        bool Reset(bool resetConfiguration = true);

        /**
         * @brief Checks if a device with the configured address is present on the I2C bus.
         *
         * @return True if device is present, false otherwise.
         */
        bool IsDeviceReady() override;
    };
}
