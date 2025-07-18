//
// Created by Jeroe on 2025-06-22.
//

#include "I2CMultiplexer_base.h"

namespace LowLevelEmbedded
{
    I2CMultiplexer_channel* I2CMultiplexer_base::CreateChannel(uint8_t channel)
    {
        return new I2CMultiplexer_channel(this, channel);
    }

    bool I2CMultiplexer_channel::I2C_WriteMethod(uint8_t address, uint8_t* data, size_t length)
    {
        if (_channelnumber != _parent->_multiplexerchannel)
        {
            _parent->SwitchMultiplexerChannel(_channelnumber);
            _parent->_multiplexerchannel = _channelnumber;
        }
        return _parent->_I2CAccess->I2C_WriteMethod(address, data, length);
    }

    bool I2CMultiplexer_channel::I2C_ReadMethod(uint8_t address, uint8_t* data, size_t length)
    {
        if (_channelnumber != _parent->_multiplexerchannel)
        {
            _parent->SwitchMultiplexerChannel(_channelnumber);
            _parent->_multiplexerchannel = _channelnumber;
        }
        return _parent->_I2CAccess->I2C_ReadMethod(address, data, length);
    }

    bool I2CMultiplexer_channel::I2C_ReadWriteMethod(uint8_t address, uint8_t* data, size_t readLength,
                                                     size_t writeLength)
    {
        if (_channelnumber != _parent->_multiplexerchannel)
        {
            _parent->SwitchMultiplexerChannel(_channelnumber);
            _parent->_multiplexerchannel = _channelnumber;
        }
        return _parent->_I2CAccess->I2C_ReadWriteMethod(address, data, readLength, writeLength);
    }

    bool I2CMultiplexer_channel::I2C_Mem_Read(uint8_t address, uint8_t memAddress, uint8_t memAddsize, uint8_t* data,
                                              size_t readLength)
    {
        if (_channelnumber != _parent->_multiplexerchannel)
        {
            _parent->SwitchMultiplexerChannel(_channelnumber);
            _parent->_multiplexerchannel = _channelnumber;
        }
        return _parent->_I2CAccess->I2C_Mem_Read(address, memAddress, memAddsize, data, readLength);
    }

    bool I2CMultiplexer_channel::I2C_IsDeviceReady(uint8_t address)
    {
        if (_channelnumber != _parent->_multiplexerchannel)
        {
            _parent->SwitchMultiplexerChannel(_channelnumber);
            _parent->_multiplexerchannel = _channelnumber;
        }
        return _parent->_I2CAccess->I2C_IsDeviceReady(address);
    }
}
