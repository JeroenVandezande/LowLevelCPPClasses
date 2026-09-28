#include "DDC114.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace LowLevelEmbedded;
using namespace LowLevelEmbedded::Devices::ADCs;

struct Pin : IOPIN
{
    bool value = false;
    void Set() override { value = true; }
    void Clear() override { value = false; }
    bool GetValue() override { return value; }
    void Toggle() override { value = !value; }
};

struct SPI : ISPIAccess
{
    Pin& valid;
    std::vector<uint8_t> response;
    size_t calls = 0;
    explicit SPI(Pin& valid) : valid(valid) {}
    void WriteSPI(uint8_t*, size_t, uint8_t, SPIMode) override { assert(false); }
    void WriteThenReadSPI(uint8_t*, size_t, uint8_t*, size_t, uint8_t, SPIMode) override { assert(false); }
    void ReadWriteSPI(uint8_t* data, size_t length, uint8_t id, SPIMode mode) override
    {
        assert(id == 7 && mode == SPIMode::Mode0);
        assert(length == response.size());
        assert(std::all_of(data, data + length, [](uint8_t b) { return b == 0; }));
        std::copy(response.begin(), response.end(), data);
        valid.Set(); // DVALID deasserts when data is clocked out.
        ++calls;
    }
};

int main()
{
    Pin valid, r0, r1, r2, format, test;
    SPI spi(valid);
    DDC114 adc(spi, valid, {&r0, &r1, &r2, &format, &test}, 7);
    DDC114::Samples samples;
    samples.channels.fill(42);
    assert(adc.TryRead(samples) == DDC114::ReadResult::NotInitialized);
    assert(!adc.IsDataReady() && spi.calls == 0);
    test.Set();
    assert(adc.Initialize(DDC114::Range::PicoCoulombs250));
    assert(r0.value && !r1.value && r2.value && format.value && !test.value);
    valid.Set();
    assert(adc.TryRead(samples) == DDC114::ReadResult::NotReady && spi.calls == 0);
    // AIN4=ABCDE, AIN3=12345, AIN2=00000, AIN1=FFFFF.
    spi.response = {0xAB, 0xCD, 0xE1, 0x23, 0x45, 0x00, 0x00, 0x0F, 0xFF, 0xFF};
    for (int i = 0; i < 4; ++i)
    {
        valid.Clear();
        assert(adc.TryRead(samples) == DDC114::ReadResult::Settling);
        assert(samples.channels[0] == 42);
    }
    valid.Clear();
    assert(adc.TryRead(samples) == DDC114::ReadResult::Ready);
    assert((samples.channels == std::array<uint32_t, 4>{0xFFFFF, 0, 0x12345, 0xABCDE}));
    assert(samples.format == DDC114::Format::Bits20);
    assert(!adc.Initialize(static_cast<DDC114::Range>(8)));
    assert(!adc.Initialize(DDC114::Range::PicoCoulombs12, static_cast<DDC114::Format>(2)));
    assert(r0.value && !r1.value && r2.value && format.value);
    for (uint8_t range = 0; range < 8; ++range)
    {
        assert(adc.Initialize(static_cast<DDC114::Range>(range), DDC114::Format::Bits16));
        assert(r0.value == bool(range & 1));
        assert(r1.value == bool(range & 2));
        assert(r2.value == bool(range & 4));
    }
    assert(!format.value);
    spi.response = {0xAB, 0xCD, 0x12, 0x34, 0x01, 0x00, 0xFF, 0xFF};
    for (int i = 0; i < 4; ++i)
    {
        valid.Clear();
        assert(adc.TryRead(samples) == DDC114::ReadResult::Settling);
    }
    valid.Clear();
    assert(adc.TryRead(samples) == DDC114::ReadResult::Ready);
    assert((samples.channels == std::array<uint32_t, 4>{0xFFFF, 0x100, 0x1234, 0xABCD}));
    assert(samples.format == DDC114::Format::Bits16);
    DDC114 strapped(spi, valid, {}, 7);
    assert(strapped.Initialize(DDC114::Range::PicoCoulombs50));
    std::cout << "DDC114 tests passed\n";
}
