#include "AS5311.h"

namespace LowLevelEmbedded {
namespace Devices {
namespace Encoders {

    AS5311::AS5311(ISPIAccess* spi_access)
        : SPIAccess(spi_access),
          csID(0)
    {
        // Any chip-specific initialization here
    }

    /**
     * Low-level read of (up to) 24 bits from the AS5311.
     * We ask for 3 bytes, which is 24 bits total. The AS5311
     * only drives the first 18 bits meaningfully, then tri-states
     * or repeats bits. We'll discard the extra 6 bits below.
     */
    uint32_t AS5311::ReadRaw24Bits(SPIMode mode)
    {
        uint8_t rx[3] = {0, 0, 0};

        // Perform 3-byte (24-bit) read; no write bytes needed
        SPIAccess->WriteThenReadSPI(nullptr, 0, rx, 3, csID, mode);

        // Combine bytes into a 24-bit value:
        //   rx[0] becomes bits [23..16]
        //   rx[1] becomes bits [15..8]
        //   rx[2] becomes bits [7..0]
        uint32_t raw24 =
            (static_cast<uint32_t>(rx[0]) << 16) |
            (static_cast<uint32_t>(rx[1]) <<  8) |
            (static_cast<uint32_t>(rx[2]) <<  0);

        return raw24;
    }

    /**
     * Read the AS5311’s 18-bit output (12 bits of position + 6 status bits)
     * and parse it.
     *
     * Use SPI Mode 0 for Magnetic Strength Reading and SPI Mode 3 for Absolute Position
     * The first bit clocked out is D11 (M11) (position MSB).
     * The last bit clocked out is the parity bit (bit 0).
     */
    AS5311Data AS5311::ReadAndParseAll(SPIMode mode)
    {
        // Read 24 bits, but only the first 18 bits are meaningful for the AS5311
        const uint32_t raw24 = ReadRaw24Bits(mode);

        // Shift down so that bits [17..0] contain exactly the 18 bits of interest.
        // The top 18 bits in raw24 are in [23..6].
        // So we shift right by 6 to move them to [17..0].
        // However, the clock triggers the data capture on the Microcontroller right before it flips,
        // Making all the bits in the transfer lag by 1 bit. So 6 - 1 = 5 bits to shift
        uint32_t raw18 = raw24 >> 5;  // now bit17 = D11, bit0 = parity

        bool dataCheck = checkParity(raw18);
        if (!dataCheck)
        {
            AS5311Data data;
            data.rawData = raw24; // Raw UNSHIFTED Data
            data.dataOk = dataCheck;
            return data;
        }

        // Mask off just 18 bits, though for safety
        raw18 &= 0x3FFFF; // 18 bits set to 1

        // Now decode them:
        //  - position is bits [17..6] (12 bits total)
        //  - OCF is bit 5
        //  - COF is bit 4
        //  - LIN is bit 3
        //  - MagINC is bit 2
        //  - MagDEC is bit 1
        //  - parity is bit 0
        AS5311Data data;
        data.rawData = raw24; // Raw UNSHIFTED Data
        data.position = static_cast<uint16_t>((raw18 >> 6) & 0x0FFF); // 12-bit position
        data.magneticStrength = static_cast<uint8_t>(data.position >> 4); // 12-bit position
        data.ocf      = ((raw18 >> 5) & 0x1) != 0;
        data.cof      = ((raw18 >> 4) & 0x1) != 0;
        data.lin      = ((raw18 >> 3) & 0x1) != 0;
        data.magInc   = ((raw18 >> 2) & 0x1) != 0;
        data.magDec   = ((raw18 >> 1) & 0x1) != 0;
        data.dataOk   = dataCheck;

        return data;
    }

    /**
     * Convenience function: just return the 12-bit position value.
     * The correct SPI mode (0, 1, 2, or 3) depends on the AS5311
     * timing requirements—Mode1 or Mode3 are typical.
     */
    uint16_t AS5311::ReadPosition()
    {
        AS5311Data d = ReadAndParseAll(SPIMode::Mode3);
        return d.position;
    }

    /**
     * Another convenience function if you want to check flags, etc.
     */
    uint8_t AS5311::ReadMagneticPosition()
    {
        AS5311Data d = ReadAndParseAll(SPIMode::Mode0);
        return d.position >> 4;
    }

    bool AS5311::checkParity(uint32_t dataBits)
    {
        // Calculate Parity
        uint8_t count = 0;
        while (dataBits)
        {
            count += (dataBits & 1);
            dataBits >>= 1;
        }

        bool dataIsOdd = ((count & 0x01) != 0);
        return (!dataIsOdd);
    }


} // namespace Encoders
} // namespace Devices
} // namespace LowLevelEmbedded
