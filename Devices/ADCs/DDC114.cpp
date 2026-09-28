#include "DDC114.h"

namespace LowLevelEmbedded::Devices::ADCs
{
    namespace
    {
        void SetPin(IOPIN* pin, bool high)
        {
            if (pin != nullptr)
            {
                if (high)
                    pin->Set();
                else
                    pin->Clear();
            }
        }
    }

    DDC114::DDC114(ISPIAccess& spi, IOPIN& dataValid, ControlPins pins, uint8_t csID)
        : spi(spi), dataValid(dataValid), pins(pins), csID(csID)
    {
    }

    bool DDC114::Initialize(Range range, Format newFormat)
    {
        const auto rangeCode = static_cast<uint8_t>(range);
        if (rangeCode > 7 || (newFormat != Format::Bits16 && newFormat != Format::Bits20))
            return false;

        SetPin(pins.test, false);
        SetPin(pins.range0, (rangeCode & 1U) != 0);
        SetPin(pins.range1, (rangeCode & 2U) != 0);
        SetPin(pins.range2, (rangeCode & 4U) != 0);
        SetPin(pins.format, newFormat == Format::Bits20);
        format = newFormat;
        settlingConversions = 4;
        initialized = true;
        return true;
    }

    bool DDC114::IsDataReady() const
    {
        return initialized && !dataValid.GetValue();
    }

    DDC114::ReadResult DDC114::TryRead(Samples& samples)
    {
        if (!initialized)
            return ReadResult::NotInitialized;
        if (!IsDataReady())
            return ReadResult::NotReady;

        std::array<uint8_t, 10> bytes{};
        const size_t bitsPerChannel = format == Format::Bits20 ? 20 : 16;
        // DOUT is already presenting AIN4's MSB when DVALID falls. Mode0 samples
        // it on the first rising edge, then the falling edge advances the output.
        spi.ReadWriteSPI(bytes.data(), bitsPerChannel / 2, csID, SPIMode::Mode0);

        if (settlingConversions != 0)
        {
            --settlingConversions;
            return ReadResult::Settling;
        }

        Samples decoded;
        decoded.format = format;
        // Wire order is AIN4, AIN3, AIN2, AIN1; 20-bit words cross byte boundaries.
        for (size_t word = 0; word < 4; ++word)
        {
            uint32_t code = 0;
            for (size_t bit = 0; bit < bitsPerChannel; ++bit)
            {
                const size_t offset = word * bitsPerChannel + bit;
                code = (code << 1) | ((bytes[offset / 8] >> (7 - offset % 8)) & 1U);
            }
            decoded.channels[3 - word] = code;
        }
        samples = decoded;
        return ReadResult::Ready;
    }
}
