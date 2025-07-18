//
// Created by Jeroe on 2025-06-22.
//

#ifndef I2CMULTIPLEXER_BASE_H
#define I2CMULTIPLEXER_BASE_H
#include "../../Base/LLE_I2C.h"

namespace LowLevelEmbedded::Devices::I2CMultiplexers
{
    class I2CMultiplexer_channel;

    /**
     * @class I2CMultiplexer_base
     *
     * @brief A base class for managing I2C multiplexer functionality.
     *
     * This class provides the foundational structure for implementing I2C
     * multiplexers, enabling channel switching and I2C communication through the
     * multiplexer. Subclasses should implement the channel switching behavior
     * specific to a given multiplexer.
     */
    class I2CMultiplexer_base
    {
    public:
        I2CMultiplexer_base(II2CAccess* i2cAccess)
        {
            _I2CAccess = i2cAccess;
        }

        /**
         * @brief Creates and returns a channel for the I2C multiplexer.
         *
         * This method is responsible for creating an instance of the
         * I2CMultiplexer_channel associated with the specified channel number. The
         * new channel enables communication through the multiplexer on the specified
         * channel.
         *
         * @param channel The channel number to create on the I2C multiplexer.
         * @return A pointer to the newly created I2CMultiplexer_channel instance.
         */
        virtual I2CMultiplexer_channel* CreateChannel(uint8_t channel);

    protected:
        /**
         * @brief Abstract method for switching the I2C multiplexer channel.
         *
         * This method is responsible for changing the active channel on the
         * I2C multiplexer to the specified channel. Implementations should define
         * the necessary logic for enabling and managing the channel switching
         * process specific to the hardware being used.
         *
         * @param channel The channel number to switch to on the multiplexer.
         */
        virtual void SwitchMultiplexerChannel(uint8_t channel) = 0;
        friend class I2CMultiplexer_channel;
        II2CAccess* _I2CAccess;

    private:
        friend class I2CMultiplexer_channel;
        int16_t _multiplexerchannel = -1;
    };

    /**
     * @class I2CMultiplexer_channel
     *
     * @brief Represents a specific channel of an I2C multiplexer.
     *
     * This class provides an interface for communicating with devices connected to
     * a specific channel of an I2C multiplexer. It ensures that the correct channel
     * is selected before carrying out I2C operations. The class interacts with
     * the parent I2CMultiplexer_base to handle channel switching and delegates I2C
     * communication to the underlying hardware access implementation.
     *
     * The operations provided include read, write, combined read/write, memory
     * read, and checking the readiness of a device on the I2C bus.
     */
    class I2CMultiplexer_channel : public II2CAccess
    {
    public:
        bool I2C_ReadMethod(uint8_t address, uint8_t* data, size_t length) override;
        bool I2C_WriteMethod(uint8_t address, uint8_t* data, size_t length) override;
        bool I2C_ReadWriteMethod(uint8_t address, uint8_t* data, size_t readLength, size_t writeLength) override;
        bool I2C_Mem_Read(uint8_t address, uint8_t memAddress, uint8_t memAddsize, uint8_t* data,
                          size_t readLength) override;
        bool I2C_IsDeviceReady(uint8_t address) override;

    private:
        I2CMultiplexer_base* _parent;
        uint8_t _channelnumber;
        friend class I2CMultiplexer_base;

        I2CMultiplexer_channel(I2CMultiplexer_base* parent, uint8_t channelNumber)
        {
            _parent = parent;
            _channelnumber = channelNumber;
        }
    };
}

#endif //I2CMULTIPLEXER_BASE_H
