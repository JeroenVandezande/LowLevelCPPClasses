//
// Created by Jeroe on 2025-09-19.
//

#include "INA228.h"

namespace LowLevelEmbedded::Devices::Monitoring
{
    // Private methods for register access
    bool INA228::Write16bitWord(uint8_t reg, uint16_t value)
    {
        uint8_t data[3] = { reg, static_cast<uint8_t>(value >> 8), static_cast<uint8_t>(value & 0xFF) };
        if (_I2CAccess->I2C_WriteMethod(_SlaveAddress, data, 3))
        {
            _registerPointer = reg;
            return true;
        }
        return false;
    }

    bool INA228::Read16bitWord(uint8_t reg, uint16_t& value)
    {
        SetRegisterPointerOnRead(reg);
        uint8_t data[2] = { 0 };
        if (!_I2CAccess->I2C_ReadMethod(_SlaveAddress, data, 2))
            return false;

        value = (static_cast<uint16_t>(data[0]) << 8) | data[1];
        return true;
    }

    bool INA228::Write24bitWord(uint8_t reg, uint32_t value)
    {
        uint8_t data[4] = {
            reg,
            static_cast<uint8_t>((value >> 16) & 0xFF),
            static_cast<uint8_t>((value >> 8) & 0xFF),
            static_cast<uint8_t>(value & 0xFF)
        };
        if (_I2CAccess->I2C_WriteMethod(_SlaveAddress, data, 4))
        {
            _registerPointer = reg;
            return true;
        }
        return false;
    }

    bool INA228::Read24bitWord(uint8_t reg, uint32_t& value)
    {
        SetRegisterPointerOnRead(reg);
        uint8_t data[3] = { 0 };
        if (!_I2CAccess->I2C_ReadMethod(_SlaveAddress, data, 3))
            return false;

        value = (static_cast<uint32_t>(data[0]) << 16) |
                (static_cast<uint32_t>(data[1]) << 8) |
                data[2];
        return true;
    }

    bool INA228::Write40bitWord(uint8_t reg, uint64_t value)
    {
        uint8_t data[6] = {
            reg,
            static_cast<uint8_t>((value >> 32) & 0xFF),
            static_cast<uint8_t>((value >> 24) & 0xFF),
            static_cast<uint8_t>((value >> 16) & 0xFF),
            static_cast<uint8_t>((value >> 8) & 0xFF),
            static_cast<uint8_t>(value & 0xFF)
        };
        if (_I2CAccess->I2C_WriteMethod(_SlaveAddress, data, 6))
        {
            _registerPointer = reg;
            return true;
        }
        return false;
    }

    bool INA228::Read40bitWord(uint8_t reg, uint64_t& value)
    {
        SetRegisterPointerOnRead(reg);
        uint8_t data[5] = { 0 };
        data[0] = reg;
        if (!_I2CAccess->I2C_ReadWriteMethod(_SlaveAddress, data, 5, 1))
            return false;

        value = (static_cast<uint64_t>(data[0]) << 32) |
                (static_cast<uint64_t>(data[1]) << 24) |
                (static_cast<uint64_t>(data[2]) << 16) |
                (static_cast<uint64_t>(data[3]) << 8) |
                data[4];
        return true;
    }

    void INA228::SetRegisterPointerOnRead(uint8_t reg)
    {
        // Registry pointer is the same? If not, we don't need to do anything
        if (reg != _registerPointer )
        {
            // We must change the registry pointer by addressing the device (See page 19 of datasheet)
            uint8_t data[1] = { reg };
            if (!_I2CAccess->I2C_WriteMethod(_SlaveAddress, data, 1))
            {
                return; // Failure, do not update the registry pointer cache
            }
            _registerPointer = reg;
        }
    }

