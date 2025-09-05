#pragma once

#include "../../Base/LLE_I2C.h"
#include <stdint.h>

namespace LowLevelEmbedded::Devices::USBDevices
{

    class TUSB544 : public II2CDevice
    {
    public:
        TUSB544(II2CAccess* i2c, uint8_t address)
            : _i2c(i2c), _addrLeft(address)
        {
        }

        // 1) Flip / No-Flip
        bool SetFlip(bool flipped)
        {
            return UpdateBits(GENERAL_4, MASK_FLIP, flipped ? MASK_FLIP : 0u);
        }

        // 2) Set mode to 4-lane DisplayPort
        bool SetModeDP4()
        {
            return UpdateBits(GENERAL_4, MASK_MODE, VAL_MODE_DP4);
        }

        bool SetModeOFF()
        {
            return UpdateBits(GENERAL_4, MASK_MODE, VAL_MODE_OFF);
        }

        bool OverrideAuxRoutingA()
        {
            return UpdateBits(DISPLAYPORT_4, 0b00110000, 0b00010000);
        }

        bool OverrideAuxRoutingB()
        {
            return UpdateBits(DISPLAYPORT_4, 0b00110000, 0b00100000);
        }

        bool DisableSnooping()
        {
            return UpdateBits(DISPLAYPORT_4, MASK_SNOOPING, 0b10000000);
        }

        bool DisableHPDPin()
        {
            return UpdateBits(GENERAL_4, 0b00001000, 0b00001000);
        }

        bool ResetAllRegisters()
        {
            return write1_(0x0B, 0b11000000);
        }

        // 3) Select Source (true) or Sink (false)
        bool SetRole(const bool source)
        {
            const uint8_t value = source ? 0x00 : 0x01;
            return UpdateBits(GENERAL_6, MASK_DIRSEL, value);
        }

        // Optional quick raw access
        bool ReadReg(uint8_t reg, uint8_t* val)
        {
            return read1_(reg, val);
        }

        bool WriteReg(uint8_t reg, uint8_t val)
        {
            return write1_(reg, val);
        }

        bool IsDeviceReady() override
        {
            return _i2c->I2C_IsDeviceReady(_addrLeft);
        }

    private:
        uint8_t GENERAL_4     = 0x0A;
        uint8_t GENERAL_6     = 0x0C;
        uint8_t DISPLAYPORT_4 = 0x13;

        uint8_t MASK_FLIP    = 0b00000100;
        uint8_t MASK_MODE    = 0b00000011;
        uint8_t MASK_SNOOPING    = 0b10000000;
        uint8_t VAL_MODE_DP4 = 0b10; // value inside MASK_MODE for "DP 4 lanes"
        uint8_t VAL_MODE_OFF = 0b00; // value inside MASK_MODE for "All Lanes Off"

        static constexpr uint8_t MASK_DIRSEL = 0x03; // GENERAL_6 bits [1:0] = DIR_SEL[1:0]

        II2CAccess* _i2c;
        uint8_t _addrLeft;

        // --- tiny I2C/RMW helpers ---
        bool send_(uint8_t reg)
        {
            return _i2c->I2C_WriteMethod(_addrLeft, &reg, 1);
        }

        bool read1_(const uint8_t reg, uint8_t* v)
        {
            if (!send_(reg))
            {
                return false;
            }
            return _i2c->I2C_ReadMethod(_addrLeft, v, 1);
        }

        bool write1_(const uint8_t reg, const uint8_t v)
        {
            uint8_t b[2] = { reg, v };
            return _i2c->I2C_WriteMethod(_addrLeft, b, 2);
        }

        bool UpdateBits(const uint8_t reg, const uint8_t mask, const uint8_t valueWithinMask)
        {
            uint8_t v;
            if (!read1_(reg, &v))
            {
                return false;
            }
            v = static_cast<uint8_t>((v & ~mask) | (valueWithinMask & mask));
            return write1_(reg, v);
        }
    };

}
