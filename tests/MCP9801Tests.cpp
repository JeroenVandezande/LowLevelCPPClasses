#include "MCP9801.h"
#include "Delay.h"
#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>

using namespace LowLevelEmbedded;
using namespace LowLevelEmbedded::Devices::Sensors;
using unitsnet_cpp::Temperature;

/// Register-level model of the chip: TA read-only, CONFIG 8-bit, THYST/TSET
/// 16-bit with only the top nine bits implemented.
struct I2C : II2CAccess
{
    uint8_t expectedAddress = 0x90;
    std::array<uint16_t, 4> regs{0x0000, 0x0000, 0x4B00, 0x5000};
    bool fail = false;
    unsigned configReadsUntilOneShotClears = 0;
    unsigned reads = 0, writes = 0;
    std::vector<uint8_t> lastWrite;

    bool I2C_ReadMethod(uint8_t, uint8_t*, size_t) override { assert(false); return false; }
    bool I2C_WriteMethod(uint8_t, uint8_t*, size_t) override { assert(false); return false; }
    bool I2C_ReadWriteMethod(uint8_t, uint8_t*, size_t, size_t) override { assert(false); return false; }
    bool I2C_IsDeviceReady(uint8_t address) override { return address == expectedAddress && !fail; }

    bool I2C_Mem_Read(uint8_t address, uint8_t reg, uint8_t size, uint8_t* data, size_t length) override
    {
        assert(address == expectedAddress && size == 1 && reg <= 3);
        if (fail) return false;
        ++reads;
        if (reg == 1)
        {
            assert(length == 1);
            if (regs[1] & 0x80)
            {
                if (configReadsUntilOneShotClears == 0) regs[1] &= 0x7F;
                else --configReadsUntilOneShotClears;
            }
            data[0] = static_cast<uint8_t>(regs[1]);
        }
        else
        {
            assert(length == 2);
            data[0] = static_cast<uint8_t>(regs[reg] >> 8);
            data[1] = static_cast<uint8_t>(regs[reg]);
        }
        return true;
    }

    bool I2C_Mem_Write(uint8_t address, uint8_t reg, uint8_t size, uint8_t* data, size_t length) override
    {
        assert(address == expectedAddress && size == 1 && reg >= 1 && reg <= 3);
        if (fail) return false;
        ++writes;
        lastWrite.assign(data, data + length);
        if (reg == 1)
        {
            assert(length == 1);
            regs[1] = data[0];
        }
        else
        {
            assert(length == 2);
            regs[reg] = static_cast<uint16_t>((data[0] << 8 | data[1]) & 0xFF80);
        }
        return true;
    }
};

static bool Near(float a, float b) { return a - b < 1e-4f && b - a < 1e-4f; }

