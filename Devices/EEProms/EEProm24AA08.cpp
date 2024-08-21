//
// Created by danan on 2024-08-20.
//

#include "EEProm24AA08.h"
#include <stdexcept>

namespace LowLevelEmbedded::Devices::EEProm
{
    void EEProm24AA08::initWriteBuffers()
    {
        Block0WriteBuffer.clear();
        Block1WriteBuffer.clear();
        Block2WriteBuffer.clear();
        Block3WriteBuffer.clear();
    }

    void EEProm24AA08::initReadBuffers()
    {
        Block0ReadBuffer.clear();
        Block1ReadBuffer.clear();
        Block2ReadBuffer.clear();
        Block3ReadBuffer.clear();
    }

    uint8_t EEProm24AA08::getBlockForVirtualAddress(uint16_t virtualAddress)
    {
        if (virtualAddress < 256) return 0;
        if (256 >= virtualAddress && virtualAddress < 512) return 1;
        if (512 >= virtualAddress && virtualAddress < 768) return 2;
        if (768 >= virtualAddress && virtualAddress < 1024) return 3;
        if (virtualAddress >= 1024)
        {
            throw std::invalid_argument( "Received Bad Address Value! Addresses are between 0-1023" );
        }
        return 0;
    }

    bool EEProm24AA08::ackPolling()
    {
        int i = 0;
        while (i < 3) // Poll on the device 3 times (50ms timeout for each)
        {
            uint8_t i2cAddress = WriteBufferAddresses[i];
            if (i2cAccess->I2C_WriteMethod(i2cAddress, NULL, 0) != HAL_OK)
            {
                return true;
            }
            i++;
        }
        return false;
    }


    void EEProm24AA08::WriteBytes(uint8_t address, uint8_t *data, size_t length)
    {
        // Clear Write buffers
        initWriteBuffers();
        for (int i = 0; i < length; i++)
        {
            currentBlockIndex = getBlockForVirtualAddress(address);
            blockAddressDataPointer = address % 256;

            // Access block
            std::vector<uint8_t> currentBlock = MainWriteBuffer[currentBlockIndex];
            if (currentBlock.size() == 0)
            {
                // push address
                currentBlock.emplace_back(blockAddressDataPointer);
            }

            // Push in data
            currentBlock.emplace_back(data[i]);
        }

        // Transmit all write buffers for each block
        for (int i = 0; i < MainWriteBuffer->size(); i++)
        {
            if (MainWriteBuffer[i].size() == 0) continue;
            uint8_t i2cAddress = WriteBufferAddresses[i];
            uint8_t* transmissionData = MainWriteBuffer[i].data();
            i2cAccess->I2C_WriteMethod(i2cAddress, transmissionData, MainWriteBuffer[i].size());
            ackPolling();
        }
    }

    void EEProm24AA08::ReadBytes(uint8_t address, uint8_t*& data, size_t length)
    {
        // Clear Write buffers
        initReadBuffers();
        for (int i = 0; i < length; i++)
        {
            currentBlockIndex = getBlockForVirtualAddress(address);
            blockAddressDataPointer = address % 256;

            // Access block
            std::vector<uint8_t> currentBlock = MainReadData[currentBlockIndex];
            if (currentBlock.size() == 0)
            {
                // push address
                currentBlock.emplace_back(blockAddressDataPointer);

                // Add a length
                currentBlock.emplace_back(0);
            }

            // Increment read length on this block
            currentBlock[1]++;
        }

        // Read all data buffers for each block
        for (int i = 0; i < MainReadData->size(); i++)
        {
            if (MainReadData[i].size() == 0) continue;

            uint8_t currentBlockAddressPointer = MainReadData[i][0];
            uint8_t currentBlockReadLength = MainReadData[i][1];

            uint8_t i2cAddressRead = ReadBufferAddresses[i];
            uint8_t i2cAddressWrite = WriteBufferAddresses[i];
            uint8_t writeData[1];
            writeData[0] = currentBlockAddressPointer;

            // Sequential Read procedure
            uint8_t tempData[currentBlockReadLength];
            i2cAccess->I2C_WriteMethod(i2cAddressWrite, writeData, length);
            if (i2cAccess->I2C_ReadMethod(i2cAddressRead, tempData, currentBlockReadLength))
            {
                throw std::invalid_argument( "Data buffer was not filled by block as expected!" );
            };

            // Construct part of data array
            for (int j = 0; j < currentBlockReadLength; i++)
            {
                MainReadBuffer.emplace_back(tempData[i]);
            }
        }

        data = MainReadBuffer.data();
    }


}
