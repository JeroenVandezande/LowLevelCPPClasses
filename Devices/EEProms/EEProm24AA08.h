//
// Created by danan on 2024-08-20.
//

#ifndef EEPROM24AA08_H
#define EEPROM24AA08_H

#include <cmath>
#include <vector>
#include <stdint.h>

#include "../../Base/LLE_I2C.h"

namespace LowLevelEmbedded::Devices::EEProm
{
    class BlockWriteBuffer
    {
    private:
    public:
        BlockWriteBuffer(uint8_t blockNumber)
        {
            BlockNumber = blockNumber;
            BlockWriteAddress = 0b10100000 | (blockNumber * 2);
        }
        void AddData(uint8_t startingAddress, const std::vector<uint8_t>& data)
        {
            StartingDataAddress = startingAddress;
            Data = data;
            PageWriteBuffers.clear(); // Clear any existing pages

            uint8_t currentAddress = startingAddress;
            size_t dataIndex = 0;
            size_t dataSize = data.size();

            while (dataIndex < dataSize)
            {
                std::vector<uint8_t> currentPage;
                currentPage.push_back(currentAddress); // Include the starting address for this page

                size_t bytesInPage = 0;
                while (bytesInPage < 16 && dataIndex < dataSize)
                {
                    currentPage.push_back(data[dataIndex]);
                    dataIndex++;
                    bytesInPage++;
                }

                PageWriteBuffers.push_back(currentPage);
                currentAddress += bytesInPage; // Increment the address by the number of bytes written
            }
        }
        void ClearData()
        {
            StartingDataAddress = 0;
            Data.clear();
        }
        uint8_t StartingDataAddress;
        std::vector<uint8_t> Data;
        std::vector<std::vector<uint8_t>> PageWriteBuffers;
        uint8_t BlockNumber;
        uint8_t BlockWriteAddress;

};

    class MainWriteBuffer
    {
    private:
        BlockWriteBuffer* Block0 = new BlockWriteBuffer(0);
        BlockWriteBuffer* Block1 = new BlockWriteBuffer(1);
        BlockWriteBuffer* Block2 = new BlockWriteBuffer(2);
        BlockWriteBuffer* Block3 = new BlockWriteBuffer(3);
    public:
        void AddData(uint16_t startingAddress, const uint8_t* data, size_t length)
        {
            StartingDataAddress = startingAddress;

            // Copy data to the object's Data vector
            Data.assign(data, data + length);

            // Check if block contains data
            uint16_t dataStart = startingAddress;
            uint16_t dataEnd = startingAddress + length;
            for (auto block: Blocks)
            {
                uint16_t blockStart = block->BlockNumber * 256;
                uint16_t blockEnd = blockStart + 256;
                if (dataStart < blockEnd && blockStart < dataEnd)
                {
                    // Find the overlap range
                    uint16_t overlapStart = std::max(dataStart, blockStart);
                    uint16_t overlapEnd = std::min(dataEnd, blockEnd);

                    // Calculate the overlap length
                    uint16_t overlapLength = overlapEnd - overlapStart;

                    // Indices in the data buffer corresponding to the overlap
                    uint16_t dataIndexStart = overlapStart - dataStart;

                    // Get pointers for the data range
                    const uint8_t* dataRangeStart = data + dataIndexStart;
                    const uint8_t* dataRangeEnd = dataRangeStart + overlapLength;

                    // Copy data from data pointer to blockData vector
                    std::vector<uint8_t> blockData(dataRangeStart, dataRangeEnd);

                    // Adjust the starting address relative to the block
                    uint16_t blockRelativeAddress = overlapStart - blockStart;

                    // Call AddData on the block with the correct parameters
                    block->AddData(blockRelativeAddress, blockData);
                }
            }
        }

        void Clear()
        {
            for (auto block: Blocks)
            {
                block->ClearData();
            }
            StartingDataAddress = 0;
            Data.clear();
        }
        uint16_t StartingDataAddress;
        std::vector<uint8_t> Data;
        BlockWriteBuffer* Blocks[4]
        {
            Block0,
            Block1,
            Block2,
            Block3
        };
    };

    class EEProm24AA08
    {
    private:
        LowLevelEmbedded::II2CAccess* i2cAccess;
        std::vector<uint8_t> MainReadData[4];
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
        bool ackPolling(int8_t blockIndex);
    public:
        EEProm24AA08(LowLevelEmbedded::II2CAccess* I2C_Access)
        {
            this->i2cAccess = I2C_Access;
            this->WriteBuffer = new MainWriteBuffer();
        }
        /**
         * @brief Writes a byte array to the EEProm Chip. Maximum Address 1023.
         *
         */
        void WriteBytes(uint16_t address, uint8_t* data, size_t length);
        /**
         * @brief Reads a byte array to the EEProm Chip. Maximum Address 1023.
         *
         * @returns The Byte at the memory location
         *
         */
        void ReadBytes(uint16_t address, uint8_t*& data, size_t length);

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
        MainWriteBuffer* WriteBuffer;
    };
}

#endif //EEPROM24AA08_H
