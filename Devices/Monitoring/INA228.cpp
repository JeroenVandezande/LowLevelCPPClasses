//
// Created by Jeroen on 2025-09-19.
//

#include "INA228.h"
#include <cmath>

namespace LowLevelEmbedded::Devices::Monitoring
{
    // Private methods for register access
    bool INA228::write16bitWord(uint8_t reg, uint16_t value)
    {
        uint8_t data[3] = { reg, static_cast<uint8_t>(value >> 8), static_cast<uint8_t>(value & 0xFF) };
        if (_i2CAccess->I2C_WriteMethod(_slaveAddress, data, 3))
        {
            _registerPointer = reg;
            return true;
        }
        return false;
    }

    bool INA228::read16bitWord(uint8_t reg, uint16_t& value)
    {
        setRegisterPointerOnRead(reg);
        uint8_t data[2] = { 0 };
        if (!_i2CAccess->I2C_ReadMethod(_slaveAddress, data, 2))
            return false;

        value = (static_cast<uint16_t>(data[0]) << 8) | data[1];
        return true;
    }

    bool INA228::write24bitWord(uint8_t reg, uint32_t value)
    {
        uint8_t data[4] = {
            reg,
            static_cast<uint8_t>((value >> 16) & 0xFF),
            static_cast<uint8_t>((value >> 8) & 0xFF),
            static_cast<uint8_t>(value & 0xFF)
        };
        if (_i2CAccess->I2C_WriteMethod(_slaveAddress, data, 4))
        {
            _registerPointer = reg;
            return true;
        }
        return false;
    }

    bool INA228::read24bitWord(uint8_t reg, uint32_t& value)
    {
        setRegisterPointerOnRead(reg);
        uint8_t data[3] = { 0 };
        if (!_i2CAccess->I2C_ReadMethod(_slaveAddress, data, 3))
            return false;

        value = (static_cast<uint32_t>(data[0]) << 16) |
                (static_cast<uint32_t>(data[1]) << 8) |
                data[2];
        return true;
    }

    bool INA228::write40bitWord(uint8_t reg, uint64_t value)
    {
        uint8_t data[6] = {
            reg,
            static_cast<uint8_t>((value >> 32) & 0xFF),
            static_cast<uint8_t>((value >> 24) & 0xFF),
            static_cast<uint8_t>((value >> 16) & 0xFF),
            static_cast<uint8_t>((value >> 8) & 0xFF),
            static_cast<uint8_t>(value & 0xFF)
        };
        if (_i2CAccess->I2C_WriteMethod(_slaveAddress, data, 6))
        {
            _registerPointer = reg;
            return true;
        }
        return false;
    }

    bool INA228::read40bitWord(uint8_t reg, uint64_t& value)
    {
        setRegisterPointerOnRead(reg);
        uint8_t data[5] = { 0 };
        data[0] = reg;
        if (!_i2CAccess->I2C_ReadWriteMethod(_slaveAddress, data, 5, 1))
            return false;

        value = (static_cast<uint64_t>(data[0]) << 32) |
                (static_cast<uint64_t>(data[1]) << 24) |
                (static_cast<uint64_t>(data[2]) << 16) |
                (static_cast<uint64_t>(data[3]) << 8) |
                data[4];
        return true;
    }

    void INA228::setRegisterPointerOnRead(uint8_t reg)
    {
        // Registry pointer is the same? If not, we don't need to do anything
        if (reg != _registerPointer )
        {
            // We must change the registry pointer by addressing the device (See page 19 of datasheet)
            uint8_t data[1] = { reg };
            if (!_i2CAccess->I2C_WriteMethod(_slaveAddress, data, 1))
            {
                return; // Failure, do not update the registry pointer cache
            }
            _registerPointer = reg;
        }
    }

    bool INA228::calibrate(float maxCurrentExpected)
    {
        // Calculate appropriate calibration value based on the sense resistor
        // and maximum expected current

        // Choose currentLSB = maxCurrentExpected / 2^19
        _currentLSB = maxCurrentExpected / 524288.0f;

        // Calculate SHUNT_CAL value
        float shuntCal = 13107200000.0f * _currentLSB * _senseResistance;
        shuntCal = shuntCal * (_config.AdcRange ? 4.0f : 1.0f);
        uint16_t calValue = static_cast<uint16_t>(shuntCal);
        if (calValue > 0x7FFF) // Max 32767 Shunt CAL Value
        {
            return false;
        }

        return write16bitWord(SHUNT_CAL, calValue);
    }

