//
// Created by Jeroen Vandezande on 29/03/2022.
//

#include "IOPIN.h"
namespace LowLevelEmbedded
{
	void IOPIN::Set()
	{
		this->pio->SetBit(this->pin);
	}

	void IOPIN::Clear()
	{
		this->pio->ClearBit(this->pin);
	}

	bool IOPIN::GetValue()
	{
		return this->pio->GetBit(this->pin);
	}

	void IOPIN::Toggle()
	{
		this->pio->ToggleBit(this->pin);
	}
}