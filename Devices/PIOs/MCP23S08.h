#ifndef MCP23S08_LIBRARY_H
#define MCP23S08_LIBRARY_H
#include <functional>
#include "SPI.h"
#include "PIO.h"

namespace LowLevelEmbedded
{
	namespace Devices
	{
		namespace PIOs
		{
			/// a Class that represents the MCP23S08 8-bit SPI PIO from Microchip.
			/// see <href https://www.microchip.com/en-us/product/MCP23S08> for more info
			class MCP23S08 : public IPIO_8
			{
				const uint8_t BASEADDRESS = 0b01000000;
				uint8_t _address;
				uint8_t _csID;
			 public:
				void SetPortDirection(uint8_t direction) override;
				void WritePort(uint8_t data) override;
				uint8_t ReadPort() override;
				ISPIAccess* SPIAccess;
				MCP23S08(uint8_t aAddress, uint8_t aCS_ID);
			};
		}
	}
}

#endif //MCP23S08_LIBRARY_H
