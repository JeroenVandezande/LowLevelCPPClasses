#include "TCPP02_M18.h"

namespace LowLevelEmbedded::Devices::USBDevices
{
    static inline void decodeStatus1(const uint8_t v, TCPP02M18_Status1Flags* s)
    {
        s->raw = v;
        s->FLG_OVP_CC = (v & 0x80u) != 0;
        s->FLG_OCP_VCONN = (v & 0x40u) != 0;
        s->FLG_OCP_VBUS = (v & 0x20u) != 0;
        s->FLG_OTP = (v & 0x10u) != 0;
    }

    static inline void decodeStatus2(const uint8_t v, TCPP02M18_Status2Flags* s)
    {
        s->raw = v;
        s->ACK_VCONN1 = (v & 0x80u) != 0;
        s->ACK_VCONN2 = (v & 0x20u) != 0;
        s->ACK_GDP = (v & 0x10u) != 0;
        s->ACK_VBUSD = (v & 0x08u) != 0;
        s->VBUS_OK = (v & 0x02u) != 0;
        s->IS_TCPP02 = (v & 0x01u) != 0;
    }

    uint8_t TCPP02_M18::encodePM(const TCPP02M18_PowerMode_t pm)
    {
        switch (pm)
        {
        case Hibernate: return (uint8_t)(0b00u << 6);
        case LowPower: return (uint8_t)(0b10u << 6);
        case Normal: return (uint8_t)(0b01u << 6);
        default: return (uint8_t)(0b00u << 6);
        }
    }

    uint8_t TCPP02_M18::encodeVSel(const TCPP02M18_VConnSel_t sel)
    {
        switch (sel)
        {
        case None: return 0b00u;
        case CC2: return 0b10u; // V2:V1=10 => CC2 closed
        case CC1: return 0b01u; // V2:V1=01 => CC1 closed
        default: return 0b00u;
        }
    }

    bool TCPP02_M18::SetPowerMode(const TCPP02M18_PowerMode_t pm) const
    {
        uint8_t cur;
        if (!readReg(Control, &cur)) return false;
        cur &= static_cast<uint8_t>(~CTRL_PM_MASK);
        cur |= encodePM(pm);
        return writeReg(cur);
    }

    bool TCPP02_M18::EnableGateDriver(const bool enable) const
    {
        uint8_t cur;
        if (!readReg(Control, &cur)) return false;
        if (enable) cur |= CTRL_GDP;
        else cur &= (uint8_t)~CTRL_GDP;
        return writeReg(cur);
    }

    bool TCPP02_M18::SetVBUSDischarge(const bool enable) const
    {
        uint8_t cur;
        if (!readReg(Control, &cur)) return false;
        if (enable) cur |= CTRL_VBUSD;
        else cur &= static_cast<uint8_t>(~CTRL_VBUSD);
        return writeReg(cur);
    }

    bool TCPP02_M18::SetVCONNDischarge(const bool enable) const
    {
        uint8_t cur;
        if (!readReg(Control, &cur)) return false;
        if (enable) cur |= CTRL_VCONND;
        else cur &= static_cast<uint8_t>(~CTRL_VCONND);
        return writeReg(cur);
    }

    bool TCPP02_M18::SelectVCONN(const TCPP02M18_VConnSel_t sel) const
    {
        uint8_t cur;
        if (!readReg(Control, &cur)) return false;
        cur &= static_cast<uint8_t>(~CTRL_VSEL_MASK);
        cur |= encodeVSel(sel);
        return writeReg(cur);
    }

    bool TCPP02_M18::RecoverFromFault(const TCPP02M18_PowerMode_t pm) const
    {
        // Datasheet "recovery word": keep only PM2:PM1 and VBUSD=1, others 0.
        const auto word = static_cast<uint8_t>(encodePM(pm) | CTRL_VBUSD);
        return writeReg(word);
    }

    bool TCPP02_M18::ReadStatus1(TCPP02M18_Status1Flags* out) const
    {
        uint8_t v;
        if (!readReg(Status1, &v)) return false;
        decodeStatus1(v, out);
        return true;
    }

    bool TCPP02_M18::ReadStatus2(TCPP02M18_Status2Flags* out) const
    {
        uint8_t v;
        if (!readReg(Status2, &v)) return false;
        decodeStatus2(v, out);
        return true;
    }

    bool TCPP02_M18::writeReg(const uint8_t value) const
    {
        uint8_t buf[2] = {0, value}; //only register 0 is writable
        return _I2CAccess->I2C_WriteMethod(_I2CAddress, &buf[0], 2);
    }

    bool TCPP02_M18::readReg(uint8_t reg, uint8_t* value) const
    {
        if (!_I2CAccess->I2C_WriteMethod(_I2CAddress, &reg, 1)) return false;
        return _I2CAccess->I2C_ReadMethod(_I2CAddress, value, 1);
    }

    bool TCPP02_M18::IsDeviceReady()
    {
        return _I2CAccess->I2C_IsDeviceReady(_I2CAddress);   
    }

}
