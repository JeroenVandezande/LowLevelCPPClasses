#include "PWM_DAC.h"

template <std::unsigned_integral ValueT>
bool LowLevelEmbedded::PWM_DAC<ValueT>::WriteDAC(uint8_t channel, ValueT value)
{
    pwmController_->WritePWM(channel, value);
    return true;
}

template <std::unsigned_integral ValueT>
ValueT LowLevelEmbedded::PWM_DAC<ValueT>::GetMaxDAValue()
{
    return maxDAValue_;
}

template <std::unsigned_integral ValueT>
uint8_t LowLevelEmbedded::PWM_DAC<ValueT>::GetMaxChannels()
{
    return pwmController_->GetMaxChannels();
}

template <std::unsigned_integral ValueT>
bool LowLevelEmbedded::PWM_DAC<ValueT>::WriteDACVoltage(
    uint8_t channel,
    unitsnet_cpp::ElectricPotential value)
{
    const auto ratio = value.volts() / maxVoltage_.volts();
    if (ratio < 0.0f || ratio > 1.0f)
    {
        return false;
    }
    return WriteDAC(channel, static_cast<ValueT>(ratio * maxDAValue_));
}

template <std::unsigned_integral ValueT>
LowLevelEmbedded::IDACChannel<ValueT>*
LowLevelEmbedded::PWM_DAC<ValueT>::CreateChannelObject(uint8_t channel)
{
    return new PWMChannel_base<ValueT>(this, channel);
}
