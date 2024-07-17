//
// Created by Jeroen on 2024-02-22.
//

#include "AD7175.h"
#include "AD7175_Constants.h"
#include "LLE_IOPIN.h"
#include "LLE_SPI.h"

namespace LowLevelEmbedded::Devices::ADCs
{
    void AD7175::WriteRegister8(uint8_t reg, uint8_t value)
    {

    }
    void AD7175::WriteRegister16(uint8_t reg, uint16_t value)
    {

    }
    void AD7175::WriteRegister24(uint8_t reg, uint32_t value)
    {

    }

    int8_t AD7175::ReadRegister8(uint8_t reg)
    {
        uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        /* Build the Command word */
        buffer[0] = COMM_REG_WEN | COMM_REG_RD | reg;

        /* Read data from the device */
        SPIAccess->ReadWriteSPI(&buffer[0], 2, SPIAccess, spmMode3);

        return buffer[1];
    }
    int16_t AD7175::ReadRegister16(uint8_t reg)
    {

    }
    int32_t AD7175::ReadRegister24(uint8_t reg)
    {

    }

    uint32_t AD7175::GetADCValue(uint8_t channelIndex)
    {
        return 1;
    }

    AD7175::AD7175(ISPIAccess *spi_access, uint8_t cs_ID, const std::function<void>* configureMethod)
    {
        if (configureMethod != nullptr) configureMethod();
        SPIAccess = spi_access;
        csID = cs_ID;
    }
}