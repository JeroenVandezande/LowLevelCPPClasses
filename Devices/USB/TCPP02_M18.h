#pragma once

#include "LLE_I2C.h"
#include <stdint.h>

/**
 * ST TCPP02-M18 — USB Type‑C Source port protector/companion.
 * I2C addressing: 7‑bit base 0b011010x (0x34/0x35), left‑adjusted for II2CAccess (bit0 is R/W).
 */
namespace LowLevelEmbedded::Devices::USBDevices
{
    enum TCPP02M18_Registers_t
    {
        Control = 0x00, Status1 = 0x01, Status2 = 0x02
    };

    enum TCPP02M18_PowerMode_t
    {
        Hibernate = 0,
        // PM2:PM1 = 00
        LowPower = 1,
        // PM2:PM1 = 10
        Normal = 2 // PM2:PM1 = 01
    };

    enum TCPP02M18_VConnSel_t
    {
        None = 0,
        // V2:V1 = 00 (open/open)
        CC2 = 1,
        // V2:V1 = 10 (close CC2)
        CC1 = 2 // V2:V1 = 01 (close CC1)
        // 11 reserved
    };

    struct TCPP02M18_Status1Flags
    {
        uint8_t raw;
        bool FLG_OVP_CC; // CC OVP (short to VBUS)
        bool FLG_OCP_VBUS; // VBUS over‑current
        bool FLG_OCP_VCONN; // VCONN over‑current
        bool FLG_OTP; // Over‑temperature
    };

    struct TCPP02M18_Status2Flags
    {
        uint8_t raw;
        bool ACK_VCONN1;
        bool ACK_VCONN2;
        bool ACK_GDP; // Gate Driver Provider ACK
        bool ACK_VBUSD; // VBUS discharge ACK
        bool VBUS_OK; // VBUS in range (internal monitor)
        bool IS_TCPP02; // 1 => TCPP02, 0 => TCPP03
    };

    class TCPP02_M18
    {
    private:
        II2CAccess* _I2CAccess;
        uint8_t _I2CAddress; // left‑adjusted 7‑bit address, bit0 unused (R/W)

        // Control register bits (datasheet Fig./table mapping):
        static constexpr uint8_t CTRL_PM_MASK = (0b11u << 6); // PM2:PM1
        static constexpr uint8_t CTRL_GDP = (1u << 4); // Gate Driver Provider enable
        static constexpr uint8_t CTRL_VBUSD = (1u << 3); // VBUS discharge
        static constexpr uint8_t CTRL_VCONND = (1u << 2); // VCONN discharge
        static constexpr uint8_t CTRL_VSEL_MASK = (0b11u); // V2:V1

        static uint8_t encodePM(TCPP02M18_PowerMode_t pm);
        static uint8_t encodeVSel(TCPP02M18_VConnSel_t sel);

        bool writeReg(uint8_t value) const;
        bool readReg(uint8_t reg, uint8_t* value) const;

    public:
        /**
                 * Constructor
                 * @param i2cAccess   pointer to II2CAccess implementation
                 * @param i2cAddPinHigh   level of the I2C_ADD pin (false => 0x34, true => 0x35)
                 */
        TCPP02_M18(II2CAccess* i2cAccess, const bool i2cAddPinHigh)
        {
            _I2CAccess = i2cAccess;
            // 7‑bit address 0b011010x becomes left‑adjusted 8‑bit 0b011010x0
            constexpr uint8_t baseLeftAdjusted = 0b01101000; // 0x34 << 1 = 0x68
            _I2CAddress = i2cAddPinHigh ? static_cast<uint8_t>(baseLeftAdjusted | 0b00000010) : baseLeftAdjusted;
            // 0x6A or 0x68
        }

        // --- High‑level controls ---
        bool SetPowerMode(TCPP02M18_PowerMode_t pm) const;
        bool EnableGateDriver(bool enable) const;
        bool SetVBUSDischarge(bool enable) const;
        bool SetVCONNDischarge(bool enable) const;
        bool SelectVCONN(TCPP02M18_VConnSel_t sel) const;
        bool RecoverFromFault(TCPP02M18_PowerMode_t pm) const;

        // --- Status readback ---
        bool ReadStatus1(TCPP02M18_Status1Flags* out) const;
        bool ReadStatus2(TCPP02M18_Status2Flags* out) const;
    };
}
