#ifndef LOWLEVELCPPCLASSES_LIBRARY_H
#define LOWLEVELCPPCLASSES_LIBRARY_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

enum SPI_Mode_t{spmUndefined, spmMode0, spmMode1, spmMode2, spmMode3};

class ISPIAccess
{
 public:
	virtual ~ISPIAccess() {};
	virtual void ReadWriteSPI(uint8_t * data, size_t length, uint8_t cs_ID, enum SPI_Mode_t mode);
};

#endif //LOWLEVELCPPCLASSES_LIBRARY_H
