//
// Created by Jeroen Vandezande on 24/03/2022.
//

#ifndef _PIO_H_
#define _PIO_H_

#include <stdint.h>

namespace LowLevelEmbedded
{
	/// Generic Base class to use for all PIOs
	class IPIO
	{
	 public:
		virtual ~IPIO()
		{
		};
		/// Sets a bit of the output port to 1
		/// \param bitNumber a zero based index of the bit number on the port
		virtual void SetBit(uint8_t bitNumber) = 0;
		/// Clears a bit of the output port to 0
		/// \param bitNumber a zero based index of the bit number on the port
		virtual void ClearBit(uint8_t bitNumber) = 0;
		/// Toggles a bit of the output port
		/// \param bitNumber a zero based index of the bit number on the port
		virtual void ToggleBit(uint8_t bitNumber) = 0;
		/// Reads the value of the input port
		/// \param bitNumber a zero based index of the bit number on the port
		virtual bool GetBit(uint8_t bitNumber) = 0;
	};

	/// Generic Base class to use for all PIOs
	class IPIO_8: public IPIO
	{
	 protected:
		uint8_t _shadow;
	 public:
		virtual ~IPIO_8()
		{
		};
		/// Writes data to the PIO
		/// \param direction The direction of the port pins, a 1 is an input pin and a 0 is an output pin
		virtual void SetPortDirection(uint8_t direction) = 0;
		/// Writes data to the PIO
		/// \param data The data you want to output
		virtual void WritePort(uint8_t data) = 0;
		/// Reads data from the PIO
		/// \return the Byte read from the PIO
		virtual uint8_t ReadPort() = 0;
		/// Sets a bit of the output port to 1
		/// \param bitNumber a zero based index of the bit number on the port
		void SetBit(uint8_t bitNumber) override;
		/// Clears a bit of the output port to 0
		/// \param bitNumber a zero based index of the bit number on the port
		void ClearBit(uint8_t bitNumber) override;
		/// Toggles a bit of the output port
		/// \param bitNumber a zero based index of the bit number on the port
		void ToggleBit(uint8_t bitNumber) override;
		/// Reads the value of the input port
		/// \param bitNumber a zero based index of the bit number on the port
		bool GetBit(uint8_t bitNumber) override;
	};
}

#endif //_PIO_H_
