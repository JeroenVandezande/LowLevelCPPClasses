#include "CD74HC4059.h"
#include <cassert>
#include <iostream>
#include <vector>
using namespace LowLevelEmbedded;
using namespace LowLevelEmbedded::Devices::ClockDividers;
using Divider = CD74HC4059;

struct Pin : IOPIN
{
    bool value = false;
    unsigned writes = 0;
    void Set() override { value = true; ++writes; }
    void Clear() override { value = false; ++writes; }
    bool GetValue() override { return value; }
    void Toggle() override { value = !value; ++writes; }
};

int main()
{
    struct ModeCase { Divider::Mode mode; unsigned bits; unsigned max; bool ka, kb, kc; };
    const ModeCase cases[] = {
        {Divider::Mode::DivideBy2, 1, 15999, true, true, true},
        {Divider::Mode::DivideBy4, 2, 15999, false, true, true},
        {Divider::Mode::DivideBy5, 3, 9999, true, false, true},
        {Divider::Mode::DivideBy8, 3, 15999, false, false, true},
        {Divider::Mode::DivideBy10, 4, 9999, true, true, false}
    };
    for (auto c : cases)
    {
        for (unsigned n = 3; n <= c.max; ++n)
        {
            Divider::Program p;
            assert(Divider::Encode(n, c.mode, p));
            const unsigned ones = (p.jam >> 4) & 15;
            const unsigned tens = (p.jam >> 8) & 15;
            const unsigned hundreds = (p.jam >> 12) & 15;
            const unsigned thousands = (p.jam & 15) >> c.bits;
            const unsigned remainder = p.jam & ((1U << c.bits) - 1);
            assert(ones <= 9 && tens <= 9 && hundreds <= 9);
            assert(remainder < static_cast<unsigned>(c.mode));
            assert(static_cast<unsigned>(c.mode) * (1000 * thousands + 100 * hundreds + 10 * tens + ones) + remainder == n);
            assert(p.ka == c.ka && p.kb == c.kb && p.kc == c.kc);
        }
        Divider::Program sentinel{0xBEEF, true, true, true};
        assert(!Divider::Encode(2, c.mode, sentinel));
        assert(!Divider::Encode(c.max + 1, c.mode, sentinel));
        assert(sentinel.jam == 0xBEEF && sentinel.ka && sentinel.kb && sentinel.kc);
    }
    Divider::Program p;
    // TI's published worked example N=8479 in /5 mode.
    assert(Divider::Encode(8479, Divider::Mode::DivideBy5, p) && p.jam == 0x695C);
    assert(!Divider::Encode(100, static_cast<Divider::Mode>(3), p));

    std::array<Pin, 16> pins;
    std::array<IOPIN*, 16> refs;
    for (size_t i = 0; i < refs.size(); ++i) refs[i] = &pins[i];
    Pin ka, kb, kc, le;
    std::vector<uint16_t> snapshots;
    Divider divider(refs, ka, kb, kc, le, [&](uint32_t periods) {
        assert(periods >= 4 && !kb.value && !kc.value);
        uint16_t word = 0;
        for (size_t i = 0; i < pins.size(); ++i) if (pins[i].value) word |= 1U << i;
        snapshots.push_back(word);
    });
    assert(divider.Configure(8479, Divider::Mode::DivideBy5));
    assert(snapshots.size() == 2 && snapshots[1] == 0x695C);
    assert(ka.value && !kb.value && kc.value && !le.value);
    unsigned writes = kb.writes;
    assert(!divider.Configure(0) && kb.writes == writes && snapshots.size() == 2);
    divider.SetLatchEnabled(true);
    assert(le.value);
    assert(divider.HoldInPreset() && !kb.value && !kc.value);
    assert(divider.Configure(1600));
    assert(!ka.value && !kb.value && kc.value && !le.value);
    assert(snapshots.back() == 0x2000);
    refs[0] = nullptr;
    Divider missingPin(refs, ka, kb, kc, le, [](uint32_t) {});
    writes = kb.writes;
    assert(!missingPin.Configure(100) && !missingPin.HoldInPreset() && kb.writes == writes);
    refs[0] = &pins[0];
    Divider missingWait(refs, ka, kb, kc, le, {});
    assert(!missingWait.Configure(100) && !missingWait.HoldInPreset() && kb.writes == writes);
    std::cout << "CD74HC4059 tests passed\n";
}