    // Public methods
    bool INA228::Init(INA228_Config config)
    {
        _config = config;

        // Check if device is present
        if (!IsDeviceReady())
            return false;

        // Reset device - set bit 15 in CONFIG register
        if (!write16bitWord(CONFIG, 0x8000))
            return false;

        // Reset Registry Pointer
        _registerPointer = 0xFF;

        // Wait for reset to complete (typically takes less than 300us)
        uint16_t configValue = 0;

        do {
            if (!read16bitWord(CONFIG, configValue))
                return false;
        } while (configValue & 0x8000);

        // Set Config register
        configValue |= (_config.AdcRange ? 0x0010 : 0x0000);
        configValue |= static_cast<uint16_t>(_config.ConversionDelay) << 6;
        configValue |= (_config.UseTemperatureCompensation ? 0x0020 : 0x0000);

        // Write new configuration
        if (!write16bitWord(CONFIG, configValue))
            return false;

        uint16_t adcConfig = 0;
        adcConfig |= (_config.Mode & 0x0F) << 12;
        adcConfig |= (_config.VBUSConversionTime & 0x07) << 9;
        adcConfig |= (_config.ShuntVoltageConversionTime & 0x07) << 6;
        adcConfig |= (_config.TemperatureConversionTime & 0x07) << 3;
        adcConfig |= (_config.AverageCount & 0x07);

        // Set default configuration
        // ADC settings: 1024 samples averaged, 16-bit resolution
        if (!write16bitWord(ADC_CONFIG, adcConfig))
            return false;

        if (_config.UseTemperatureCompensation)
        {
            if (!SetShuntTemperatureCoefficient(_config.UseTemperatureCompensation))
            {
                return false;
            }
        }

        uint16_t diagConfig = 0;
        diagConfig ^= (-static_cast<uint16_t>(_config.LatchAlert) ^ diagConfig) & (1u << 15);
        diagConfig ^= (-static_cast<uint16_t>(_config.ConversionReadyAssert) ^ diagConfig) & (1u << 14);
        diagConfig ^= (-static_cast<uint16_t>(_config.ConversionDelay) ^ diagConfig) & (1u << 13);
        diagConfig ^= (-static_cast<uint16_t>(_config.AlertPolarity) ^ diagConfig) & (1u << 12);
        if (!write16bitWord(CONFIG, diagConfig))
        {
            return false;
        }

        return calibrate(_config.ExpectedCurrent);
    }

    bool INA228::SetShuntTemperatureCoefficient(const uint16_t coefficient)
    {
        if (coefficient > 0x3FFF) // Max 16383 PPM/C
        {
            return false;
        }
        return write16bitWord(SHUNT_TEMPCO, coefficient);
    }

    float INA228::ReadShuntVoltage()
    {
        uint32_t reg24 = 0;
        if (!read24bitWord(VSHUNT, reg24))
            return 0.0f;

        // Sign-extend from 24-bit two's complement
        if (reg24 & 0x800000)          // bit 23 = sign
            reg24 |= 0xFF000000;       // extend to 32 bits

        // 4 reserved LSBs (bits 3..0)
        int32_t vshuntRaw = static_cast<int32_t>(reg24) >> 4;  // arithmetic shift

        // Shunt Voltage LSB = 312.5 nV or 78.125 nV per datasheet
        const float vshuntLSB = _config.AdcRange ? 78.125e-9f : 312.5e-9f;

        return static_cast<float>(vshuntRaw) * vshuntLSB;
    }

    float INA228::ReadBusVoltage()
    {
        uint32_t reg24 = 0;
        if (!read24bitWord(VBUS, reg24))
            return 0.0f;

        // Sign-extend from 24-bit two's complement
        if (reg24 & 0x800000)          // bit 23 = sign
            reg24 |= 0xFF000000;       // extend to 32 bits

        // 4 reserved LSBs (bits 3..0)
        int32_t vbusRaw = static_cast<int32_t>(reg24) >> 4;  // arithmetic shift

        // Bus Voltage LSB = 195.3125 μV/bit per datasheet
        constexpr float vbusLSB = 195.3125e-6f;
        return vbusRaw * vbusLSB;
    }

