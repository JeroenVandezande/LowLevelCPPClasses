#pragma once

#include <cstdint>
#include <concepts>

namespace LowLevelEmbedded
{
    class ISimpleADCChannel
    {
     public:
        virtual ~ISimpleADCChannel()
        {
        };

        virtual float ReadVoltage() = 0;
    };

    template <std::unsigned_integral ValueT>
    class IADCChannel : public ISimpleADCChannel
    {
    public:
        virtual ~IADCChannel()
        {
        };

        virtual ValueT ReadADC() = 0;
        virtual ValueT GetMaxADCValue() = 0;
    };

    template <std::unsigned_integral ValueT>
    class IADC
    {
    public:
        virtual ~IADC()
        {
        };

        virtual ValueT ReadADC(uint8_t channel) = 0;
        virtual ValueT GetMaxADCValue() = 0;
        virtual uint8_t GetMaxChannels() = 0;
        virtual float ReadVoltage(uint8_t channel) = 0;
        virtual IADCChannel<ValueT>* CreateChannelObject(uint8_t channel) = 0;
    };

    template <std::unsigned_integral ValueT>
    class ADCChannel_base : public IADCChannel<ValueT>
    {
    private:
        IADC<ValueT>* parent_;
        uint8_t channel_;

    public:
        ADCChannel_base(IADC<ValueT>* parent, const uint8_t channel)
        {
            parent_ = parent;
            channel_ = channel;
        }

        float ReadVoltage() override
        {
            return parent_->ReadVoltage(channel_);
        }

        ValueT ReadADC() override
        {
            return parent_->ReadADC(channel_);
        }

        ValueT GetMaxADCValue() override
        {
            return parent_->GetMaxADCValue();
        }
    };
}
