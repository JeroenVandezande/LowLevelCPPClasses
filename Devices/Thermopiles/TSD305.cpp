//
// Created by DanaNatov on 2025-05-16.
//

#include "TSD305.h"

namespace LowLevelEmbedded
{
    namespace Devices
    {
        namespace Thermopiles
        {
            /**
             * Constructor for TSD305 class.
             * Initializes the TSD305 object with a specific I2C access interface and device address.
             *
             * @param i2cAccess A pointer to the II2CAccess interface used for I2C communication.
             * @param i2cAddress The 7-bit I2C address of the TSD305 device.
             */
            TSD305::TSD305(II2CAccess *i2cAccess, uint8_t i2cAddress)
            {
                this->i2cAccess = i2cAccess;
                this->i2cAddress = i2cAddress;
            }

            /**
             * Checks the provided status byte for specific error flags and returns the corresponding error code.
             *
             * @param statusByte A byte representing the status to be checked.
             *                   It may contain flags for errors or other status indicators.
             * @return An error code of type TSD305_Constants::ErrorCode. Possible values are:
             *         - MEMORY_ERROR: If the memory error flag is set in the status byte.
             *         - BUSY: If the busy flag is set in the status byte.
             *         - OK: If no error flags are detected in the status byte.
             */
            TSD305_Constants::ErrorCode TSD305::CheckStatusByte(uint8_t statusByte)
            {
                if (statusByte & (uint8_t)TSD305_Constants::StatusByte::MEMORY_ERROR) // Memory Error
                {
                    return TSD305_Constants::ErrorCode::MEMORY_ERROR;
                }
                if (statusByte & (uint8_t)TSD305_Constants::StatusByte::BUSY)
                {
                    return TSD305_Constants::ErrorCode::BUSY;
                }

                return TSD305_Constants::ErrorCode::OK;
            }

            /**
             * Recalculates the CRC for the TSD305 device.
             * This method sends a specific command to the device to trigger a CRC recalculation.
             *
             * @return An error code indicating the result of the operation:
             *         - TSD305_Constants::ErrorCode::OK if the operation was successful.
             *         - TSD305_Constants::ErrorCode::I2C_WRITE_ERROR if there was an error during I2C communication.
             */
            TSD305_Constants::ErrorCode TSD305::RecalcCRC()
            {
                uint8_t data[1] = {TSD305_Constants::CALC_CRC_ADDRESS };
                if (!i2cAccess->I2C_WriteMethod(i2cAddress, data, 1))
                {
                    return TSD305_Constants::ErrorCode::I2C_WRITE_ERROR;
                }

                return TSD305_Constants::ErrorCode::OK;
            }

            /**
             * Writes a byte of data to the specified address on the I2C device.
             *
             * @param data_address The address on the I2C device where the data will be written.
             * @param data The byte of data to be written to the specified address.
             * @return An error code of type TSD305_Constants::ErrorCode
             *         indicating the success or failure of the operation.
             *         Returns TSD305_Constants::ErrorCode::OK on success,
             *         or TSD305_Constants::ErrorCode::I2C_WRITE_ERROR on failure.
             */
            TSD305_Constants::ErrorCode TSD305::WriteByte(uint8_t data_address, uint8_t data)
            {
                uint8_t buffer[2] = { data_address, data };
                if (!i2cAccess->I2C_WriteMethod(i2cAddress, buffer, 2))
                {
                    return TSD305_Constants::ErrorCode::I2C_WRITE_ERROR;
                }
                return TSD305_Constants::ErrorCode::OK;
            }

