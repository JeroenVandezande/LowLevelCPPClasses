#pragma once

#include "LLE_I2C.h"
#include <stdint.h>

namespace LowLevelEmbedded::Devices::Power
{
    // PMBus / MPQ4262 commands (subset)
    enum MPQ4262_Command : uint8_t
    {
        MPQ4262_CMD_OPERATION = 0x01,
        MPQ4262_CMD_CLEAR_FAULTS = 0x03,
        MPQ4262_CMD_VOUT_COMMAND = 0x21,
        // Linear16, N = -10
        MPQ4262_CMD_STATUS_WORD = 0x79,
        MPQ4262_CMD_STATUS_TEMPERATURE = 0x7D,
        MPQ4262_CMD_MFR_CTRL1 = 0xD0,
        MPQ4262_CMD_MFR_CURRENT_LIMIT = 0xD1,
        MPQ4262_CMD_MFR_CTRL2 = 0xD2,
        MPQ4262_CMD_MFR_CTRL3 = 0xD3,
        MPQ4262_CMD_MFR_CTRL4 = 0xD4,
        MPQ4262_CMD_MFR_STATUS_MASK = 0xD8,
        MPQ4262_CMD_MFR_OTP_CONFIG = 0xD9,
        MPQ4262_CMD_MFR_OTP_REV = 0xDA
    };

    class MPQ4262: public II2CDevice
    {
    private:
        II2CAccess* _I2C ;
        const uint8_t _addrLeft = 0xCE;

        bool writeByte(uint8_t cmd, uint8_t data);
        bool writeWord(uint8_t cmd, uint16_t dataLE); // little-endian on wire
        bool sendCommand(uint8_t cmd); // write-only command
        bool readByte(uint8_t cmd, uint8_t* data);
        bool readWord(uint8_t cmd, uint16_t* dataLE);

        static uint16_t volts_to_L16(float volts); // N = -10
        static float L16_to_volts(uint16_t w);

    public:
        MPQ4262(II2CAccess* i2c)
        {
            _I2C = i2c;
        }

        bool IsDeviceReady() override;

        // Core PMBus controls
        bool SetOperation(bool on); // OPERATION (0x01)
        bool ClearFaults(); // CLEAR_FAULTS (0x03)
        bool SetVout_V(float volts); // VOUT_COMMAND (0x21) Linear16
        bool ReadStatusWord(uint16_t* out); // STATUS_WORD (0x79)
        bool ReadStatusTemperature(uint8_t* out); // STATUS_TEMPERATURE (0x7D)

        // Manufacturer controls
        bool SetDither(bool enable); // D0[7]
        enum class Freq : uint8_t
        {
            F280k = 0, F420k = 1, F580k = 2
        };

        bool SetSwitchFrequency(Freq f); // D0[6:5]
        bool SetForcedPWM(bool forcedPWM); // D0[0]
        bool SetOutputOVPEnable(bool en); // D0[2]
        bool SetOutputDischargeEnable(bool en); // D0[1]
        enum class SWARon : uint8_t
        {
            R5m = 0,
            R10m = 1,
            R15m = 2,
            R20m = 3
        };

        bool SetSWA_Ron(SWARon r); // D0[4:3]

        bool SetCurrentLimit_A(float amps); // D1, 50mA step (clamp ~5.4A)
        enum class LineDrop : uint8_t
        {
            OFF = 0,
            MV100_AT3A = 1,
            MV300_AT3A = 2,
            MV600_AT3A = 3
        };

        bool SetLineDropComp(LineDrop v); // D2[1:0]

        enum class SwCurrLim : uint8_t
        {
            LIM_6A_8A = 0,
            LIM_9A_12A = 1,
            LIM_13A_15A = 2,
            LIM_17A_20A = 3
        };

        bool SetSwitchingCurrentLimit(SwCurrLim lim); // D3[7:6]
        enum class RSense : uint8_t
        {
            RS_5m = 0, RS_10m = 1
        };

        bool SetRSense(RSense rs); // D3[5]
        enum class SlewRise : uint8_t
        {
            VREF_0p08mV_us = 0,
            VREF_0p16mV_us = 1,
            VREF_0p4mV_us = 2,
            VREF_0p8mV_us = 3
        };

        bool SetSlewRise(SlewRise sr); // D3[4:3]
        enum class SlewFall : uint8_t
        {
            VREF_0p02mV_us = 0,
            VREF_0p04mV_us = 1,
            VREF_0p1mV_us = 2,
            VREF_0p2mV_us = 3
        };

        bool SetSlewFall(SlewFall sf); // D3[2:1]
        bool SetBuckBoostFreqMode(bool maintainSame); // D3[0]

        bool SetCCBlankTimerFast(bool fast400us); // D4[6]
        bool SetSW2EdgeFast(bool fast); // D4[5]

        // Raw access (advanced)
        bool ReadReg(uint8_t reg, uint8_t* val);
        bool WriteReg(uint8_t reg, uint8_t val);
    };
}

