//
// Created by Jeroe on 2025-06-22.
//

#ifndef TCA9548A_H
#define TCA9548A_H
#include "I2CMultiplexer_base.h"

namespace LowLevelEmbedded::Devices::I2CMultiplexers
{

/**
 * @class TCA9548A
 *
 * @brief A class for interfacing with the TCA9548A I2C multiplexer.
 *
 * This class provides implementation for interacting with the TCA9548A I2C
 * multiplexer, enabling the switching of active channels and managing
 * communication with downstream I2C devices. It inherits common multiplexer
 * functionality from the I2CMultiplexer_base class and implements the specific
 * channel switching logic required for the TCA9548A device.
 */
class TCA9548A
    : public LowLevelEmbedded::I2CMultiplexer_base
{
public:
  TCA9548A(LowLevelEmbedded::II2CAccess *I2CAccess, uint8_t address)
      : I2CMultiplexer_base(I2CAccess)
  {
    _address = address;
  }

protected:
  /**
   * @brief Switches the active channel on the TCA9548A I2C multiplexer.
   *
   * This method enables the specified channel on the TCA9548A multiplexer by
   * sending the appropriate command to the device. It shifts a bit
   * corresponding to the provided channel and writes the resulting value to the
   * multiplexer via the I2C bus.
   *
   * @param channel The channel number to switch to on the TCA9548A multiplexer.
   */
  void SwitchMultiplexerChannel(uint8_t channel) override;

private:
  uint8_t _address = 0;
};

} // namespace LowLevelEmbedded::Devices::I2CMultiplexers
#endif // TCA9548A_H
