//
// Created by Jeroen Vandezande on 05/11/2025.
//

#pragma once
#include "LLE_I2C.h"
#include <cstdint>

namespace LowLevelEmbedded::Devices::USBDevices
{
    class TUSB546 : public LowLevelEmbedded::II2CDevice
    {
    public:
        enum Configuration
        {
            Disabled = 0,
            USB_Only = 1,
            DisplayPort_Only = 2,
            DisplayPort_USB = 3,
        };

        explicit TUSB546(LowLevelEmbedded::II2CAccess* i2c, const uint8_t i2cAddress)
        {
            i2c_        = i2c;
            i2cAddress_ = i2cAddress;
        }

        bool SetConfiguration(Configuration configuration) const;

        bool SetFlipped(bool flipped) const;

        bool SwapHDP_IN_Pins(bool flipped) const;

        bool IsDeviceReady() override;

    private:
        uint8_t i2cAddress_;
        LowLevelEmbedded::II2CAccess* i2c_;
        bool writeRegister(uint8_t address, uint8_t value) const;
        [[nodiscard]] uint8_t readRegister(uint8_t address) const;
        /**
         * Modifies specific bits of a register at the given address by first reading the
         * register value, applying a bitwise mask, and then writing the updated value back.
         *
         * @param address The I2C address of the register to read and modify.
         * @param mask The bitwise mask that specifies which bits to modify. Bits set to 1 in
         *             the mask are those that will be cleared before applying the new value.
         * @param value The new value to apply to the masked bits in the register.
         */
        bool rmwRegister(uint8_t address, uint8_t mask, uint8_t value) const;
    };
}