            /**
             * Reads a 16-bit word from the specified data address over the I2C interface.
             *
             * @param data_address The address from which the 16-bit data word should be read.
             * @param data A reference to a uint16_t where the read 16-bit data will be stored.
             * @return An error code of type TSD305_Constants::ErrorCode indicating the success or failure of the operation.
             */
            TSD305_Constants::ErrorCode TSD305::ReadWord(uint8_t data_address, uint16_t& data)
            {
                uint8_t addr = data_address;
                uint8_t data_out[3] = { 0, 0, 0 };
                if (!i2cAccess->I2C_WriteMethod(i2cAddress, &addr, 1))
                    return TSD305_Constants::ErrorCode::I2C_WRITE_ERROR;
                if (!i2cAccess->I2C_ReadMethod(i2cAddress, data_out, 3))
                    return TSD305_Constants::ErrorCode::I2C_READ_ERROR;
                data = (uint16_t)((data_out[1] << 8) | data_out[2]);
                return CheckStatusByte(data_out[0]);
            }

            /**
             * Reads a 32-bit long word from the specified data address by combining two consecutive 16-bit words.
             *
             * @param data_address The address from which the data is to be read.
             * @param data Reference to a 32-bit unsigned integer where the read data will be stored.
             * @return An error code representing the result of the operation. Returns TSD305_Constants::ErrorCode::OK if successful or an appropriate error code otherwise.
             */
            TSD305_Constants::ErrorCode TSD305::ReadLongWord(uint8_t data_address, uint32_t& data)
            {
                uint16_t word1 = 0;
                uint16_t word2 = 0;
                TSD305_Constants::ErrorCode result = ReadWord(data_address, word1);
                if (result != TSD305_Constants::ErrorCode::OK) return result;
                result = ReadWord(data_address + 1, word2);
                data = (uint32_t)((word1 << 16) | word2);
                return result;
            }

            /**
             * Reads the temperature coefficient from the sensor.
             *
             * This method reads a 32-bit value from the sensor's temperature coefficient
             * memory address and converts it into a floating-point value to represent the
             * temperature coefficient.
             *
             * @param tc A reference to a float where the temperature coefficient will be stored.
             * @return A TSD305_Constants::ErrorCode indicating the success or failure of the operation.
             */
            TSD305_Constants::ErrorCode TSD305::ReadTemperatureCoefficient(float& tc)
            {
                uint32_t data = 0;
                TSD305_Constants::ErrorCode result = ReadLongWord(TSD305_Constants::ADDR_TEMP_COEFF, data);
                tc = static_cast<float>(data);
                return result;
            }

            /**
             * Reads the reference temperature from the sensor.
             *
             * This method retrieves the reference temperature from the sensor's memory
             * and converts it into a floating-point value.
             *
             * @param tref A reference to a float where the reference temperature value
             *             will be stored. The value is converted to a floating-point
             *             representation.
             * @return An error code of type TSD305_Constants::ErrorCode indicating the
             *         success or failure of the operation.
             */
            TSD305_Constants::ErrorCode TSD305::ReadReferenceTemperature(float& tref)
            {
                uint32_t data;
                TSD305_Constants::ErrorCode result = ReadLongWord(TSD305_Constants::ADDR_REF_TEMP, data);
                tref = static_cast<float>(data);
                return result;
            }

            /**
             * Reads the sensor's temperature range, including the minimum and maximum temperatures.
             *
             * @param tmin Reference to an integer for storing the minimum temperature value of the sensor.
             * @param tmax Reference to an integer for storing the maximum temperature value of the sensor.
             * @return A TSD305_Constants::ErrorCode indicating the success or failure of the operation.
             */
            TSD305_Constants::ErrorCode TSD305::ReadSensorTempRange(int16_t& tmin, int16_t& tmax)
            {
                uint16_t data = 0;
                TSD305_Constants::ErrorCode result = ReadWord(TSD305_Constants::ADDR_SENSOR_TEMP_MIN, data);
                tmin = data;
                if (result != TSD305_Constants::ErrorCode::OK) return result;

                result = ReadWord(TSD305_Constants::ADDR_SENSOR_TEMP_MAX, data);
                tmax = data;

                return result;
            }

