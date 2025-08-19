#pragma once
#include <cstdint>
#include <concepts>

namespace LowLevelEmbedded
{
    // Simple channel interface: percentage-only control
    class ISimplePWMChannel
    {
    public:
        virtual ~ISimplePWMChannel() = default;
        virtual bool SetPWMPercentage(float value) = 0; // 0.0 .. 1.0
    };

    // Full channel interface (templated on the raw counter type, e.g., uint16_t)
    template <std::unsigned_integral ValueT>
    class IPWMChannel : public ISimplePWMChannel
    {
    public:
        virtual ~IPWMChannel() = default;

        // Write raw timer value
        virtual bool WritePWM(ValueT value) = 0;

        // Query capabilities
        virtual ValueT GetMaxPWMValue() = 0;
    };

    // Controller interface with per-channel operations (mirrors the IDAC design)
    template <std::unsigned_integral ValueT>
    class IPWMController
    {
    public:
        virtual ~IPWMController() = default;

        // Per-channel writes
        virtual bool WritePWM(uint8_t channel, ValueT value) = 0;
        virtual bool SetPWMPercentage(uint8_t channel, float value) = 0; // 0.0 .. 1.0

        // Capabilities
        virtual ValueT GetMaxPWMValue() = 0;
        virtual uint8_t GetMaxChannels() = 0;

        // Channel factory
        virtual IPWMChannel<ValueT>* CreateChannelObject(uint8_t channel) = 0;
        // Consider std::unique_ptr<IPWMChannel<ValueT>> for ownership safety.
    };

    // Helper base channel that forwards calls to a parent IPWM
    template <std::unsigned_integral ValueT>
    class PWMChannel_base : public IPWMChannel<ValueT>
    {
    private:
        IPWMController<ValueT>* parent_;
        uint8_t channel_;

    public:
        PWMChannel_base(IPWMController<ValueT>* parent, uint8_t channel)
            : parent_(parent), channel_(channel) {}

        bool SetPWMPercentage(float value) override
        {
            return parent_->SetPWMPercentage(channel_, value);
        }

        bool WritePWM(ValueT value) override
        {
            return parent_->WritePWM(channel_, value);
        }

        ValueT GetMaxPWMValue() override
        {
            return parent_->GetMaxPWMValue();
        }
    };
} // namespace LowLevelEmbedded
