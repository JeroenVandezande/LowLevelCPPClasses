//
// Created by Jeroen on 2024-02-22.
//

#include "AD7175.h"
#include "AD7175_Constants.h"
#include "LLE_IOPIN.h"
#include "LLE_SPI.h"

namespace LowLevelEmbedded::Devices::ADCs
{
    uint8_t AD7175::getChannelConfigAddress(uint8_t aChannelIndex)
    {
        switch (aChannelIndex)
        {
            case 0:
                return AD7175_CH0;
            break;
            case 1:
                return AD7175_CH1;
            break;
            case 2:
                return AD7175_CH2;
            break;
            case 3:
                return AD7175_CH3;
            break;
            default:
                return AD7175_CH0;
        }
    }

    void AD7175::WriteRegister8(uint8_t reg, uint8_t value)
    {
        uint8_t wrBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        /* Build the Command word */
        wrBuf[0] = COMM_REG_WEN | COMM_REG_WR | reg;

        /* Fill the write buffer */
        wrBuf[1] = value;

        /* Write data to the device */
        SPIAccess->ReadWriteSPI(&wrBuf[0], 2, csID, SPIMode::Mode3);
    }

    void AD7175::WriteRegister16(uint8_t reg, uint16_t value)
    {
        uint8_t wrBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        /* Build the Command word */
        wrBuf[0] = COMM_REG_WEN | COMM_REG_WR | reg;

        /* Fill the write buffer */
        for (int i = 0; i < 2; i++)
        {
            wrBuf[2 - i] = value & 0xFF;
            value >>= 8;
        }

        /* Write data to the device */
        SPIAccess->ReadWriteSPI(&wrBuf[0], 3, csID, SPIMode::Mode3);
    }

    void AD7175::WriteRegister24(uint8_t reg, uint32_t value)
    {
        uint8_t wrBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        /* Build the Command word */
        wrBuf[0] = COMM_REG_WEN | COMM_REG_WR | reg;

        /* Fill the write buffer */
        for (int i = 0; i < 3; i++)
        {
            wrBuf[3 - i] = value & 0xFF;
            value >>= 8;
        }

        /* Write data to the device */
        SPIAccess->ReadWriteSPI(&wrBuf[0], 4, csID, SPIMode::Mode3);
    }

    int8_t AD7175::ReadRegister8(uint8_t reg)
    {
        uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        /* Build the Command word */
        buffer[0] = COMM_REG_WEN | COMM_REG_RD | reg;

        /* Read data from the device */
        SPIAccess->ReadWriteSPI(&buffer[0], 2, csID, SPIMode::Mode3);

        return buffer[1];
    }
    int16_t AD7175::ReadRegister16(uint8_t reg)
    {
        int16_t ret = 0;
        uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        /* Build the Command word */
        buffer[0] = COMM_REG_WEN | COMM_REG_RD | reg;

        /* Read data from the device */
        SPIAccess->ReadWriteSPI(&buffer[0], 3, csID, SPIMode::Mode3);

        /* Build the result */
        for (int i = 1; i < 3; i++)
        {
            ret <<= 8;
            ret += buffer[i];
        }

        return ret;
    }
    int32_t AD7175::ReadRegister24(uint8_t reg)
    {
        int32_t ret = 0;
        uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        /* Build the Command word */
        buffer[0] = COMM_REG_WEN | COMM_REG_RD | reg;

        /* Read data from the device */
        SPIAccess->ReadWriteSPI(&buffer[0], 4, csID, SPIMode::Mode3);

        /* Build the result */
        for (int i = 1; i < 4; i++)
        {
            ret <<= 8;
            ret += buffer[i];
        }

        return ret;
    }

