//
// Created by Jeroen Vandezande on 24/03/2022.
//

#ifndef _PIO_H_
#define _PIO_H_

#include <stdint.h>

namespace LowLevelEmbedded
{
	/// Generic Base class to use for all PIOs
	/// \tparam T The type / width of the PIO, mostly a Byte
	template <class T>
	class IPIO
	{
		T _Shadow;
	 public:
		virtual ~IPIO()
		{
		};
		/// Writes data to the PIO
		/// \param data The data you want to output
		virtual void WritePort(T data);
		/// Reads data from the PIO
		/// \return the Byte read from the PIO
		virtual T ReadPort() = 0;
		/// Sets a bit of the output port to 1
		/// \param bitNumber a zero based index of the bit number on the port
		void SetBit(uint8_t bitNumber);
		/// Clears a bit of the output port to 0
		/// \param bitNumber a zero based index of the bit number on the port
		void ClearBit(uint8_t bitNumber);
		/// Toggles a bit of the output port
		/// \param bitNumber a zero based index of the bit number on the port
		void ToggleBit(uint8_t bitNumber);
	};
}

#endif //_PIO_H_
