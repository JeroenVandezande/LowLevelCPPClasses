#ifndef TSD305CONSTANTS_H
#define TSD305CONSTANTS_H

#include "../../../Base/LLE_I2C.h"

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace Thermopiles
        {
            namespace TSD305_Constants
            {
                constexpr uint8_t DEFAULT_I2C_ADDRESS       = 0x00;
                constexpr uint8_t WRITE_DATA_OFFSET         = 0x40;
                constexpr uint8_t CALC_CRC_ADDRESS          = 0x90;

                // Identification
                constexpr uint8_t ADDR_LOT_NUMBER           = 0x00;
                constexpr uint8_t ADDR_SERIAL_NUMBER        = 0x01;
                constexpr uint8_t ADDR_I2C_ADDRESS          = 0x02;

                // Factory Calibration Data
                constexpr uint8_t ADDR_SENSOR_TEMP_MIN      = 0x1A;
                constexpr uint8_t ADDR_SENSOR_TEMP_MAX      = 0x1B;
                constexpr uint8_t ADDR_OBJECT_TEMP_MIN      = 0x1C;
                constexpr uint8_t ADDR_OBJECT_TEMP_MAX      = 0x1D;

                constexpr uint8_t ADDR_TEMP_COEFF           = 0x1E;
                constexpr uint8_t ADDR_REF_TEMP             = 0x20;

                // Measurement Commands
                enum class MeasurementType : uint8_t {
                    SAMPLES_1  = 0xAA,
                    SAMPLES_2  = 0xAC,
                    SAMPLES_4  = 0xAD,
                    SAMPLES_8  = 0xAE,
                    SAMPLES_16 = 0xAF
                };

                // Status Byte bit Flags
                enum class StatusByte : uint8_t
                {
                     MEMORY_ERROR = 0x04,
                     BUSY         = 0x32,
                };

                enum class ErrorCode : uint8_t
                {
                     OK = 0x00,
                     BUSY = 0x01,
                     MEMORY_ERROR = 0x02,
                     I2C_WRITE_ERROR = 0x03,
                     I2C_READ_ERROR = 0x04,
                     CRC_ERROR = 0x05
                };

                // Compensation Coefficients
                constexpr uint8_t ADDR_K4_COMP              = 0x22;
                constexpr uint8_t ADDR_K3_COMP              = 0x24;
                constexpr uint8_t ADDR_K2_COMP              = 0x26;
                constexpr uint8_t ADDR_K1_COMP              = 0x28;
                constexpr uint8_t ADDR_K0_COMP              = 0x2A;

                // Object Temp Coefficients
                constexpr uint8_t ADDR_K4_OBJ               = 0x2E;
                constexpr uint8_t ADDR_K3_OBJ               = 0x30;
                constexpr uint8_t ADDR_K2_OBJ               = 0x32;
                constexpr uint8_t ADDR_K1_OBJ               = 0x33;
                constexpr uint8_t ADDR_K0_OBJ               = 0x36;

                // Miscellaneous
                constexpr uint8_t ADDR_UNUSED               = 0x2D;
                constexpr uint8_t ADDR_FACTORY_CAL_STATUS   = 0x38;
            }
        }
    }
}

#endif //TSD305CONSTANTS_H