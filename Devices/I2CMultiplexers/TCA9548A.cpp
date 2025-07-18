//
// Created by Jeroe on 2025-06-22.
//

#include "TCA9548A.h"
#include <ulog.h>

namespace LowLevelEmbedded::Devices::I2CMultiplexers
{
void TCA9548A::SwitchMultiplexerChannel(uint8_t channel)
{
  uint8_t data = 1 << channel;
  _I2CAccess->I2C_WriteMethod(_address, &data, 1);
  log_trace("TCA9548A: Switching to channel %d", channel);
}
}