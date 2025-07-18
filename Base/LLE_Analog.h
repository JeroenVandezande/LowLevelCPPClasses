//
// Created by Jeroe on 2025-06-20.
//

#ifndef LLE_ANALOG_H
#define LLE_ANALOG_H
#include <cstdint>
namespace LowLevelEmbedded
{

class IDACChannel
{
public:
  virtual ~IDACChannel() {};

  virtual bool WriteDAC(uint16_t value) = 0;

  virtual bool WriteDACVoltage(float value) = 0;
};

class IDAC
{
public:
  virtual ~IDAC() {};

  virtual bool WriteDAC(uint8_t channel, uint16_t value) = 0;

  virtual bool WriteDACVoltage(uint8_t channel, float value) = 0;

  virtual IDACChannel* CreateChannelObject(uint8_t channel) = 0;
};

} // namespace LowLevelEmbedded
#endif // LLE_ANALOG_H
