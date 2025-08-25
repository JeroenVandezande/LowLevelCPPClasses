#pragma once
#include <cstdint>
#include <cstddef>
#include "LLE_I2C.h"

// --- Auto-include the right STM32 HAL I2C header ----------------------------
#if defined(__has_include)
#if   __has_include("stm32f0xx_hal_i2c.h")
#include "stm32f0xx_hal_i2c.h"
#elif __has_include("stm32f1xx_hal_i2c.h")
#include "stm32f1xx_hal_i2c.h"
#elif __has_include("stm32f2xx_hal_i2c.h")
#include "stm32f2xx_hal_i2c.h"
#elif __has_include("stm32f3xx_hal_i2c.h")
#include "stm32f3xx_hal_i2c.h"
#elif __has_include("stm32f4xx_hal_i2c.h")
#include "stm32f4xx_hal_i2c.h"
#elif __has_include("stm32f7xx_hal_i2c.h")
#include "stm32f7xx_hal_i2c.h"
#elif __has_include("stm32g0xx_hal_i2c.h")
#include "stm32g0xx_hal_i2c.h"
#elif __has_include("stm32g4xx_hal_i2c.h")
#include "stm32g4xx_hal_i2c.h"
#elif __has_include("stm32h7xx_hal_i2c.h")
#include "stm32h7xx_hal_i2c.h"
#elif __has_include("stm32l0xx_hal_i2c.h")
#include "stm32l0xx_hal_i2c.h"
#elif __has_include("stm32l1xx_hal_i2c.h")
#include "stm32l1xx_hal_i2c.h"
#elif __has_include("stm32l4xx_hal_i2c.h")
#include "stm32l4xx_hal_i2c.h"
#elif __has_include("stm32l5xx_hal_i2c.h")
#include "stm32l5xx_hal_i2c.h"
#elif __has_include("stm32u5xx_hal_i2c.h")
#include "stm32u5xx_hal_i2c.h"
#elif __has_include("stm32wbxx_hal_i2c.h")
#include "stm32wbxx_hal_i2c.h"
#elif __has_include("stm32wlxx_hal_i2c.h")
#include "stm32wlxx_hal_i2c.h"
#elif __has_include("stm32mp1xx_hal_i2c.h")
#include "stm32mp1xx_hal_i2c.h"
#else
#error "No stm32*xx_hal_i2c.h found in include paths. Add the proper Cube HAL include directories."
#endif
#else
#error "__has_include not supported by this compiler. Include the correct stm32*xx_hal_i2c.h before this header."
#endif
// ---------------------------------------------------------------------------

namespace LowLevelEmbedded
{
    class STM32I2CAccess : public II2CAccess
    {
    private:
        I2C_HandleTypeDef* I2C_;
        uint32_t timeout_ms_;

    public:
        // Constructor:
        // - i2c: HAL I2C handle
        // - timeoutMs: per-call timeout (default 50 ms like your sample)
        STM32I2CAccess(I2C_HandleTypeDef* i2c, uint32_t timeoutMs = 50)
            : I2C_(i2c), timeout_ms_(timeoutMs)
        {
        }

        // Simple read: Master Receive
        bool I2C_ReadMethod(uint8_t address, uint8_t* data, size_t length) override
        {
            HAL_StatusTypeDef st = HAL_I2C_Master_Receive(I2C_, address, data, static_cast<uint16_t>(length),
                                                          timeout_ms_);
            return (st == HAL_OK);
        }

        // Simple write: Master Transmit
        bool I2C_WriteMethod(uint8_t address, uint8_t* data, size_t length) override
        {
            HAL_StatusTypeDef st = HAL_I2C_Master_Transmit(I2C_, address, data, static_cast<uint16_t>(length),
                                                           timeout_ms_);
            return (st == HAL_OK);
        }

        // Write then Read (common for register access without STOP in between if repeated start is used)
        bool I2C_ReadWriteMethod(uint8_t address, uint8_t* data, size_t readLength, size_t writeLength) override
        {
            HAL_StatusTypeDef st = HAL_I2C_Master_Transmit(I2C_, address, data, static_cast<uint16_t>(writeLength),
                                                           timeout_ms_);
            if (st != HAL_OK) return false;

            st = HAL_I2C_Master_Receive(I2C_, address, data, static_cast<uint16_t>(readLength), timeout_ms_);
            return (st == HAL_OK);
        }

        bool I2C_Mem_Read(uint8_t address, uint8_t memAddress, uint8_t memAddSize, uint8_t* data,
                          size_t length) override
        {
            HAL_StatusTypeDef st = HAL_I2C_Mem_Read(I2C_, address, memAddress, memAddSize, data,
                                                    static_cast<uint16_t>(length), timeout_ms_);
            return (st == HAL_OK);
        }

        // Ping device
        bool I2C_IsDeviceReady(uint8_t address) override
        {
            HAL_StatusTypeDef st = HAL_I2C_IsDeviceReady(I2C_, address, 3, timeout_ms_);
            return (st == HAL_OK);
        }

        I2C_HandleTypeDef* Handle() const { return I2C_; }
        uint32_t TimeoutMs() const { return timeout_ms_; }
        void SetTimeoutMs(uint32_t ms) { timeout_ms_ = ms; }
    };
} // namespace LowLevelEmbedded
