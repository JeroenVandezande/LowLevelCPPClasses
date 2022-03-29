//
// Created by Jeroen Vandezande on 29/03/2022.
//

#include "IOPIN.h"
namespace LowLevelEmbedded
{
	template <class T>
	void IOPIN<T>::Set()
	{
		this->pio->SetBit(this->pin);
	}

	template <class T>
	void IOPIN<T>::Clear()
	{
		this->pio->ClearBit(this->pin);
	}

	template <class T>
	bool IOPIN<T>::GetValue()
	{
		return this->pio->GetBit(this->pin);
	}

	template <class T>
	void IOPIN<T>::Toggle()
	{
		this->pio->ToggleBit(this->pin);
	}

	template <class T>
	IOPIN<T>::operator bool()
	{
		return this->GetValue();
	}

	template <class T>
	IOPIN<T>& IOPIN<T>::operator = (bool value)
	{
		if (value)
		{
			this->Set();
		}
		else
		{
			this->Clear();
		}
		return *this;
	}

}