//
// Created by DanaNatov on 2026-06-30.
//

#pragma once

#include "LLE_I2C.h"
#include "LLE_Temp.h"
#include "LLE_Pressure.h"

#include <cstdint>

namespace LowLevelEmbedded::Devices::Sensors
{
    /// Oversampling ratio. The conversion takes longer (and is less noisy) the
    /// higher the ratio. The on-chip minimum conversion time is roughly
    /// (2^OS * 4 ms) + 2 ms, ranging from ~6 ms (OS1) to ~512 ms (OS128).
    typedef enum {
        MPL3115A2_OS1   = 0, // ~6 ms
        MPL3115A2_OS2   = 1, // ~10 ms
        MPL3115A2_OS4   = 2, // ~18 ms
        MPL3115A2_OS8   = 3, // ~34 ms
        MPL3115A2_OS16  = 4, // ~66 ms
        MPL3115A2_OS32  = 5, // ~130 ms
        MPL3115A2_OS64  = 6, // ~258 ms
        MPL3115A2_OS128 = 7, // ~512 ms
    } MPL3115A2_Oversample;

    /// NXP MPL3115A2 I2C precision barometric pressure / altitude + temperature sensor.
    ///
    /// The device exposes a 20-bit pressure (or altitude) channel and a 12-bit
    /// temperature channel. This driver uses one-shot (OST triggered) acquisitions
    /// so the bus stays idle between measurements.
    class MPL3115A2 final : public ITemperatureSensor, public IPressureSensor
    {
    private:
        II2CAccess* _i2cAccess;
        uint8_t _address;
        MPL3115A2_Oversample _oversample;

        bool ReadRegister(uint8_t reg, uint8_t* data, size_t length);
        bool WriteRegister(uint8_t reg, uint8_t value);
        /// Trigger a single one-shot measurement, wait for data-ready, then burst-read
        /// the 5 OUT_P/OUT_T bytes (OUT_P_MSB..OUT_T_LSB) into outData[0..4].
        /// @param altimeter true selects altimeter mode (ALT bit), false barometer mode.
        bool MeasureOnce(bool altimeter, uint8_t* outData);
        static uint16_t MaxConversionTimeMs(MPL3115A2_Oversample os);

    public:
        /// 7-bit address 0x60, stored left-adjusted (0x60 << 1) per II2CAccess convention.
        static constexpr uint8_t DEFAULT_I2C_ADDRESS = 0xC0;
        /// Value returned by the WHO_AM_I register for a genuine MPL3115A2.
        static constexpr uint8_t DEVICE_ID = 0xC4;

        MPL3115A2(II2CAccess* i2c, uint8_t address = DEFAULT_I2C_ADDRESS,
                  MPL3115A2_Oversample oversample = MPL3115A2_OS16);

        /// Verify the device (WHO_AM_I) and enable the data-ready event flags.
        /// @return true if an MPL3115A2 was found and configured.
        bool Initialize();
        /// Read the WHO_AM_I register (expected: DEVICE_ID, 0xC4).
        uint8_t WhoAmI();
        /// Issue a software reset. The device does not ACK the reset write, so a
        /// failed I2C transaction here is expected and ignored.
        void Reset();
        /// Change the oversampling ratio used by subsequent measurements.
        void SetOversample(MPL3115A2_Oversample oversample) { _oversample = oversample; }

        /// Take a one-shot measurement and return absolute pressure in Pascals (Pa).
        bool ReadPressure(float& pressurePa);
        /// Take a one-shot measurement and return altitude in meters (m), referenced
        /// to the configured sea-level pressure (default 101326 Pa).
        bool ReadAltitude(float& altitudeM);
        /// Take a one-shot measurement and return temperature in degrees Celsius.
        bool ReadTemperature(float& temperatureC);

        /// ITemperatureSensor: convenience wrapper, returns 0.0f on failure.
        float GetTemperature() override;
        /// IPressureSensor: convenience wrapper, returns 0.0f on failure.
        float GetPressure() override;
    };
}