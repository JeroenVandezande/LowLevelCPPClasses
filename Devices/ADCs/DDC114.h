#pragma once

#include <array>
#include <cstdint>
#include "../../Base/LLE_IOPIN.h"
#include "../../Base/LLE_SPI.h"

namespace LowLevelEmbedded::Devices::ADCs
{
    /// Single DDC114, with externally generated CLK and CONV. No chip select exists.
    /// SPI must be synchronous, MSB first, 8-bit, Mode0, with no extra clock pulses.
    /// All referenced interfaces must outlive this object. Calls must be serialized.
    class DDC114
    {
    public:
        enum class Range : uint8_t
        {
            PicoCoulombs12, PicoCoulombs50, PicoCoulombs100, PicoCoulombs150,
            PicoCoulombs200, PicoCoulombs250, PicoCoulombs300, PicoCoulombs350
        };

        enum class Format : uint8_t { Bits16, Bits20 };
        enum class ReadResult { NotInitialized, NotReady, Settling, Ready };

        struct Samples
        {
            /// Native unsigned codes in AIN1, AIN2, AIN3, AIN4 order.
            /// Zero input is nominally 4096 (20-bit) or 256 (16-bit).
            std::array<uint32_t, 4> channels{};
            Format format = Format::Bits20;
        };

        struct ControlPins
        {
            /// Null means hardwired/external control; configuration must match hardware.
            IOPIN* range0 = nullptr;
            IOPIN* range1 = nullptr;
            IOPIN* range2 = nullptr;
            IOPIN* format = nullptr;
            IOPIN* test = nullptr;
        };

        /// csID is only an adapter routing ID; configure that route without a CS GPIO.
        DDC114(ISPIAccess& spi, IOPIN& dataValid, ControlPins pins, uint8_t csID = 0);

        /// Call with CONV stopped, after board power/reset sequencing. Sets TEST low.
        /// Does not generate CLK/CONV or drive RESET. Rejects invalid enum values.
        /// The next four retrieved conversions are discarded. Call again after reset
        /// or range/format changes; do not change FORMAT during a serial transfer.
        [[nodiscard]] bool Initialize(Range range, Format format = Format::Bits20);

        [[nodiscard]] bool IsDataReady() const;

        /// Polls active-low DVALID; never waits for a conversion. A ready read performs
        /// one blocking 8/10-byte SPI transaction, including when discarding startup data.
        /// Output is unchanged unless Ready is returned. Caller must keep the transfer
        /// >=10 us away from CONV edges and finish before the next result replaces it.
        /// ISPIAccess cannot report errors: Ready does not guarantee transport success.
        [[nodiscard]] ReadResult TryRead(Samples& samples);

    private:
        ISPIAccess& spi;
        IOPIN& dataValid;
        ControlPins pins;
        uint8_t csID;
        Format format = Format::Bits20;
        uint8_t settlingConversions = 0;
        bool initialized = false;
    };
}
