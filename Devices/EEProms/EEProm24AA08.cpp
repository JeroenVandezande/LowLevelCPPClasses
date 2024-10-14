//
// Created by danan on 2024-08-20.
//

#include "EEProm24AA08.h"
#include <stdexcept>
#include <stdint.h>

namespace LowLevelEmbedded::Devices::EEProm
{
    void EEProm24AA08::initWriteBuffers()
    {
        WriteBuffer->Clear();
    }

    void EEProm24AA08::initReadBuffers()
    {
        Block0ReadBuffer.clear();
        Block1ReadBuffer.clear();
        Block2ReadBuffer.clear();
        Block3ReadBuffer.clear();
        MainReadBuffer.clear();
    }

    uint8_t EEProm24AA08::getBlockForVirtualAddress(uint16_t virtualAddress)
    {
        if (virtualAddress < 256) return 0;
        if (virtualAddress < 512) return 1; // virtualAddress >= 256 implied
        if (virtualAddress < 768) return 2; // virtualAddress >= 512 implied
        if (virtualAddress < 1024) return 3; // virtualAddress >= 768 implied
        throw std::invalid_argument("Received Bad Address Value! Addresses are between 0-1023");
    }

    bool EEProm24AA08::ackPolling(int8_t blockIndex)
    {
        int i = 0;
        while (i < 40) // Poll on the device 20 times (50ms timeout for each)
        {
            uint8_t i2cAddress = WriteBuffer->Blocks[blockIndex]->BlockWriteAddress;
            if (i2cAccess->I2C_IsDeviceReady(i2cAddress))
            {
                return true;
            }
            i++;
        }
        return false;
    }


    void EEProm24AA08::WriteBytes(uint16_t address, uint8_t *data, size_t length)
    {
        // Clear Write buffers
        initWriteBuffers();

        // Fill Data
        WriteBuffer->AddData(address, data, length);

        // Transmit all write buffers for each block
        for (auto block: WriteBuffer->Blocks)
        {
            uint8_t i2cAddress = block->BlockWriteAddress;
            for (auto page: block->PageWriteBuffers)
            {
                i2cAccess->I2C_WriteMethod(i2cAddress, page.data(), page.size());
                while (!ackPolling(block->BlockNumber));
            }
        }
    }

    void EEProm24AA08::ReadBytes(uint16_t address, uint8_t*& data, size_t length)
    {
        // Clear Write buffers
        initReadBuffers();
        for (int i = 0; i < length; i++)
        {
            currentBlockIndex = getBlockForVirtualAddress(address);
            blockAddressDataPointer = address % 256;

            // Access block
            if (MainReadData[currentBlockIndex].size() == 0)
            {
                // push address
                MainReadData[currentBlockIndex].push_back(blockAddressDataPointer);

                // Add a length
                MainReadData[currentBlockIndex].push_back(0);
            }

            // Increment read length on this block
            MainReadData[currentBlockIndex][1]++;
        }

        // Read all data buffers for each block
        for (int i = 0; i < 4; i++)
        {
            if (MainReadData[i].size() == 0) continue;

            uint8_t currentBlockAddressPointer = MainReadData[i][0];
            uint8_t currentBlockReadLength = MainReadData[i][1];

            uint8_t i2cAddressRead = ReadBufferAddresses[i];

            // Sequential Read procedure
            uint8_t tempData[currentBlockReadLength];
            ackPolling(i); // Wait for block to be ready
            if (!i2cAccess->I2C_Mem_Read(i2cAddressRead, currentBlockAddressPointer, 1, tempData, currentBlockReadLength))
            {
                throw std::invalid_argument( "Data buffer was not filled by block as expected!" );
            };

            // Construct part of data array
            for (int j = 0; j < currentBlockReadLength; j++)
            {
                MainReadBuffer.push_back(tempData[j]);
            }
        }

        data = MainReadBuffer.data();
    }
}
