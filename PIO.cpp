//
// Created by Jeroen Vandezande on 24/03/2022.
//

#include "PIO.h"

namespace LowLevelEmbedded
{
	template <class T>
	void IPIO<T>::WritePort(T data)
	{
		this->_Shadow = data;
	}

	template <class T>
	void IPIO<T>::SetBit(uint8_t bitNumber)
	{
		T modifiedPort = this->_Shadow | 1UL << bitNumber;
		this->WritePort(modifiedPort);
		this->_Shadow = modifiedPort;
	}

	template <class T>
	void IPIO<T>::ClearBit(uint8_t bitNumber)
	{
		T modifiedPort = this->_Shadow & ~(1UL << bitNumber);
		this->WritePort(modifiedPort);
		this->_Shadow = modifiedPort;
	}

	template <class T>
	void IPIO<T>::ToggleBit(uint8_t bitNumber)
	{
		T modifiedPort = this->_Shadow ^ 1UL << bitNumber;
		this->WritePort(modifiedPort);
		this->_Shadow = modifiedPort;
	}

}