//
// Created by Jeroen Vandezande on 05/11/2025.
//

#include "TUSB546.h"

namespace LowLevelEmbedded::Devices::USBDevices
{
    bool TUSB546::writeRegister(const uint8_t address, const uint8_t value) const
    {
        uint8_t data[2] = { address, value };
        return i2c_->I2C_WriteMethod(i2cAddress_, data, 2);
    }

    uint8_t TUSB546::readRegister(uint8_t address) const
    {
        uint8_t returndata;
        i2c_->I2C_WriteMethod(i2cAddress_, &address, 1);
        i2c_->I2C_ReadMethod(i2cAddress_, &returndata, 1);
        return returndata;
    }

    bool TUSB546::rmwRegister(uint8_t address, uint8_t mask, uint8_t value) const
    {
        uint8_t reg = readRegister(address);
        reg &= ~mask;
        reg |= value;
        return writeRegister(address, reg);
    }

    bool TUSB546::SetConfiguration(const Configuration configuration) const
    {
        return rmwRegister(0x0A, 0b00000011, static_cast<uint8_t>(configuration));
    }

    bool TUSB546::SetFlipped(const bool flipped) const
    {
        return rmwRegister(0x0A, 0b00000100, flipped ? 0b00000100 : 0);
    }

    bool TUSB546::SwapHDP_IN_Pins(bool swapped) const
    {
        return rmwRegister(0x0A, 0b00100000, swapped ? 0b00100000 : 0);
    }


    bool TUSB546::IsDeviceReady()
    {
        return i2c_->I2C_IsDeviceReady(i2cAddress_);
    }


}