    float INA228::ReadTemperature()
    {
        int16_t tempRaw = 0;
        if (!read16bitWord(DIETEMP, reinterpret_cast<uint16_t&>(tempRaw)))
            return 0.0f;

        // Temperature LSB = 7.8125 m°C/bit per datasheet
        constexpr float tempLSB = 7.8125e-3f;
        return tempRaw * tempLSB;
    }

    float INA228::ReadCurrent()
    {
        uint32_t reg24 = 0;
        if (!read24bitWord(CURRENT, reg24))
            return 0.0f;

        // Sign-extend from 24-bit two's complement
        if (reg24 & 0x800000)          // bit 23 = sign
            reg24 |= 0xFF000000;       // extend to 32 bits

        // 4 reserved LSBs (bits 3..0)
        int32_t currentRaw = static_cast<int32_t>(reg24) >> 4;  // arithmetic shift

        return static_cast<float>(currentRaw) * _currentLSB;
    }

    float INA228::ReadPower()
    {
        uint32_t powerRaw = 0;
        if (!read24bitWord(POWER, powerRaw))
            return 0.0f;

        return powerRaw * _currentLSB * 3.2;
    }

    float INA228::ReadEnergy()
    {
        uint64_t energyRaw = 0;
        if (!read40bitWord(ENERGY, energyRaw))
            return 0.0f;

        return energyRaw * _currentLSB * 3.2 * 16;
    }

    float INA228::ReadCharge()
    {
        int64_t chargeRaw = 0;
        if (!read40bitWord(CHARGE, reinterpret_cast<uint64_t&>(chargeRaw)))
            return 0.0f;

        // Handle sign extension for 40-bit two's complement
        if (chargeRaw & 0x8000000000)
        {
            chargeRaw |= 0xFFFFFF0000000000;
        }

        return chargeRaw * _currentLSB;
    }

    bool INA228::SetShuntVoltageOverLimit(const float limitInVolts)
    {
        // LSB from datasheet
        const float vshuntLSB = _config.AdcRange ? 1.25e-6f : 5e-6f;

        // Convert to raw integer
        float rawF = limitInVolts / vshuntLSB;

        // Round to nearest integer
        int32_t raw = static_cast<int32_t>(std::round(rawF));

        // Clamp to signed 16-bit range
        if (raw > INT16_MAX) raw = INT16_MAX;
        if (raw < INT16_MIN) raw = INT16_MIN;

        // Convert to signed 16-bit two’s-complement
        int16_t raw16 = static_cast<int16_t>(raw);

        // Reinterpret as uint16_t for transmission
        uint16_t regValue = static_cast<uint16_t>(raw16);

        return write16bitWord(SOVL, regValue);
    }

    bool INA228::SetShuntVoltageUnderLimit(const float limitInVolts)
    {
        // LSB from datasheet
        const float vshuntLSB = _config.AdcRange ? 1.25e-6f : 5e-6f;

        // Convert to raw integer
        float rawF = limitInVolts / vshuntLSB;

        // Round to nearest integer
        int32_t raw = static_cast<int32_t>(std::round(rawF));

        // Clamp to signed 16-bit range
        if (raw > INT16_MAX) raw = INT16_MAX;
        if (raw < INT16_MIN) raw = INT16_MIN;

        // Convert to signed 16-bit two’s-complement
        int16_t raw16 = static_cast<int16_t>(raw);

        // Reinterpret as uint16_t for transmission
        uint16_t regValue = static_cast<uint16_t>(raw16);

        return write16bitWord(SUVL, regValue);
    }

    bool INA228::SetBusVoltageOverLimit(const float limitInVolts)
    {
        if (limitInVolts < 0.0f) return false; // Negative values are not allowed (see datasheet)

        // LSB from datasheet: 3.125 mV
        constexpr float vbusLSB = 3.125e-3f;

        // Convert to raw value
        float rawF = limitInVolts / vbusLSB;

        // Round to nearest integer
        uint32_t raw = static_cast<uint32_t>(std::round(rawF));

        // Limit raw value to 15-bit range (0..0x7FFF)
        if (raw > 0x7FFF)
            raw = 0x7FFF;

        // Bit 15 must be zero
        uint16_t regValue = static_cast<uint16_t>(raw & 0x7FFF);

        return write16bitWord(BOVL, regValue);
    }

