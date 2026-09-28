#include "CD74HC4059.h"
#include <utility>

namespace LowLevelEmbedded::Devices::ClockDividers
{
    namespace
    {
        void SetPin(IOPIN& pin, bool high)
        {
            if (high)
                pin.Set();
            else
                pin.Clear();
        }
    }

    bool CD74HC4059::Encode(uint16_t divisor, Mode mode, Program& program)
    {
        Program result;
        unsigned remainderBits;
        switch (mode)
        {
        case Mode::DivideBy2: result.ka = true; result.kb = true; result.kc = true; remainderBits = 1; break;
        case Mode::DivideBy4: result.kb = true; result.kc = true; remainderBits = 2; break;
        case Mode::DivideBy5: result.ka = true; result.kc = true; remainderBits = 3; break;
        case Mode::DivideBy8: result.kc = true; remainderBits = 3; break;
        case Mode::DivideBy10: result.ka = true; result.kb = true; remainderBits = 4; break;
        default: return false;
        }
        const auto modulus = static_cast<uint16_t>(mode);
        const uint16_t maximum = (modulus == 5 || modulus == 10) ? 9999 : 15999;
        if (divisor < 3 || divisor > maximum)
            return false;

        const uint16_t quotient = divisor / modulus;
        // First nibble combines the remainder with the last counting section.
        result.jam = static_cast<uint16_t>((divisor % modulus) | ((quotient / 1000) << remainderBits)
                     | ((quotient % 10) << 4) | (((quotient / 10) % 10) << 8)
                     | (((quotient / 100) % 10) << 12));
        program = result;
        return true;
    }

    CD74HC4059::CD74HC4059(std::array<IOPIN*, 16> jamPins, IOPIN& ka, IOPIN& kb, IOPIN& kc,
                           IOPIN& latchEnable, std::function<void(uint32_t)> waitClockPeriods)
        : jamPins(jamPins), ka(ka), kb(kb), kc(kc), latchEnable(latchEnable),
          waitClockPeriods(std::move(waitClockPeriods))
    {
    }

    bool CD74HC4059::IsConnected() const
    {
        if (!waitClockPeriods)
            return false;
        for (auto* pin : jamPins)
            if (pin == nullptr)
                return false;
        return true;
    }

    bool CD74HC4059::HoldInPreset()
    {
        if (!IsConnected())
            return false;
        kb.Clear();
        kc.Clear();
        waitClockPeriods(4);
        return true;
    }

    bool CD74HC4059::Configure(uint16_t divisor, Mode mode)
    {
        Program program;
        if (!Encode(divisor, mode, program) || !IsConnected())
            return false;
        if (!HoldInPreset())
            return false;
        latchEnable.Clear();
        SetPin(ka, program.ka);
        for (size_t i = 0; i < jamPins.size(); ++i)
            SetPin(*jamPins[i], (program.jam & (uint16_t{1} << i)) != 0);
        // Allow the final jam values to propagate through the preset pipeline.
        waitClockPeriods(4);
        // For the default /8 mode only Kc changes when leaving preset.
        SetPin(kb, program.kb);
        SetPin(kc, program.kc);
        return true;
    }

    void CD74HC4059::SetLatchEnabled(bool enabled)
    {
        SetPin(latchEnable, enabled);
    }
}
