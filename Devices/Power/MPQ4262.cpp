#include "MPQ4262.h"

namespace LowLevelEmbedded::Devices::Power
{
    uint16_t MPQ4262::volts_to_L16(float volts)
    {
        if (volts < 0) volts = 0;
        float val = volts * 1024.0f; // N = -10
        if (val > 65535.0f) val = 65535.0f;
        return (uint16_t)(val + 0.5f);
    }

    float MPQ4262::L16_to_volts(uint16_t w)
    {
        return (float)w / 1024.0f;
    }

    bool MPQ4262::sendCommand(uint8_t cmd)
    {
        uint8_t b[1] = {cmd};
        return _I2C->I2C_WriteMethod(_addrLeft, b, 1);
    }

    bool MPQ4262::writeByte(uint8_t cmd, uint8_t data)
    {
        uint8_t b[2] = {cmd, data};
        return _I2C->I2C_WriteMethod(_addrLeft, b, 2);
    }

    bool MPQ4262::writeWord(uint8_t cmd, uint16_t dataLE)
    {
        uint8_t b[3] = {cmd, (uint8_t)(dataLE & 0xFFu), (uint8_t)(dataLE >> 8)};
        return _I2C->I2C_WriteMethod(_addrLeft, b, 3);
    }

    bool MPQ4262::readByte(uint8_t cmd, uint8_t* data)
    {
        if (!sendCommand(cmd)) return false;
        return _I2C->I2C_ReadMethod(_addrLeft, data, 1);
    }

    bool MPQ4262::readWord(uint8_t cmd, uint16_t* dataLE)
    {
        if (!sendCommand(cmd)) return false;
        uint8_t b[2];
        if (!_I2C->I2C_ReadMethod(_addrLeft, b, 2)) return false;
        *dataLE = (uint16_t)b[0] | ((uint16_t)b[1] << 8);
        return true;
    }

    // --- Public API ---
    bool MPQ4262::SetOperation(bool on) { return writeByte(0x01, on ? 0x01 : 0x00); }
    bool MPQ4262::ClearFaults() { return sendCommand(0x03); }

    bool MPQ4262::SetVout_V(float volts)
    {
        if (volts < 1.0f) volts = 1.0f;
        if (volts > 21.47f) volts = 21.47f; // device max range guard
        const uint16_t l16 = volts_to_L16(volts);
        return writeWord(0x21, l16);
    }

    bool MPQ4262::ReadStatusWord(uint16_t* out) { return readWord(0x79, out); }
    bool MPQ4262::ReadStatusTemperature(uint8_t* out) { return readByte(0x7D, out); }

    bool MPQ4262::SetDither(bool enable)
    {
        uint8_t v;
        if (!ReadReg(0xD0, &v)) return false;
        if (enable) v |= (1u << 7);
        else v &= ~(1u << 7);
        return WriteReg(0xD0, v);
    }

    bool MPQ4262::SetSwitchFrequency(Freq f)
    {
        uint8_t v;
        if (!ReadReg(0xD0, &v)) return false;
        v &= ~(0b11u << 5);
        v |= (uint8_t(f) & 0x3u) << 5;
        return WriteReg(0xD0, v);
    }

    bool MPQ4262::SetForcedPWM(bool forcedPWM)
    {
        uint8_t v;
        if (!ReadReg(0xD0, &v)) return false;
        if (forcedPWM) v |= (1u << 0);
        else v &= ~(1u << 0);
        return WriteReg(0xD0, v);
    }

    bool MPQ4262::SetOutputOVPEnable(bool en)
    {
        uint8_t v;
        if (!ReadReg(0xD0, &v)) return false;
        if (en) v |= (1u << 2);
        else v &= ~(1u << 2);
        return WriteReg(0xD0, v);
    }

    bool MPQ4262::SetOutputDischargeEnable(bool en)
    {
        uint8_t v;
        if (!ReadReg(0xD0, &v)) return false;
        if (en) v |= (1u << 1);
        else v &= ~(1u << 1);
        return WriteReg(0xD0, v);
    }

    bool MPQ4262::SetSWA_Ron(SWARon r)
    {
        uint8_t v;
        if (!ReadReg(0xD0, &v)) return false;
        v &= ~(0b11u << 3);
        v |= (uint8_t(r) & 0x3u) << 3;
        return WriteReg(0xD0, v);
    }

    bool MPQ4262::SetCurrentLimit_A(float amps)
    {
        if (amps < 0.0f) amps = 0.0f;
        if (amps > 5.4f) amps = 5.4f; // clamp to device limit
        uint8_t raw = (uint8_t)(amps / 0.05f + 0.5f); // 50 mA steps
        return writeByte(0xD1, raw & 0x7Fu);
    }

    bool MPQ4262::SetLineDropComp(LineDrop v)
    {
        uint8_t reg = (uint8_t)v & 0x3u; // only D2[1:0] used
        return WriteReg(0xD2, reg);
    }

    bool MPQ4262::SetSwitchingCurrentLimit(SwCurrLim lim)
    {
        uint8_t v;
        if (!ReadReg(0xD3, &v)) return false;
        v &= ~(0b11u << 6);
        v |= (uint8_t(lim) & 0x3u) << 6;
        return WriteReg(0xD3, v);
    }

    bool MPQ4262::SetRSense(RSense rs)
    {
        uint8_t v;
        if (!ReadReg(0xD3, &v)) return false;
        if (rs == RSense::RS_10m) v |= (1u << 5);
        else v &= ~(1u << 5);
        return WriteReg(0xD3, v);
    }

    bool MPQ4262::SetSlewRise(SlewRise sr)
    {
        uint8_t v;
        if (!ReadReg(0xD3, &v)) return false;
        v &= ~(0b11u << 4);
        v |= (uint8_t(sr) & 0x3u) << 4;
        return WriteReg(0xD3, v);
    }

    bool MPQ4262::SetSlewFall(SlewFall sf)
    {
        uint8_t v;
        if (!ReadReg(0xD3, &v)) return false;
        v &= ~(0b11u << 1);
        v |= (uint8_t(sf) & 0x3u) << 1;
        return WriteReg(0xD3, v);
    }

    bool MPQ4262::SetBuckBoostFreqMode(bool maintainSame)
    {
        uint8_t v;
        if (!ReadReg(0xD3, &v)) return false;
        if (maintainSame) v |= (1u << 0);
        else v &= ~(1u << 0);
        return WriteReg(0xD3, v);
    }

    bool MPQ4262::SetCCBlankTimerFast(bool fast400us)
    {
        uint8_t v;
        if (!ReadReg(0xD4, &v)) return false;
        if (fast400us) v |= (1u << 6);
        else v &= ~(1u << 6);
        return WriteReg(0xD4, v);
    }

    bool MPQ4262::SetSW2EdgeFast(bool fast)
    {
        uint8_t v;
        if (!ReadReg(0xD4, &v)) return false;
        if (fast) v |= (1u << 5);
        else v &= ~(1u << 5);
        return WriteReg(0xD4, v);
    }

    bool MPQ4262::ReadReg(uint8_t reg, uint8_t* val) { return readByte(reg, val); }
    bool MPQ4262::WriteReg(uint8_t reg, uint8_t val) { return writeByte(reg, val); }
}
