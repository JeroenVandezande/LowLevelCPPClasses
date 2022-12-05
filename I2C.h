#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

namespace LowLevelEmbedded
{
	/// This is the Interface for all derived I2C Access classes.
    class II2CAccess
    {
    public:
        virtual ~II2CAccess()
        {
        };
		/// a method to read from an I2C Slave device
		/// \param address The 7-bit address (left adjusted, bit 0 is ignored as it is the R/W bit)
		/// \param data a pointer to a uint8_t or and array of uint8_t
		/// \param length the length of the data
		/// \return true if no errors are detected
        virtual bool I2C_ReadMethod(uint8_t address, uint8_t * data, size_t length) = 0;
		/// a method to write to an I2C Slave device
		/// \param address The 7-bit address (left adjusted, bit 0 is ignored as it is the R/W bit)
		/// \param data a pointer to a uint8_t or and array of uint8_t
		/// \param length the length of the data
		/// \return true if no errors are detected
        virtual bool I2C_WriteMethod(uint8_t address, uint8_t * data, size_t length) = 0;
		/// a method to read and write to an I2C Slave, the same buffer is used to first write and then read
		/// \param address The 7-bit address (left adjusted, bit 0 is ignored as it is the R/W bit)
		/// \param data a pointer to a uint8_t or and array of uint8_t
		/// \param readLength the length of the data to read
		/// \param writeLength the length of the data to write
		/// \return true if no errors are detected
        virtual bool I2C_ReadWriteMethod(uint8_t address, uint8_t * data, size_t readLength, size_t writeLength) = 0;
    };
}

#endif //I2C_H