    // Public methods
    bool INA228::Init()
    {
        // Check if device is present
        if (!IsDevicePresent())
            return false;

        // Reset device - set bit 15 in CONFIG register
        if (!Write16bitWord(Registers::CONFIG, 0x8000))
            return false;

        // Reset Registry Pointer
        _registerPointer = 0xFF;

        // Wait for reset to complete (typically takes less than 1ms)
        uint16_t configValue = 0;
        do {
            if (!Read16bitWord(Registers::CONFIG, configValue))
                return false;
        } while (configValue & 0x8000);

        // Set default configuration
        // ADC settings: 1024 samples averaged, 16-bit resolution
        if (!Write16bitWord(Registers::ADC_CONFIG, 0x0F07))
            return false;

        return Calibrate(10.0); // Default calibration for 10A max
    }

    bool INA228::SetPowerOverLimit(const float limitInWatts)
    {
        // Convert watts to the device's internal format
        // Power LSB = 3.2 µW/bit per datasheet
        constexpr float powerLSB = 3.2e-6f;
        uint32_t limitRegValue = static_cast<uint32_t>(limitInWatts / powerLSB);

        return Write24bitWord(Registers::PWR_LIMIT, limitRegValue);
    }

    bool INA228::SetTemperatureOverLimit(const float limitInCelsius)
    {
        // Convert Celsius to the device's internal format
        // Temperature LSB = 7.8125 m°C/bit per datasheet
        constexpr float tempLSB = 7.8125e-3f;
        int16_t limitRegValue = static_cast<int16_t>(limitInCelsius / tempLSB);

        return Write16bitWord(Registers::TEMP_LIMIT, static_cast<uint16_t>(limitRegValue));
    }

    float INA228::ReadCurrent()
    {
        int32_t currentRaw = 0;
        if (!Read24bitWord(Registers::CURRENT, reinterpret_cast<uint32_t&>(currentRaw)))
            return 0.0f;

        // Handle sign extension for 24-bit two's complement
        if (currentRaw & 0x800000)
            currentRaw |= 0xFF000000;

        // Current LSB = 196.3 nA/bit (with calibration) per datasheet
        constexpr float currentLSB = 196.3e-9f;
        return currentRaw * currentLSB;
    }

    float INA228::ReadBusVoltage()
    {
        uint32_t vbusRaw = 0;
        if (!Read24bitWord(Registers::VBUS, vbusRaw))
            return 0.0f;

        // Bus Voltage LSB = 195.3125 μV/bit per datasheet
        constexpr float vbusLSB = 195.3125e-6f;
        return vbusRaw * vbusLSB;
    }

    float INA228::ReadShuntVoltage()
    {
        int32_t vshuntRaw = 0;
        if (!Read24bitWord(Registers::VSHUNT, reinterpret_cast<uint32_t&>(vshuntRaw)))
            return 0.0f;

        // Handle sign extension for 24-bit two's complement
        if (vshuntRaw & 0x800000)
            vshuntRaw |= 0xFF000000;

        // Shunt Voltage LSB = 312.5 nV/bit per datasheet
        constexpr float vshuntLSB = 312.5e-9f;
        return vshuntRaw * vshuntLSB;
    }

    float INA228::ReadPower()
    {
        uint32_t powerRaw = 0;
        if (!Read24bitWord(Registers::POWER, powerRaw))
            return 0.0f;

        // Power LSB = 3.2 µW/bit per datasheet
        constexpr float powerLSB = 3.2e-6f;
        return powerRaw * powerLSB;
    }

    float INA228::ReadTemperature()
    {
        int16_t tempRaw = 0;
        if (!Read16bitWord(Registers::DIETEMP, reinterpret_cast<uint16_t&>(tempRaw)))
            return 0.0f;

        // Temperature LSB = 7.8125 m°C/bit per datasheet
        constexpr float tempLSB = 7.8125e-3f;
        return tempRaw * tempLSB;
    }

    float INA228::ReadEnergy()
    {
        uint64_t energyRaw = 0;
        if (!Read40bitWord(Registers::ENERGY, energyRaw))
            return 0.0f;

        // Energy is accumulated power readings
        // Energy LSB = 32 µJ/LSB per datasheet (assuming default settings)
        constexpr float energyLSB = 32.0e-6f;
        return energyRaw * energyLSB;
    }

