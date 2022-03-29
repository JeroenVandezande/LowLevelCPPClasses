//
// Created by Jeroen Vandezande on 24/03/2022.
//

#include "PIO.h"

namespace LowLevelEmbedded
{
	template <class T>
	void IPIO<T>::SetBit(uint8_t bitNumber)
	{
		T modifiedPort = this->_shadow | 1UL << bitNumber;
		this->WritePort(modifiedPort);
		this->_shadow = modifiedPort;
	}

	template <class T>
	void IPIO<T>::ClearBit(uint8_t bitNumber)
	{
		T modifiedPort = this->_shadow & ~(1UL << bitNumber);
		this->WritePort(modifiedPort);
		this->_shadow = modifiedPort;
	}

	template <class T>
	void IPIO<T>::ToggleBit(uint8_t bitNumber)
	{
		T modifiedPort = this->_shadow ^ 1UL << bitNumber;
		this->WritePort(modifiedPort);
		this->_shadow = modifiedPort;
	}

}