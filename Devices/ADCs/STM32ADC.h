#pragma once

// Your project interface(s)
#include "LLE_ADC.h"

#include <cstdint>
#include <concepts>
#include <type_traits>

// --- Auto-include the right STM32 HAL ADC header --------------------------------
#if defined(__has_include)

  #if   __has_include("stm32f0xx_hal_adc.h")
    #include "stm32f0xx_hal_adc.h"
    #if __has_include("stm32f0xx_hal_adc_ex.h")
      #include "stm32f0xx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32f1xx_hal_adc.h")
    #include "stm32f1xx_hal_adc.h"
    #if __has_include("stm32f1xx_hal_adc_ex.h")
      #include "stm32f1xx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32f2xx_hal_adc.h")
    #include "stm32f2xx_hal_adc.h"
    #if __has_include("stm32f2xx_hal_adc_ex.h")
      #include "stm32f2xx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32f3xx_hal_adc.h")
    #include "stm32f3xx_hal_adc.h"
    #if __has_include("stm32f3xx_hal_adc_ex.h")
      #include "stm32f3xx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32f4xx_hal_adc.h")
    #include "stm32f4xx_hal_adc.h"
    #if __has_include("stm32f4xx_hal_adc_ex.h")
      #include "stm32f4xx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32f7xx_hal_adc.h")
    #include "stm32f7xx_hal_adc.h"
    #if __has_include("stm32f7xx_hal_adc_ex.h")
      #include "stm32f7xx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32g0xx_hal_adc.h")
    #include "stm32g0xx_hal_adc.h"
    #if __has_include("stm32g0xx_hal_adc_ex.h")
      #include "stm32g0xx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32g4xx_hal_adc.h")
    #include "stm32g4xx_hal_adc.h"
    #if __has_include("stm32g4xx_hal_adc_ex.h")
      #include "stm32g4xx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32h7xx_hal_adc.h")
    #include "stm32h7xx_hal_adc.h"
    #if __has_include("stm32h7xx_hal_adc_ex.h")
      #include "stm32h7xx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32l0xx_hal_adc.h")
    #include "stm32l0xx_hal_adc.h"
    #if __has_include("stm32l0xx_hal_adc_ex.h")
      #include "stm32l0xx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32l1xx_hal_adc.h")
    #include "stm32l1xx_hal_adc.h"
    #if __has_include("stm32l1xx_hal_adc_ex.h")
      #include "stm32l1xx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32l4xx_hal_adc.h")
    #include "stm32l4xx_hal_adc.h"
    #if __has_include("stm32l4xx_hal_adc_ex.h")
      #include "stm32l4xx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32l5xx_hal_adc.h")
    #include "stm32l5xx_hal_adc.h"
    #if __has_include("stm32l5xx_hal_adc_ex.h")
      #include "stm32l5xx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32u5xx_hal_adc.h")
    #include "stm32u5xx_hal_adc.h"
    #if __has_include("stm32u5xx_hal_adc_ex.h")
      #include "stm32u5xx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32wbxx_hal_adc.h")
    #include "stm32wbxx_hal_adc.h"
    #if __has_include("stm32wbxx_hal_adc_ex.h")
      #include "stm32wbxx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32wlxx_hal_adc.h")
    #include "stm32wlxx_hal_adc.h"
    #if __has_include("stm32wlxx_hal_adc_ex.h")
      #include "stm32wlxx_hal_adc_ex.h"
    #endif
  #elif __has_include("stm32mp1xx_hal_adc.h")
    #include "stm32mp1xx_hal_adc.h"
    #if __has_include("stm32mp1xx_hal_adc_ex.h")
      #include "stm32mp1xx_hal_adc_ex.h"
    #endif
  #else
    #error "No stm32*xx_hal_adc.h found in include paths. Add the proper Cube HAL include directories."
  #endif

#else
  #error "__has_include not supported by this compiler. Include the correct stm32*xx_hal_adc.h before this header."
#endif
// --------------------------------------------------------------------------------

// Feature flag: families whose ADC channel config has .SingleDiff
#if defined(STM32G4xx) || defined(STM32G0xx) || defined(STM32H7xx) || defined(STM32L4xx) || defined(STM32U5xx)
  #define LLE_ADC_HAS_SINGLE_DIFF 1
#endif

