#pragma once
#include "LLE_IOPIN.h"
#include <cstdint>

#if defined(__has_include)

#if __has_include("stm32f0xx_ll_gpio.h")
#include "stm32f0xx_ll_gpio.h"

#elif __has_include("stm32f1xx_ll_gpio.h")
#include "stm32f1xx_ll_gpio.h"

#elif __has_include("stm32f2xx_ll_gpio.h")
#include "stm32f2xx_ll_gpio.h"

#elif __has_include("stm32f3xx_ll_gpio.h")
#include "stm32f3xx_ll_gpio.h"

#elif __has_include("stm32f4xx_ll_gpio.h")
#include "stm32f4xx_ll_gpio.h"

#elif __has_include("stm32f7xx_ll_gpio.h")
#include "stm32f7xx_ll_gpio.h"

#elif __has_include("stm32g0xx_ll_gpio.h")
#include "stm32g0xx_ll_gpio.h"

#elif __has_include("stm32g4xx_ll_gpio.h")
#include "stm32g4xx_ll_gpio.h"

#elif __has_include("stm32h7xx_ll_gpio.h")
#include "stm32h7xx_ll_gpio.h"

#elif __has_include("stm32l0xx_ll_gpio.h")
#include "stm32l0xx_ll_gpio.h"

#elif __has_include("stm32l1xx_ll_gpio.h")
#include "stm32l1xx_ll_gpio.h"

#elif __has_include("stm32l4xx_ll_gpio.h")
#include "stm32l4xx_ll_gpio.h"

#elif __has_include("stm32l5xx_ll_gpio.h")
#include "stm32l5xx_ll_gpio.h"

#elif __has_include("stm32u5xx_ll_gpio.h")
#include "stm32u5xx_ll_gpio.h"

#elif __has_include("stm32wbxx_ll_gpio.h")
#include "stm32wbxx_ll_gpio.h"

#elif __has_include("stm32wlxx_ll_gpio.h")
#include "stm32wlxx_ll_gpio.h"

#elif __has_include("stm32mp1xx_ll_gpio.h")
#include "stm32mp1xx_ll_gpio.h"

#else
#error                                                                         \
    "Could not detect STM32 family: no stm32*xx_ll_gpio.h header found in include path."
#endif

#endif // defined(__has_include)

class STM32IOPin : public LowLevelEmbedded::IOPIN
{
private:
  GPIO_TypeDef *pio_;
  uint16_t pin_;

public:
  STM32IOPin(GPIO_TypeDef *GPIO_Port, uint16_t GPIO_Pin)
  {
    this->pio_ = GPIO_Port;
    this->pin_ = GPIO_Pin;
  }
  void Set() final { LL_GPIO_SetOutputPin(pio_, pin_); };
  void Clear() final { LL_GPIO_ResetOutputPin(pio_, pin_); };
  bool GetValue() final
  {
    return (pio_->IDR & pin_) != (uint32_t)GPIO_PIN_RESET;
  };
  void Toggle() final { LL_GPIO_TogglePin(pio_, pin_); };
};