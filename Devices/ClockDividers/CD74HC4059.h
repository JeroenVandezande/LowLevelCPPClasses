#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include "../../Base/LLE_IOPIN.h"

namespace LowLevelEmbedded::Devices::ClockDividers
{
    /// GPIO-programmed divider. CP is external; Q is a one-CP-period pulse.
    class CD74HC4059
    {
    public:
        enum class Mode : uint8_t { DivideBy2 = 2, DivideBy4 = 4, DivideBy5 = 5, DivideBy8 = 8, DivideBy10 = 10 };
        struct Program
        {
            uint16_t jam = 0; // bit 0 = J1, bit 15 = J16
            bool ka = false;
            bool kb = false;
            bool kc = false;
        };

        /// Pure encoder; supports the datasheet's design range, not extended counts.
        /// On failure leaves program unchanged. DivideBy8 supports all N=3..15999.
        [[nodiscard]] static bool Encode(uint16_t divisor, Mode mode, Program& program);

        /// jamPins are J1..J16, all required. All interfaces must outlive this object.
        /// waitClockPeriods(n) must block for AT LEAST n complete CP periods while
        /// CP keeps running. Four periods conservatively cover three full pulses
        /// regardless of entry phase. Callback must not throw or reenter the driver.
        CD74HC4059(std::array<IOPIN*, 16> jamPins, IOPIN& ka, IOPIN& kb, IOPIN& kc,
                   IOPIN& latchEnable, std::function<void(uint32_t)> waitClockPeriods);

        /// Programs under master preset, then starts counting with LE low.
        /// Invalid inputs/missing pins/callback return false without changing GPIO.
        /// Reprogramming is not glitch-free; stop downstream acquisition first.
        [[nodiscard]] bool Configure(uint16_t divisor, Mode mode = Mode::DivideBy8);

        /// Enter master preset and wait. A pulse already imminent may still occur.
        /// This is not an asynchronous output clamp or a guaranteed Q-low operation.
        [[nodiscard]] bool HoldInPreset();

        /// High latches the next output pulse high; low releases it. This is NOT
        /// a square-wave mode and does not stop the internal counter.
        void SetLatchEnabled(bool enabled);

    private:
        bool IsConnected() const;
        std::array<IOPIN*, 16> jamPins;
        IOPIN& ka;
        IOPIN& kb;
        IOPIN& kc;
        IOPIN& latchEnable;
        std::function<void(uint32_t)> waitClockPeriods;
    };
}