    void AD7175::ChangeChannelAnalogPinInput(uint8_t channelIndex, uint8_t analogPinInput)
    {
        // Check if pin was already used on the channel. In this case we need to do nothing
        if ((lastUsedChannel == channelIndex) && (lastUsedPin == analogPinInput)) return;
        // Check configuration for channel and pin input
        uint8_t regAddress = getChannelConfigAddress(channelIndex);
        uint16_t channelValue = ReadRegister16(regAddress);
        // Check if channelValue is already set correctly
        // Mask off channel analog pin input portion
        uint16_t maskedValue = (channelValue & 0x03E0);
        // Shift the masked bits to the right by 5 positions
        maskedValue >>= 5;
        if (maskedValue == analogPinInput) return;
        // Otherwise change the channel
        // Adding pin to channel
        uint16_t analogInPinMask = 0xFC1F; // Masks off the analog pin selection
        uint16_t analogInPinSelection = analogPinInput << 5;
        channelValue = channelValue & analogInPinMask; // Mask off pins
        channelValue = channelValue | analogInPinSelection; // Add requested pin
        WriteRegister16(regAddress, channelValue);
        lastUsedPin = analogPinInput;
        lastUsedChannel = channelIndex;
    }

    uint32_t AD7175::GetADCValue(uint8_t channelIndex)
    {
        if (this->lastUsedChannel != channelIndex) //change channel
        {
            if (this->lastUsedChannel < 5) // turn off old channel
            {
                uint8_t regAddress = getChannelConfigAddress(this->lastUsedChannel);
                uint16_t prevValue = ReadRegister16(regAddress);
                prevValue &= ~CH_MAP_REG_CHEN;
                WriteRegister16(regAddress, prevValue);
            }
            uint8_t regAddress = getChannelConfigAddress(channelIndex);
            uint16_t prevValue = ReadRegister16(regAddress);
            prevValue |= CH_MAP_REG_CHEN;
            WriteRegister16(regAddress, prevValue);
            this->lastUsedChannel = channelIndex;
        }

        //start ADC
        uint16_t mode = ReadRegister16(AD7175_ADCMODE) & 0b1110011100001100;
        mode |= ADC_MODE_REG_MODE_SINGLE;
        WriteRegister16(AD7175_ADCMODE, mode);

        //wait until ready
        bool ready = false;
        while (!ready)
        {
            /* Read the value of the Status Register */
            uint8_t stat = ReadRegister8(AD7175_STATUS);
            /* Check the RDY bit in the Status Register */
            ready = ((stat & STATUS_REG_RDY) == 0);
        }

        //read ADC data
        return ReadRegister24(AD7175_DATA);
    }

    void AD7175_IOPin::Clear()
    {
        uint16_t gpioReg = chipADC->ReadRegister16(AD7175_GPIOCON);
        if (gpioPinNumber == 0)
        {
            uint16_t newGpioReg = (uint16_t) gpioReg & ~(0x1);
            chipADC->WriteRegister16(AD7175_GPIOCON, newGpioReg);
        } else
        {
            uint16_t newGpioReg = (uint16_t) gpioReg & ~(0x2);
            chipADC->WriteRegister16(AD7175_GPIOCON, newGpioReg);
        }
    }

    void AD7175_IOPin::Set()
    {
        uint16_t gpioReg = chipADC->ReadRegister16(AD7175_GPIOCON);
        if (gpioPinNumber == 0)
        {
            uint16_t newGpioReg = (uint16_t) gpioReg | 0x1;
            chipADC->WriteRegister16(AD7175_GPIOCON, newGpioReg);
        } else
        {
            uint16_t newGpioReg = (uint16_t) gpioReg | 0x2;
            chipADC->WriteRegister16(AD7175_GPIOCON, newGpioReg);
        }
    }

    void AD7175_IOPin::Toggle()
    {
        if (this->GetValue())
        {
            this->Clear();
        }
        else
        {
            this->Set();
        }
    }

    bool AD7175_IOPin::GetValue()
    {
        uint16_t gpioReg = chipADC->ReadRegister16(AD7175_GPIOCON);

        if (gpioPinNumber == 0)
        {
            return gpioReg & 0x1;
        } else
        {
            return gpioReg & 0x2;
        }
    }
    void AD7175::Initialize()
    {
        if (initializeMethodPtr != nullptr) initializeMethodPtr();
    }

    AD7175::AD7175(ISPIAccess *spi_access, uint8_t cs_ID, const std::function<void()>& configureMethod)
    {
        SPIAccess = spi_access;
        csID = cs_ID;
        lastUsedChannel = 255; // Initialize to unused channel
        lastUsedPin = 255; // Init unused pin
        GPIO0 = new AD7175_IOPin(this, 0);
        GPIO1 = new AD7175_IOPin(this, 1);
        initializeMethodPtr = configureMethod;
    }
}