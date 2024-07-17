//
// Created by Jeroen Vandezande on 24/03/2022.
//

#include "LLE_PIO.h"
#include "LLE_IOPIN.h"

namespace LowLevelEmbedded
{

	void IPIO_8::SetBit(uint8_t bitNumber)
	{
		uint8_t modifiedPort = this->_shadow | 1UL << bitNumber;
		this->WritePort(modifiedPort);
		this->_shadow = modifiedPort;
	}


	void IPIO_8::ClearBit(uint8_t bitNumber)
	{
		uint8_t modifiedPort = this->_shadow & ~(1UL << bitNumber);
		this->WritePort(modifiedPort);
		this->_shadow = modifiedPort;
	}


	void IPIO_8::ToggleBit(uint8_t bitNumber)
	{
		uint8_t modifiedPort = this->_shadow ^ 1UL << bitNumber;
		this->WritePort(modifiedPort);
		this->_shadow = modifiedPort;
	}

	bool IPIO_8::GetBit(uint8_t bitNumber)
	{
		return (this->ReadPort() & (1UL << bitNumber)) != 0;
	}

	IOPIN* IPIO_8::CreateIOPin(uint8_t bitNumber)
	{
		return new IOPIN_PIO8(this, bitNumber);
	}

}