            /**
             * Reads the minimum and maximum object temperature range from the sensor.
             *
             * @param[out] tmin Reference to a variable where the minimum object temperature will be stored.
             * @param[out] tmax Reference to a variable where the maximum object temperature will be stored.
             * @return An error code of type TSD305_Constants::ErrorCode indicating the success or failure of the operation.
             */
            TSD305_Constants::ErrorCode TSD305::ReadObjectTempRange(int16_t& tmin, int16_t& tmax)
            {
                uint16_t data = 0;
                TSD305_Constants::ErrorCode result = ReadWord(TSD305_Constants::ADDR_OBJECT_TEMP_MIN, data);
                tmin = data;

                result = ReadWord(TSD305_Constants::ADDR_OBJECT_TEMP_MAX, data);
                tmax = data;

                return TSD305_Constants::ErrorCode::OK;
            }

            /**
             * Reads compensation coefficients from predefined memory addresses and stores them into the provided array.
             *
             * @param coeffs An array of size 5 where the read compensation coefficients will be stored.
             *               Each coefficient is converted to a floating-point value.
             * @return An error code of type TSD305_Constants::ErrorCode indicating the success or failure of the operation.
             */
            TSD305_Constants::ErrorCode TSD305::ReadCompensationCoefficients(float coeffs[5])
            {
                const uint8_t addresses[5] = {
                    TSD305_Constants::ADDR_K4_COMP, TSD305_Constants::ADDR_K3_COMP, TSD305_Constants::ADDR_K2_COMP,
                    TSD305_Constants::ADDR_K1_COMP, TSD305_Constants::ADDR_K0_COMP
                };

                uint32_t data = 0;
                TSD305_Constants::ErrorCode result = TSD305_Constants::ErrorCode::OK;
                for (int i = 0; i < 5; ++i) {
                    result = ReadLongWord(addresses[i], data);
                    coeffs[i] = static_cast<float>(data);
                    if (result != TSD305_Constants::ErrorCode::OK) return result;
                }
                return result;
            }

            /**
             * Reads the object temperature coefficients from the device.
             *
             * This method reads five specific coefficients from the device memory
             * addresses and stores them into the provided array.
             *
             * @param coeffs An array of size 5 to store the read temperature coefficients.
             *               Each coefficient is represented as a float.
             * @return An error code of type TSD305_Constants::ErrorCode indicating
             *         the success or failure of the operation.
             */
            TSD305_Constants::ErrorCode TSD305::ReadObjectTempCoefficients(float coeffs[5])
            {
                const uint8_t addresses[5] = {
                    TSD305_Constants::ADDR_K4_OBJ, TSD305_Constants::ADDR_K3_OBJ, TSD305_Constants::ADDR_K2_OBJ,
                    TSD305_Constants::ADDR_K1_OBJ, TSD305_Constants::ADDR_K0_OBJ
                };

                uint32_t data;
                TSD305_Constants::ErrorCode result = TSD305_Constants::ErrorCode::OK;
                for (int i = 0; i < 5; ++i) {
                    result = ReadLongWord(addresses[i], data);
                    coeffs[i] = static_cast<float>(data);
                }
                return result;
            }

            /**
             * Changes the I2C address of the device and recalculates the CRC.
             * A power cycle is required after successfully changing the address.
             *
             * @param newAddress The new I2C address to assign to the device.
             * @return Returns the result of the operation as an ErrorCode. If the operation
             *         is successful, it returns TSD305_Constants::ErrorCode::OK. Otherwise,
             *         it returns the appropriate error code.
             */
            TSD305_Constants::ErrorCode TSD305::ChangeI2CAddress(uint8_t newAddress)
            {
                TSD305_Constants::ErrorCode result =
                    WriteByte(TSD305_Constants::ADDR_I2C_ADDRESS + TSD305_Constants::WRITE_DATA_OFFSET, newAddress);
                if (result != TSD305_Constants::ErrorCode::OK) return result;
                result = RecalcCRC();
                if (result != TSD305_Constants::ErrorCode::OK) return result;
                i2cAddress = newAddress;
                return result;
                // Power Cycle is required!
            }

            TSD305_Constants::ErrorCode TSD305::PerformMeasurement(uint32_t& objectADC, uint32_t& sensorADC)
            {

            }

        }
    }
}