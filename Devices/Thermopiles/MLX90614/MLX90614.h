#ifndef MLX90614_H
#define MLX90614_H

#include <cstdint>
#include "LLE_I2C.h"

namespace LowLevelEmbedded {

class MLX90614 {
public:
    static constexpr uint8_t DEFAULT_I2C_ADDRESS = 0x5A;

    enum class RamRegister : uint8_t {
        Ta     = 0x06,  // Ambient temperature
        Tobj1  = 0x07,  // Object temperature 1
        Tobj2  = 0x08   // Object temperature 2 (for dual zone sensors)
    };

    MLX90614(II2CAccess* i2c, uint8_t address = DEFAULT_I2C_ADDRESS);

    bool ReadTemperature(RamRegister reg, float& temperatureC);
    bool ReadAmbient(float& temperatureC);
    bool ReadObject(float& temperatureC);

    /// Set and get emissivity (0.1 … 1.0)
    bool SetEmissivity(float emissivity);
    bool GetEmissivity(float &emissivity);

private:
    II2CAccess* _i2c;
    uint8_t _address;

    bool WriteWord(uint8_t reg, uint16_t value);
    bool ReadWord(uint8_t reg, uint16_t& value);
    static float ConvertToCelsius(uint16_t raw);
};

} // namespace LowLevelEmbedded

#endif // MLX90614_H
