//
// Created by danan on 2024-08-20.
//

#ifndef EEPROM24AA08_H
#define EEPROM24AA08_H

#include <vector>
#include <stdint.h>

#include "LLE_I2C.h"

namespace LowLevelEmbedded::Devices::EEProm
{
    class EEProm24AA08
    {
    private:
        LowLevelEmbedded::II2CAccess* i2cAccess;
        std::vector<uint8_t> Block0WriteBuffer;
        std::vector<uint8_t> Block1WriteBuffer;
        std::vector<uint8_t> Block2WriteBuffer;
        std::vector<uint8_t> Block3WriteBuffer;
        std::vector<uint8_t> Block0ReadBuffer;
        std::vector<uint8_t> Block1ReadBuffer;
        std::vector<uint8_t> Block2ReadBuffer;
        std::vector<uint8_t> Block3ReadBuffer;
        std::vector<uint8_t> MainWriteBuffer[4]
        {
            Block0WriteBuffer,
            Block1WriteBuffer,
            Block2WriteBuffer,
            Block3WriteBuffer
        };
        std::vector<uint8_t> MainReadData[4]
        {
            Block0ReadBuffer,
            Block1ReadBuffer,
            Block2ReadBuffer,
            Block3ReadBuffer
        };
        std::vector<uint8_t> MainReadBuffer;
        const uint8_t WriteBufferAddresses[4]
        {
           0b10100000 | 0x00,
           0b10100000 | 0x02,
           0b10100000 | 0x04,
           0b10100000 | 0x06,
        };
        const uint8_t ReadBufferAddresses[4]
        {
           0b10100001 | 0x00,
           0b10100001 | 0x02,
           0b10100001 | 0x04,
           0b10100001 | 0x06,
        };
        /**
         * @brief Address within each 256 byte block (0-255)
         */
        uint8_t blockAddressDataPointer;
        /**
         * @brief block address Index (0 - 3)
         */
        uint8_t currentBlockIndex;
        uint8_t getBlockForVirtualAddress(uint16_t virtualAddress);
        void initWriteBuffers();
        void initReadBuffers();
        /**
         * @brief Polls the EEProm in order to figure out if the write operation is complete
         */
        bool ackPolling();
    public:
        EEProm24AA08(LowLevelEmbedded::II2CAccess* I2C_Access)
        {
            this->i2cAccess = I2C_Access;
        }
        /**
         * @brief Writes a byte array to the EEProm Chip. Maximum Address 1023.
         *
         */
        void WriteBytes(uint8_t address, uint8_t* data, size_t length);
        /**
         * @brief Reads a byte array to the EEProm Chip. Maximum Address 1023.
         *
         * @returns The Byte at the memory location
         *
         */
        void ReadBytes(uint8_t address, uint8_t*& data, size_t length);

        const uint8_t USED_I2C_ADDRESSES[8]
        {
            0b10100000 | 0x00, // 0xA0
            0b10100000 | 0x02,
            0b10100000 | 0x04,
            0b10100000 | 0x06,
            0b10101000 | 0x00, // 0xA8
            0b10101000 | 0x02,
            0b10101000 | 0x04,
            0b10101000 | 0x06
        };
    };
}

#endif //EEPROM24AA08_H
