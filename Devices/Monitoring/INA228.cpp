//
// Created by Jeroe on 2025-09-19.
//

#include "INA228.h"

namespace LowLevelEmbedded::Devices::Monitoring
{
    constexpr uint8_t CONFIG_REG_ADDR = 0x00; // 16-bit
    constexpr uint8_t ADC_CONFIG_REG = 0x01; // 16-bit
    constexpr uint8_t SHUNT_CAL_REG = 0x02; // 16-bit
    constexpr uint8_t SHUNT_TEMPCO_REG = 0x03; // 16-bit
    constexpr uint8_t VSHUNT_REG = 0x04; // 24-bit (signed)
    constexpr uint8_t VBUS_REG = 0x05; // 24-bit
    constexpr uint8_t DIETEMP_REG = 0x06; // 16-bit
    constexpr uint8_t CURRENT_REG = 0x07; // 24-bit (signed)
    constexpr uint8_t POWER_REG = 0x08; // 24-bit
    constexpr uint8_t ENERGY_REG = 0x09; // 40-bit (5 bytes)
    constexpr uint8_t CHARGE_REG = 0x0A; // 40-bit (5 bytes)
    constexpr uint8_t DIAG_ALRT_REG = 0x0B; // 16-bit
    constexpr uint8_t SOVL_REG = 0x0C; // 16-bit
    constexpr uint8_t SUVL_REG = 0x0D; // 16-bit
    constexpr uint8_t BOVL_REG = 0x0E; // 16-bit
    constexpr uint8_t BUVL_REG = 0x0F; // 16-bit
    constexpr uint8_t TEMP_LIMIT_REG = 0x10; // 16-bit
    constexpr uint8_t PWR_LIMIT_REG = 0x11; // 16-bit
    constexpr uint8_t MANUFACTURER_ID_REG = 0x3E; // 16-bit
    constexpr uint8_t DEVICE_ID_REG = 0x3F; // 16-bit

    bool INA228::Init()
    {

    }

    bool INA228::SetPowerOverLimit(const float limitInWatts)
    {
        return false;
    }
}
