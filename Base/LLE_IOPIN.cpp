//
// Created by Jeroen Vandezande on 29/03/2022.
//

#include "LLE_IOPIN.h"
namespace LowLevelEmbedded
{
	void IOPIN_PIO8::Set()
	{
		this->pio->SetBit(this->pin);
	}

	void IOPIN_PIO8::Clear()
	{
		this->pio->ClearBit(this->pin);
	}

	bool IOPIN_PIO8::GetValue()
	{
		return this->pio->GetBit(this->pin);
	}

	void IOPIN_PIO8::Toggle()
	{
		this->pio->ToggleBit(this->pin);
	}
}