namespace LowLevelEmbedded
{
namespace STM32HAL
{
    // Map HAL resolution enum to bit count across families.
    inline uint8_t STM32_HAL_ResolutionBits(uint32_t hal_res_enum)
    {
    #ifdef ADC_RESOLUTION_12B
        if (hal_res_enum == ADC_RESOLUTION_12B) return 12;
    #endif
    #ifdef ADC_RESOLUTION_10B
        if (hal_res_enum == ADC_RESOLUTION_10B) return 10;
    #endif
    #ifdef ADC_RESOLUTION_8B
        if (hal_res_enum == ADC_RESOLUTION_8B) return 8;
    #endif
    #ifdef ADC_RESOLUTION_6B
        if (hal_res_enum == ADC_RESOLUTION_6B) return 6;
    #endif
        // Optional LL fallbacks (some older HALs piggyback LL):
    #ifdef LL_ADC_RESOLUTION_12B
        if (hal_res_enum == LL_ADC_RESOLUTION_12B) return 12;
    #endif
    #ifdef LL_ADC_RESOLUTION_10B
        if (hal_res_enum == LL_ADC_RESOLUTION_10B) return 10;
    #endif
    #ifdef LL_ADC_RESOLUTION_8B
        if (hal_res_enum == LL_ADC_RESOLUTION_8B) return 8;
    #endif
    #ifdef LL_ADC_RESOLUTION_6B
        if (hal_res_enum == LL_ADC_RESOLUTION_6B) return 6;
    #endif
        // Safe default
        return 12;
    }

    // Simple channel index (0..18) -> ADC_CHANNEL_X macro.
    // Extend with helpers for VBAT/VREFINT/TEMPSENSOR as desired.
    inline constexpr uint32_t STM32_ADC_ChannelMacro(uint8_t ch)
    {
        switch (ch)
        {
        #ifdef ADC_CHANNEL_0
            case 0:  return ADC_CHANNEL_0;
            case 1:  return ADC_CHANNEL_1;
            case 2:  return ADC_CHANNEL_2;
            case 3:  return ADC_CHANNEL_3;
            case 4:  return ADC_CHANNEL_4;
            case 5:  return ADC_CHANNEL_5;
            case 6:  return ADC_CHANNEL_6;
            case 7:  return ADC_CHANNEL_7;
            case 8:  return ADC_CHANNEL_8;
            case 9:  return ADC_CHANNEL_9;
            case 10: return ADC_CHANNEL_10;
            case 11: return ADC_CHANNEL_11;
            case 12: return ADC_CHANNEL_12;
            case 13: return ADC_CHANNEL_13;
            case 14: return ADC_CHANNEL_14;
            case 15: return ADC_CHANNEL_15;
        #endif
        #ifdef ADC_CHANNEL_16
            case 16: return ADC_CHANNEL_16;
        #endif
        #ifdef ADC_CHANNEL_17
            case 17: return ADC_CHANNEL_17;
        #endif
        #ifdef ADC_CHANNEL_18
            case 18: return ADC_CHANNEL_18;
        #endif
            default: return 0xFFFFFFFFu; // invalid
        }
    }

    // Family-agnostic default sample-time picker (used if user passes 0)
    inline constexpr uint32_t PickDefaultSampleTime()
    {
    #ifdef ADC_SAMPLETIME_3CYCLES
        return ADC_SAMPLETIME_3CYCLES;
    #elif defined(ADC_SAMPLETIME_2CYCLES_5)
        return ADC_SAMPLETIME_2CYCLES_5;
    #elif defined(ADC_SAMPLETIME_4CYCLES_5)
        return ADC_SAMPLETIME_4CYCLES_5;
    #elif defined(ADC_SAMPLETIME_12CYCLES_5)
        return ADC_SAMPLETIME_12CYCLES_5;
    #elif defined(ADC_SAMPLETIME_15CYCLES)
        return ADC_SAMPLETIME_15CYCLES;
    #else
        return 0; // let HAL validate
    #endif
    }

} // namespace STM32HAL


// A concrete channel type that uses your fixed base class
template <std::unsigned_integral ValueT>
class STM32ADCChannel : public ADCChannel_base<ValueT>
{
public:
    STM32ADCChannel(IADC<ValueT>* parent, uint8_t channel)
        : ADCChannel_base<ValueT>(parent, channel)
    {}
};

// Generic, cross-family STM32 HAL ADC wrapper
template <std::unsigned_integral ValueT>
class STM32ADC : public IADC<ValueT>
{
    static_assert(std::is_unsigned_v<ValueT>, "ValueT must be an unsigned integer type");

public:
    explicit STM32ADC(ADC_HandleTypeDef* hadc,
                      float vref_volts = 3.3f,
                      uint8_t max_channels = 16,
                      uint32_t default_sample_time =
    #ifdef HAL_ADC_MODULE_ENABLED
        #ifdef ADC_SAMPLETIME_3CYCLES
                          ADC_SAMPLETIME_3CYCLES
        #elif defined(ADC_SAMPLETIME_2CYCLES_5)
                          ADC_SAMPLETIME_2CYCLES_5
        #else
                          0
        #endif
    #else
                          0
    #endif
                      )
        : hadc_(hadc),
          vref_(vref_volts),
          max_channels_(max_channels),
          default_sample_time_(default_sample_time)
    {
        bits_ = STM32HAL::STM32_HAL_ResolutionBits(hadc_->Init.Resolution);
        max_adc_value_ = (bits_ >= 16)
            ? static_cast<ValueT>(0xFFFFu)
            : static_cast<ValueT>((static_cast<uint32_t>(1u) << bits_) - 1u);
    }

