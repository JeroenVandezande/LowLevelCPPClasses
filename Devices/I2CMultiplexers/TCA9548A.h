//
// Created by Jeroe on 2025-06-22.
//

#ifndef TCA9548A_H
#define TCA9548A_H
#include "I2CMultiplexer_base.h"


class TCA9548A: public LowLevelEmbedded::Devices::I2CMultiplexers::I2CMultiplexer_base
{
public:
    TCA9548A(LowLevelEmbedded::II2CAccess *I2CAccess, uint8_t address):I2CMultiplexer_base(I2CAccess)
    {
        _address = address;
    }
protected:
    void SwitchMultiplexerChannel(uint8_t channel) override;
private:
  uint8_t _address = 0;
};


#endif //TCA9548A_H