int main()
{
    unsigned delayedMs = 0;
    Utility::Delay_ms = [&](uint32_t ms) { delayedMs += ms; };

    // Datasheet worked examples and format corners.
    assert(Near(MCP9801::RawToCelsius(0x1940), 25.25f));   // Figure 5-3
    assert(Near(MCP9801::RawToCelsius(0x5000), 80.0f));    // TSET default
    assert(Near(MCP9801::RawToCelsius(0x4B00), 75.0f));    // THYST default
    assert(Near(MCP9801::RawToCelsius(0x7FF0), 127.9375f));
    assert(Near(MCP9801::RawToCelsius(0xFF80), -0.5f));
    assert(Near(MCP9801::RawToCelsius(0xE700), -25.0f));
    assert(Near(MCP9801::RawToCelsius(0x8000), -128.0f));
    assert(MCP9801::CelsiusToLimitRaw(95.0f) == 0x5F00);   // Section 5.3.3 example
    assert(MCP9801::CelsiusToLimitRaw(80.0f) == 0x5000);
    assert(MCP9801::CelsiusToLimitRaw(75.5f) == 0x4B80);
    assert(MCP9801::CelsiusToLimitRaw(75.3f) == 0x4B80);   // rounds to nearest 0.5
    assert(MCP9801::CelsiusToLimitRaw(75.2f) == 0x4B00);
    assert(MCP9801::CelsiusToLimitRaw(-0.5f) == 0xFF80);
    assert(MCP9801::CelsiusToLimitRaw(-25.0f) == 0xE700);
    assert(MCP9801::CelsiusToLimitRaw(200.0f) == 0x7F80);  // clamps to +127.5
    assert(MCP9801::CelsiusToLimitRaw(-200.0f) == 0x8000); // clamps to -128
    for (int halves = -256; halves <= 255; ++halves)
    {
        const float celsius = halves / 2.0f;
        assert(Near(MCP9801::RawToCelsius(MCP9801::CelsiusToLimitRaw(celsius)), celsius));
    }

    // CONFIG encoding, Section 5.3.2 example: 12-bit resolution -> 0x60.
    MCP9801::Configuration config;
    assert(MCP9801::EncodeConfiguration(config) == 0x60);
    config.resolution = MCP9801::Resolution::Bits9;
    assert(MCP9801::EncodeConfiguration(config) == 0x00); // power-up default
    config.resolution = MCP9801::Resolution::Bits11;
    config.faultQueue = MCP9801::FaultQueue::Faults6;
    config.alertPolarity = MCP9801::AlertPolarity::ActiveHigh;
    config.alertMode = MCP9801::AlertMode::Interrupt;
    config.shutdown = true;
    assert(MCP9801::EncodeConfiguration(config) == 0x5F);
    for (unsigned value = 0; value < 0x80; ++value)
        assert(MCP9801::EncodeConfiguration(MCP9801::DecodeConfiguration(static_cast<uint8_t>(value))) == value);
    assert(MCP9801::MaxConversionTimeMs(MCP9801::Resolution::Bits9) == 75);
    assert(MCP9801::MaxConversionTimeMs(MCP9801::Resolution::Bits12) == 600);

    assert(MCP9801::DEFAULT_I2C_ADDRESS == 0x90);
    assert(MCP9801::AddressFromPins(false, false, false) == 0x90);
    assert(MCP9801::AddressFromPins(false, false, true) == 0x92);
    assert(MCP9801::AddressFromPins(true, true, true) == 0x9E);

    I2C i2c;
    MCP9801 sensor(&i2c);

    // Initialize writes CONFIG and verifies the read-back.
    assert(sensor.Initialize());
    assert(i2c.regs[1] == 0x60);
    MCP9801::Configuration readback;
    assert(sensor.ReadConfiguration(readback));
    assert(readback.resolution == MCP9801::Resolution::Bits12 && !readback.shutdown);

    // Continuous-mode reads just return TA.
    i2c.regs[0] = 0x1940;
    Temperature t = Temperature::from_degrees_celsius(0.0f);
    assert(sensor.ReadTemperature(t) && Near(t.degrees_celsius(), 25.25f));
    assert(Near(sensor.GetTemperature().degrees_celsius(), 25.25f));
    i2c.regs[0] = 0xE700;
    assert(Near(sensor.GetTemperature().degrees_celsius(), -25.0f));

    // Limits are written MSB first with the 9-bit format and read back.
    assert(sensor.SetLimit(Temperature::from_degrees_celsius(95.0f)));
    assert(i2c.lastWrite.size() == 2 && i2c.lastWrite[0] == 0x5F && i2c.lastWrite[1] == 0x00);
    assert(sensor.SetHysteresis(Temperature::from_degrees_celsius(-10.5f)));
    assert(i2c.lastWrite[0] == 0xF5 && i2c.lastWrite[1] == 0x80);
    assert(sensor.GetLimit(t) && Near(t.degrees_celsius(), 95.0f));
    assert(sensor.GetHysteresis(t) && Near(t.degrees_celsius(), -10.5f));

    // SetShutdown preserves the other fields.
    assert(sensor.SetShutdown(true) && i2c.regs[1] == 0x61);
    assert(sensor.SetShutdown(false) && i2c.regs[1] == 0x60);

    // One-shot from continuous mode: shutdown first, then shutdown + one-shot,
    // poll until the device clears bit 7, read TA, remain in shutdown.
    i2c.regs[0] = 0x1900;
    i2c.configReadsUntilOneShotClears = 3;
    delayedMs = 0;
    unsigned writes = i2c.writes;
    assert(sensor.ReadTemperatureOneShot(t) && Near(t.degrees_celsius(), 25.0f));
    assert(i2c.writes == writes + 2);
    assert(i2c.regs[1] == 0x61);
    assert(delayedMs == 20);
    // Already in shutdown: only the one-shot write is issued.
    i2c.configReadsUntilOneShotClears = 0;
    writes = i2c.writes;
    assert(sensor.ReadTemperatureOneShot(t));
    assert(i2c.writes == writes + 1 && i2c.lastWrite[0] == 0xE1);
    // Device never clears ONE-SHOT: bounded by the maximum conversion time.
    i2c.configReadsUntilOneShotClears = 1000;
    delayedMs = 0;
    assert(!sensor.ReadTemperatureOneShot(t));
    assert(delayedMs == 600);
    i2c.regs[1] = 0x01; // 9-bit, shutdown
    i2c.configReadsUntilOneShotClears = 1000;
    delayedMs = 0;
    assert(!sensor.ReadTemperatureOneShot(t));
    assert(delayedMs == 75);
    assert(sensor.SetShutdown(false));

    // I2C failures propagate; GetTemperature returns 0 °C.
    i2c.fail = true;
    assert(!sensor.Initialize());
    assert(!sensor.ReadTemperature(t));
    assert(!sensor.ReadTemperatureOneShot(t));
    assert(!sensor.SetLimit(Temperature::from_degrees_celsius(1.0f)));
    assert(!sensor.GetLimit(t) && !sensor.GetHysteresis(t));
    assert(!sensor.SetShutdown(true) && !sensor.ReadConfiguration(readback));
    assert(Near(sensor.GetTemperature().degrees_celsius(), 0.0f));
    i2c.fail = false;

    // Initialize fails when the read-back does not match (wrong or absent device).
    struct Stuck : I2C
    {
        bool I2C_Mem_Write(uint8_t address, uint8_t reg, uint8_t size, uint8_t* data, size_t length) override
        {
            I2C::I2C_Mem_Write(address, reg, size, data, length);
            regs[1] = 0x00;
            return true;
        }
    } stuck;
    MCP9801 absent(&stuck);
    assert(!absent.Initialize());

    // Alternate address is forwarded to the bus.
    I2C strapped;
    strapped.expectedAddress = 0x9E;
    MCP9801 other(&strapped, MCP9801::AddressFromPins(true, true, true));
    assert(other.Initialize());

    std::cout << "MCP9801 tests passed\n";
}
