//
// Created by Jeroen Vandezande on 24/03/2022.
//

#ifndef _PIO_HPP_
#define _PIO_HPP_
#include <stdint.h>

class IPIO
{
 public:
	virtual ~IPIO() {};
	virtual bool WritePort(uint8_t data) = 0;
	virtual bool ReadPort(uint8_t* data) = 0;
};

#endif //_PIO_HPP_
