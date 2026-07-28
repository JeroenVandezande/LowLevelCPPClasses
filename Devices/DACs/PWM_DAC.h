#pragma once
#include "LLE_DAC.h"
#include "LLE_PWM.h"

namespace LowLevelEmbedded
{
    template <std::unsigned_integral ValueT>
    class PWM_DAC : public LowLevelEmbedded::IDAC<ValueT>
    {
    private:
        IPWMController<ValueT>* pwmController_;
        unitsnet_cpp::ElectricPotential maxVoltage_;
        ValueT maxDAValue_;

    public:
        PWM_DAC(
            IPWMController<ValueT>* pwmController,
            unitsnet_cpp::ElectricPotential maxVoltage)
        {
            pwmController_ = pwmController;
            maxVoltage_ = maxVoltage;
            maxDAValue_ = pwmController_->GetMaxPWMValue();
        }

        bool WriteDAC(uint8_t channel, ValueT value) override;
        ValueT GetMaxDAValue() override;
        uint8_t GetMaxChannels() override;
        bool WriteDACVoltage(
            uint8_t channel,
            unitsnet_cpp::ElectricPotential value) override;
        IDACChannel<ValueT>* CreateChannelObject(uint8_t channel) override;
    };
}
