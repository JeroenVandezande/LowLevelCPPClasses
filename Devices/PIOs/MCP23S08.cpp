#include "MCP23S08.h"

namespace LowLevelEmbedded
{
	namespace Devices
	{
		namespace PIOs
		{
			enum class Registers : uint8_t
			{
				IODIR = 0x00,
				IPOL = 0x01,
				GPINTEN = 0x02,
				GPIO = 0x09
			};

			void MCP23S08::SetPortDirection(uint8_t direction)
			{
				uint8_t buffer[3] = { static_cast<uint8_t>(this->_address & 0b11111110), static_cast<uint8_t>(Registers::IODIR), direction };
				this->SPIAccess->ReadWriteSPI(&buffer[0], 3, this->_csID, spmMode0);
			}

			void MCP23S08::WritePort(uint8_t data)
			{
				uint8_t buffer[3] = { static_cast<uint8_t>(this->_address & 0b11111110), static_cast<uint8_t>(Registers::GPIO), data };
				this->SPIAccess->ReadWriteSPI(&buffer[0], 3, this->_csID, spmMode0);
				IPIO<uint8_t>::WritePort(data);
			}

			uint8_t MCP23S08::ReadPort()
			{
				uint8_t buffer[3] = { static_cast<uint8_t>(this->_address | 0b00000001), static_cast<uint8_t>(Registers::GPIO), 0 };
				this->SPIAccess->ReadWriteSPI(&buffer[0], 3, this->_csID, spmMode0);
				return buffer[2];
			}

			MCP23S08::MCP23S08(uint8_t aAddress, uint8_t aCS_ID)
			{
				this->_shadow = 0;
				this->_address = (aAddress & 0b00000110) | BASEADDRESS;
				this->_csID = aCS_ID;
			}
		}
	}
}