    float INA228::ReadCharge()
    {
        int64_t chargeRaw = 0;
        if (!Read40bitWord(Registers::CHARGE, reinterpret_cast<uint64_t&>(chargeRaw)))
            return 0.0f;

        // Handle sign extension for 40-bit two's complement
        if (chargeRaw & 0x8000000000)
            chargeRaw |= 0xFFFFFF0000000000;

        // Charge LSB = 1.9629 µC/LSB per datasheet (assuming default settings)
        constexpr float chargeLSB = 1.9629e-6f;
        return chargeRaw * chargeLSB;
    }

    bool INA228::Calibrate(float maxCurrentExpected)
    {
        // Calculate appropriate calibration value based on the sense resistor
        // and maximum expected current
        // SHUNT_CAL = 819.2 * 10^6 * currentLSB * _SenseResistance

        // Choose currentLSB = maxCurrentExpected / 2^19
        float currentLSB = maxCurrentExpected / 524288.0f;

        // Calculate SHUNT_CAL value
        float shuntCal = 819200000.0f * currentLSB * _SenseResistance;
        uint16_t calValue = static_cast<uint16_t>(shuntCal);

        return Write16bitWord(Registers::SHUNT_CAL, calValue);
    }

    bool INA228::SetBusVoltageOverLimit(const float limitInVolts)
    {
        // Bus Voltage LSB = 195.3125 μV/bit per datasheet
        constexpr float vbusLSB = 195.3125e-6f;
        uint16_t limitValue = static_cast<uint16_t>(limitInVolts / vbusLSB);

        return Write16bitWord(Registers::BOVL, limitValue);
    }

    bool INA228::SetBusVoltageUnderLimit(const float limitInVolts)
    {
        // Bus Voltage LSB = 195.3125 μV/bit per datasheet
        constexpr float vbusLSB = 195.3125e-6f;
        uint16_t limitValue = static_cast<uint16_t>(limitInVolts / vbusLSB);

        return Write16bitWord(Registers::BUVL, limitValue);
    }

    bool INA228::SetShuntVoltageOverLimit(const float limitInVolts)
    {
        // Shunt Voltage LSB = 312.5 nV/bit per datasheet
        constexpr float vshuntLSB = 312.5e-9f;
        uint16_t limitValue = static_cast<uint16_t>(limitInVolts / vshuntLSB);

        return Write16bitWord(Registers::SOVL, limitValue);
    }

    bool INA228::SetShuntVoltageUnderLimit(const float limitInVolts)
    {
        // Shunt Voltage LSB = 312.5 nV/bit per datasheet
        constexpr float vshuntLSB = 312.5e-9f;
        uint16_t limitValue = static_cast<uint16_t>(limitInVolts / vshuntLSB);

        return Write16bitWord(Registers::SUVL, limitValue);
    }

    bool INA228::ResetAccumulators()
    {
        // Read current CONFIG register
        uint16_t config;
        if (!Read16bitWord(Registers::CONFIG, config))
            return false;

        // Set bit 14 (RST_ACC) in CONFIG register
        config |= 0x4000;

        return Write16bitWord(Registers::CONFIG, config);
    }

    uint16_t INA228::ReadAlertStatus()
    {
        uint16_t alertStatus;
        if (!Read16bitWord(Registers::DIAG_ALRT, alertStatus))
            return 0;

        return alertStatus;
    }

    bool INA228::IsDevicePresent()
    {
        uint16_t manufacturerId = 0;
        if (!Read16bitWord(Registers::MANUFACTURER_ID, manufacturerId))
            return false;

        uint16_t deviceId = 0;
        if (!Read16bitWord(Registers::DEVICE_ID, deviceId))
            return false;

        // Expected values from datasheet:
        // Manufacturer ID: 0x5449 (TI in ASCII)
        // Device ID: 0x2281 for INA228
        return (manufacturerId == 0x5449) && ((deviceId & 0xFFF0) == 0x2280);
    }

}
