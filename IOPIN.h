//
// Created by Jeroen Vandezande on 29/03/2022.
//

#ifndef _IOPIN_H_
#define _IOPIN_H_

#include "PIO.h"

namespace LowLevelEmbedded
{
	template <class T>
	class IOPIN
	{
	 private:
		IPIO<T> * pio;
		uint8_t pin;
	public:
		IOPIN(IPIO<T> * pio, uint8_t pin)
		{
			this->pio = pio;
			this->pin = pin;
		}

		void Set();
		void Clear();
		bool GetValue();
		void Toggle();

		explicit operator bool();
		IOPIN& operator = (bool value);
	};
}
#endif //_IOPIN_H_
