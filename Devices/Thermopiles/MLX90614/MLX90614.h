#ifndef MLX90614_H
#define MLX90614_H

#include "LLE_I2C.h"
#include <Ratio.hpp>
#include <Temperature.hpp>
#include <cstdint>

namespace LowLevelEmbedded {

  namespace Devices
  {
    namespace Thermopiles
    {
      class MLX90614 {
      public:
        static constexpr uint8_t DEFAULT_I2C_ADDRESS = 0xB4;

        enum class RamRegister : uint8_t {
          Ta     = 0x06,  // Ambient temperature
          Tobj1  = 0x07,  // Object temperature 1
          Tobj2  = 0x08   // Object temperature 2 (for dual zone sensors)
      };

        MLX90614(II2CAccess* i2c, uint8_t address = DEFAULT_I2C_ADDRESS);

        bool ReadTemperature(RamRegister reg, unitsnet_cpp::Temperature& temperature);
        bool ReadAmbient(unitsnet_cpp::Temperature& temperature);
        bool ReadObject(unitsnet_cpp::Temperature& temperature);

        /// Set and get emissivity (0.1 … 1.0)
        bool SetEmissivity(unitsnet_cpp::Ratio emissivity);
        bool GetEmissivity(unitsnet_cpp::Ratio& emissivity);

      private:
        II2CAccess* _i2c;
        uint8_t _address;

        bool WriteWord(uint8_t reg, uint16_t value);
        bool ReadWord(uint8_t reg, uint16_t& value);
        static unitsnet_cpp::Temperature ConvertToTemperature(uint16_t raw);
      };
    }
  }
} // namespace LowLevelEmbedded

#endif // MLX90614_H
