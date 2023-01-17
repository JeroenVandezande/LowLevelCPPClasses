#ifndef LOWLEVELCPPCLASSES_LIBRARY_H
#define LOWLEVELCPPCLASSES_LIBRARY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

namespace LowLevelEmbedded
{
	enum class SPIMode
	{
		Undefined, Mode0, Mode1, Mode2, Mode3
	};

	class ISPIAccess
	{
	 public:
		virtual ~ISPIAccess()
		{
		};
		virtual void WriteSPI(uint8_t* data, size_t length, uint8_t cs_ID, enum SPIMode mode) = 0;
		virtual void ReadWriteSPI(uint8_t* data, size_t length, uint8_t cs_ID, enum SPIMode mode) = 0;
		virtual void WriteThenReadSPI(uint8_t* writedata, size_t writelength, uint8_t* readdata, size_t readlength, uint8_t cs_ID, enum SPIMode mode) = 0;
	};
}

#endif //LOWLEVELCPPCLASSES_LIBRARY_H
