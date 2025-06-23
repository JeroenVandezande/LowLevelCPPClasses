#include "MLX90614.h"

#include <cmath>

namespace LowLevelEmbedded {

MLX90614::MLX90614(II2CAccess* i2c, uint8_t address)
    : _i2c(i2c), _address(address) {}

bool MLX90614::ReadWord(uint8_t reg, uint16_t& value) {
    uint8_t buffer[3] = {0};
    if (!_i2c->I2C_WriteMethod(_address, &reg, 1)) return false;
    if (!_i2c->I2C_ReadMethod(_address, buffer, 3)) return false;

    value = static_cast<uint16_t>(buffer[1] << 8 | buffer[0]);  // LSB first
    return true;
}

float MLX90614::ConvertToCelsius(uint16_t raw) {
    return (static_cast<float>(raw) * 0.02f) - 273.15f;
}

bool MLX90614::ReadTemperature(RamRegister reg, float& temperatureC) {
    uint16_t raw;
    if (!ReadWord(static_cast<uint8_t>(reg), raw)) return false;
    temperatureC = ConvertToCelsius(raw);
    return true;
}

bool MLX90614::ReadAmbient(float& temperatureC) {
    return ReadTemperature(RamRegister::Ta, temperatureC);
}

bool MLX90614::ReadObject(float& temperatureC) {
    return ReadTemperature(RamRegister::Tobj1, temperatureC);
}

bool MLX90614::SetEmissivity(float emissivity) {
  if (emissivity < 0.1f || emissivity > 1.0f) return false;
  // Calculate register value
  uint16_t regVal = static_cast<uint16_t>(std::round(65535.0f * emissivity));
  // Erase (write 0) first
  if (!WriteWord(0x04, 0x0000)) return false;
  // Then write the new emissivity
  return WriteWord(0x04, regVal);
}

bool MLX90614::GetEmissivity(float &emissivity) {
  uint16_t regVal;
  if (!ReadWord(0x04, regVal)) return false;
  emissivity = static_cast<float>(regVal) / 65535.0f;
  return true;
}

/**
 * WAIT 5-10ms AFTER!!
 * Writes a 16-bit word to a specified register of the MLX90614 sensor using SMBus protocol.
 *
 * @param reg The register address to write the value to.
 * @param value The 16-bit value to be written to the register.
 * @return Returns true if the write operation is successful, false otherwise.
 */
bool MLX90614::WriteWord(uint8_t reg, uint16_t value) {
  // SMBus Write Word: [slave+W] reg LSB MSB PEC [stop]
  uint8_t buf[3];
  buf[0] = static_cast<uint8_t>(value & 0xFF);         // LSB
  buf[1] = static_cast<uint8_t>((value >> 8) & 0xFF);  // MSB
  // TODO: compute and append CRC-8 (PEC) if desired
  if (!_i2c->I2C_WriteMethod(_address, &reg, 1)) return false;
  if (!_i2c->I2C_WriteMethod(_address, buf, sizeof(buf))) return false;
  // Wait for EEPROM write to complete (device needs ~5ms)
  _i2c->I2C_IsDeviceReady(_address);
  return true;
}

} // namespace LowLevelEmbedded
