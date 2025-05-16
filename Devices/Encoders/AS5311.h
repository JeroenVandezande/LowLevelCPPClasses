//
// Created by DanaNatov on 4/16/2025.
//

#ifndef AS5311_H
#define AS5311_H
#include "LLE_IOPIN.h"
#include "LLE_SPI.h"

namespace LowLevelEmbedded {
    namespace Devices {
        namespace Encoders {

            // A small struct to hold the parsed results.
            struct AS5311Data
            {
                uint32_t rawData;  // 24 bits: [24..6] first 6 bits are useless
                uint16_t position;  // 12 bits: [17..6] from the 18-bit stream
                uint8_t magneticStrength;  // 8 bits: [17..10] from the 18-bit stream (M11:M4)
                bool     ocf;       // Bit 5
                bool     cof;       // Bit 4
                bool     lin;       // Bit 3
                bool     magInc;    // Bit 2
                bool     magDec;    // Bit 1
                bool     dataOk; // Bool flag to indicate data is good
            };

            class AS5311 {
            public:
                AS5311(ISPIAccess *spi_access);
                uint32_t ReadRaw24Bits(SPIMode mode);
                AS5311Data ReadAndParseAll(SPIMode mode);
                uint16_t ReadPosition();
                uint8_t ReadMagneticPosition();
            private:
                bool checkParity(uint32_t dataBits);
                ISPIAccess* SPIAccess;
                uint8_t csID;
            };
        }
    }
}
#endif //AS5311_H