    bool INA228::SetBusVoltageUnderLimit(const float limitInVolts)
    {
        if (limitInVolts < 0.0f) return false; // Negative values are not allowed (see datasheet)

        // LSB from datasheet: 3.125 mV
        constexpr float vbusLSB = 3.125e-3f;

        // Convert to raw value
        float rawF = limitInVolts / vbusLSB;

        // Round to nearest integer
        uint32_t raw = static_cast<uint32_t>(std::round(rawF));

        // Limit raw value to 15-bit range (0..0x7FFF)
        if (raw > 0x7FFF)
            raw = 0x7FFF;

        // Bit 15 must be zero
        uint16_t regValue = static_cast<uint16_t>(raw & 0x7FFF);

        return write16bitWord(BUVL, regValue);
    }

    bool INA228::SetTemperatureOverLimit(const float limitInCelsius)
    {
        // LSB from datasheet
        const float tempLSB = 7.8125e-3f;

        // Convert to raw integer
        float rawF = limitInCelsius / tempLSB;

        // Round to nearest integer
        int32_t raw = static_cast<int32_t>(std::round(rawF));

        // Clamp to signed 16-bit range
        if (raw > INT16_MAX) raw = INT16_MAX;
        if (raw < INT16_MIN) raw = INT16_MIN;

        // Convert to signed 16-bit two’s-complement
        int16_t raw16 = static_cast<int16_t>(raw);

        // Reinterpret as uint16_t for transmission
        uint16_t regValue = static_cast<uint16_t>(raw16);

        return write16bitWord(TEMP_LIMIT, regValue);
    }

    bool INA228::SetPowerOverLimit(const float limitInWatts)
    {
        if (limitInWatts < 0.0f)
        {
            return false; // Negative values are not allowed (see datasheet)
        }

        // LSB from datasheet
        float powerLSB = _currentLSB * 3.2f * 256;

        // Convert to raw value
        float rawF = limitInWatts / powerLSB;

        // Round to nearest integer
        uint16_t regValue = static_cast<uint16_t>(std::round(rawF));

        return write24bitWord(PWR_LIMIT, regValue);
    }

    bool INA228::ResetAccumulators()
    {
        // Read current CONFIG register
        uint16_t config;
        if (!read16bitWord(CONFIG, config))
        {
            return false;
        }

        // Set bit 14 (RST_ACC) in CONFIG register
        config |= 0x4000;

        return write16bitWord(CONFIG, config);
    }

    bool INA228::ReadAlertStatus(INA228_DiagAlertStatus& alert_status)
    {
        uint16_t alertStatus;
        if (!read16bitWord(DIAG_ALRT, alertStatus))
        {
            return false;
        }

        alert_status
        =
        {
            !!((alertStatus) & (1<<11)),
            !!((alertStatus) & (1<<10)),
            !!((alertStatus) & (1<<9)),
            !!((alertStatus) & (1<<7)),
            !!((alertStatus) & (1<<6)),
            !!((alertStatus) & (1<<5)),
            !!((alertStatus) & (1<<4)),
            !!((alertStatus) & (1<<3)),
            !!((alertStatus) & (1<<2)),
            !((alertStatus) & (1<<0))
        };

        return true;
    }

    bool INA228::Reset(bool resetConfiguration)
    {
        // Read current CONFIG register
        uint16_t config;
        if (!read16bitWord(CONFIG, config))
        {
            return false;
        }

        // Set bit 15 (RST) in CONFIG register
        uint16_t configReset = config | 0x8000;

        if (!write16bitWord(CONFIG, configReset))
        {
            return false;
        }

        uint16_t configValue = 0;
        do {
            if (!read16bitWord(CONFIG, configValue))
                return false;
        } while (configValue & 0x8000);

        // Reset Config
        if (resetConfiguration)
        {
            return true;
        }

        // Preserve old config
        return Init(_config);
    }


    bool INA228::IsDeviceReady()
    {
        uint16_t manufacturerId = 0;
        if (!read16bitWord(MANUFACTURER_ID, manufacturerId))
            return false;

        // Expected values from datasheet:
        // Manufacturer ID: 0x5449 (TI in ASCII)
        return (manufacturerId == 0x5449);
    }

}
