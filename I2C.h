#ifndef LOWLEVELCPPCLASSES_LIBRARY_H
#define LOWLEVELCPPCLASSES_LIBRARY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

namespace LowLevelEmbedded
{
    class II2CAccess
    {
    public:
        virtual ~II2CAccess()
        {
        };
        virtual bool I2C_ReadMethod(uint8_t address, uint8_t * data, size_t length);
        virtual bool I2C_WriteMethod(uint8_t address, uint8_t * data, size_t length);
        virtual bool I2C_ReadWriteMethod(uint8_t address, uint8_t * data, size_t readLength, size_t writeLength);
    };
}

#endif //LOWLEVELCPPCLASSES_LIBRARY_H
