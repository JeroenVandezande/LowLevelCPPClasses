//
// Created by Jeroen Vandezande on 03/01/2023.
//

#include "DAC7578.h"


namespace LowLevelEmbedded::Devices::DACs
{
    const uint8_t CA_WRITEANDUPDATE = 0b00110000;

    DAC7578::DAC7578(
        II2CAccess* i2cAccess,
        uint8_t slaveAddres,
        unitsnet_cpp::ElectricPotential referenceVoltage)
        : _I2CAccess(i2cAccess),
          _SlaveAddress(slaveAddres),
          _ReferenceVoltage(referenceVoltage)
    {
    }

    bool DAC7578::InitDAC()
    {
        return true;
    }

    bool DAC7578::WriteDAC(const uint8_t channel, const uint16_t value)
    {
        uint8_t data[3];
        data[0] = CA_WRITEANDUPDATE | channel;
        data[1] = (value & 0x0FF0) >> 4;
        data[2] = (value & 0x000F) << 4;
        return _I2CAccess->I2C_WriteMethod(_SlaveAddress, &data[0], 3);
    }

    uint16_t DAC7578::GetMaxDAValue()
    {
        return 4095;
    }

    uint8_t DAC7578::GetMaxChannels()
    {
        return 8;
    }

    bool DAC7578::WriteDACVoltage(
        uint8_t channel,
        unitsnet_cpp::ElectricPotential value)
    {
        const auto ratio = value.volts() / _ReferenceVoltage.volts();
        if (ratio < 0.0f || ratio > 1.0f)
        {
            return false;
        }
        return WriteDAC(channel, static_cast<uint16_t>(ratio * GetMaxDAValue()));
    }

    IDACChannel<uint16_t>* DAC7578::CreateChannelObject(uint8_t channel)
    {
        return new DACChannel_base<uint16_t>(this, channel);
    }
}