    ValueT ReadADC(uint8_t channel) override
    {
        const uint32_t hal_ch = STM32HAL::STM32_ADC_ChannelMacro(channel);
        if (hal_ch == 0xFFFFFFFFu)
            return 0; // invalid channel

        if (ensure_calibrated_() != HAL_OK)
            return 0;

        ADC_ChannelConfTypeDef sConfig{};
        sConfig.Channel = hal_ch;

    #ifdef LLE_ADC_HAS_SINGLE_DIFF
        sConfig.SingleDiff = ADC_SINGLE_ENDED; // no-op on families without the field
    #endif

    #if defined(ADC_REGULAR_RANK_1)
        sConfig.Rank = ADC_REGULAR_RANK_1;
    #else
        sConfig.Rank = 1u;
    #endif

        sConfig.SamplingTime = (default_sample_time_ != 0)
            ? default_sample_time_
            : STM32HAL::PickDefaultSampleTime();

    #if defined(HAL_ADC_MODULE_ENABLED)
        if (HAL_ADC_ConfigChannel(hadc_, &sConfig) != HAL_OK) return 0;
        if (HAL_ADC_Start(hadc_) != HAL_OK)                   return 0;

        if (HAL_ADC_PollForConversion(hadc_, poll_timeout_ms_) != HAL_OK) {
            (void)HAL_ADC_Stop(hadc_);
            return 0;
        }

        uint32_t raw = HAL_ADC_GetValue(hadc_);
        (void)HAL_ADC_Stop(hadc_);

        if (raw > static_cast<uint32_t>(max_adc_value_))
            raw = static_cast<uint32_t>(max_adc_value_);

        return static_cast<ValueT>(raw);
    #else
        return 0;
    #endif
    }

    ValueT GetMaxADCValue() override { return max_adc_value_; }
    uint8_t GetMaxChannels() override { return max_channels_; }

    float ReadVoltage(uint8_t channel) override
    {
        const ValueT raw = ReadADC(channel);
        return (static_cast<float>(raw) * vref_) / static_cast<float>(max_adc_value_);
    }

    IADCChannel<ValueT>* CreateChannelObject(uint8_t channel) override
    {
        return new STM32ADCChannel<ValueT>(this, channel);
    }

    // --- Extras --------------------------------------------------------------
    void SetVref(float vref_volts) { vref_ = vref_volts; }
    [[nodiscard]] float GetVref() const { return vref_; }
    [[nodiscard]] uint8_t GetResolutionBits() const { return bits_; }

    void SetPollTimeout(uint32_t ms) { poll_timeout_ms_ = ms; }
    [[nodiscard]] uint32_t GetPollTimeout() const { return poll_timeout_ms_; }

    void SetDefaultSampleTime(uint32_t st) { default_sample_time_ = st; }
    [[nodiscard]] uint32_t GetDefaultSampleTime() const { return default_sample_time_; }

private:
    // Some families (G0/G4/L4/H7/U5…) prefer a calibration before first use
    HAL_StatusTypeDef ensure_calibrated_()
    {
    #if defined(HAL_ADC_MODULE_ENABLED) && \
        (defined(STM32G4xx) || defined(STM32G0xx) || defined(STM32L4xx) || defined(STM32H7xx) || defined(STM32U5xx))
        if (!calibrated_) {
            (void)HAL_ADC_Stop(hadc_);
            #ifdef LLE_ADC_HAS_SINGLE_DIFF
              if (HAL_ADCEx_Calibration_Start(hadc_, ADC_SINGLE_ENDED) != HAL_OK) return HAL_ERROR;
            #else
              if (HAL_ADCEx_Calibration_Start(hadc_) != HAL_OK) return HAL_ERROR; // some families omit the mode arg
            #endif
            calibrated_ = true;
        }
    #endif
        return HAL_OK;
    }

private:
    ADC_HandleTypeDef* hadc_;
    float              vref_;
    uint8_t            max_channels_;
    uint32_t           default_sample_time_;
    uint8_t            bits_;
    ValueT             max_adc_value_;
    uint32_t           poll_timeout_ms_ = 10;
    bool               calibrated_ = false;
};

} // namespace LowLevelEmbedded
