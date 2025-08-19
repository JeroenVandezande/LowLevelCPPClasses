#pragma once
#include "LLE_Analog.h"
#include "LLE_PWM.h"

namespace LowLevelEmbedded
{
    template <std::unsigned_integral ValueT>
    class PWM_DAC : public LowLevelEmbedded::IDAC<ValueT>
    {
    private:
        IPWMController<ValueT>* pwmController_;
        float maxVoltage_;
        ValueT maxDAValue_;
        float voltagePerBit_;

    public:
        PWM_DAC(IPWMController<ValueT>* pwmController, float maxVoltage)
        {
            pwmController_ = pwmController;
            maxVoltage_ = maxVoltage;
            maxDAValue_ = pwmController_->GetMaxPWMValue();
            voltagePerBit_ = maxVoltage_ / maxDAValue_;
        }

        bool WriteDAC(uint8_t channel, ValueT value) override;
        ValueT GetMaxDAValue() override;
        uint8_t GetMaxChannels() override;
        bool WriteDACVoltage(uint8_t channel, float value) override;
        IDACChannel<unsigned short>* CreateChannelObject(uint8_t channel) override;
    };
}
