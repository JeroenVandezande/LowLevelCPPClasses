//
// Created by Jeroe on 2025-06-20.
//

#ifndef LLE_ANALOG_H
#define LLE_ANALOG_H
#include <cstdint>
#include <concepts>

namespace LowLevelEmbedded
{
    /**
     * @class ISimpleDACChannel
     * @brief Interface for a simple Digital-to-Analog Converter (DAC) channel.
     *
     * Provides an abstract interface for writing voltage values to a DAC channel.
     * This interface is designed for use in hardware abstraction layers to allow
     * control over individual DAC channels without dependency on specific hardware.
     *
     * Derived classes are required to implement the method for writing voltage
     * values, ensuring conformance to this general interface.
     */
    class ISimpleDACChannel
    {
        /**
         * Destructor for the ISimpleDACChannel interface.
         *
         * Ensures proper cleanup and resource deallocation during destruction of derived
         * classes that implement this interface. Allows derived classes to handle cleanup
         * in a polymorphic context when objects are destroyed through a pointer to this interface.
         */
    public:
        virtual ~ISimpleDACChannel()
        {
        };

        /**
         * Writes a specified voltage value to the DAC.
         *
         * @param value The desired voltage in volts to be written to the DAC.
         * @return Returns true if the operation succeeded, otherwise false.
         */
        virtual bool WriteDACVoltage(float value) = 0;
    };

    template <std::unsigned_integral ValueT>
    /**
     * Interface for a Digital-to-Analog Converter (DAC) channel.
     *
     * Provides an abstract interface for manipulating individual channels of a DAC.
     * Derived classes must implement the methods for writing digital values or
     * voltage levels to the channel. This class allows hardware abstraction and
     * facilitates interaction with DAC peripherals.
     */
    class IDACChannel: public ISimpleDACChannel
    {
        /**
         * Destructor for the IDACChannel interface.
         *
         * This virtual destructor ensures that derived classes will have their
         * destructors called properly during object destruction, enabling safe cleanup of
         * dynamically allocated resources if the object is deleted through a pointer
         * to this interface.
         */
    public:
        virtual ~IDACChannel()
        {
        };

        /**
         * Writes a digital value to the DAC (Digital-to-Analog Converter).
         *
         * @param value The 16-bit digital value to be written to the DAC.
         *              The specific range and resolution are dependent
         *              on the implementation.
         * @return True if the operation is successful, otherwise false.
         */
        virtual bool WriteDAC(ValueT value) = 0;

        /**
         * @brief Retrieves the maximum digital-to-analog (DA) value supported by the DAC.
         *
         * This virtual method allows querying the maximum value range that can be output
         * by the DAC. The specific range is determined by the DAC hardware capabilities.
         *
         * @tparam ValueT The numeric type, as defined for the DAC, representing the DA value.
         * @return The maximum supported DA value for the DAC.
         */
        virtual ValueT GetMaxDAValue() = 0;

    };

    template <std::unsigned_integral ValueT>
    /**
     * @class IDAC
     * @brief Abstract interface for managing a Digital-to-Analog Converter (DAC).
     *
     * This class serves as a base interface for interacting with DAC hardware. It provides abstract methods
     * for writing values to DAC channels either as raw data or voltages, as well as querying DAC properties.
     * Subclasses must implement these methods based on the specific DAC hardware.
     *
     * @tparam ValueT The numeric type used for representing DAC values, typically an unsigned integral type.
     */
    class IDAC
    {
        /**
         * @brief Virtual destructor for the IDAC class.
         *
         * Ensures proper cleanup of derived class objects during destruction.
         */
    public:
        virtual ~IDAC()
        {
        };

        /**
         * Writes a value to the specified DAC (Digital-to-Analog Converter) channel.
         *
         * @param channel The DAC channel to write to. Typically ranges from 0 to the maximum number of available channels.
         * @param value The value to write to the DAC. The range depends on the DAC resolution (e.g., for a 12-bit DAC, the range is 0 to 4095).
         * @return Returns true if the value was written successfully, otherwise false.
         */
        virtual bool WriteDAC(uint8_t channel, ValueT value) = 0;

        /**
         * @brief Retrieves the maximum digital-to-analog (DA) value supported by the DAC.
         *
         * This virtual method allows querying the maximum value range that can be output
         * by the DAC. The specific range is determined by the DAC hardware capabilities.
         *
         * @tparam ValueT The numeric type, as defined for the DAC, representing the DA value.
         * @return The maximum supported DA value for the DAC.
         */
        virtual ValueT GetMaxDAValue() = 0;


        /**
         * @brief Retrieves the maximum number of DAC channels supported by the hardware.
         *
         * This virtual method allows querying the total number of channels that can be
         * accessed and controlled by the DAC. The exact number is determined by
         * the specific DAC hardware implementation.
         *
         * @return The maximum number of supported DAC channels.
         */
        virtual uint8_t GetMaxChannels() = 0;

        /**
         * Writes a specific voltage value to the specified DAC channel.
         *
         * @param channel The DAC channel to which the voltage will be written.
         * @param value The voltage value to be written. The valid range depends on the DAC hardware specifications.
         * @return True if the operation is successful, false otherwise.
         */
        virtual bool WriteDACVoltage(uint8_t channel, float value) = 0;

        /**
         * Creates and returns an object that represents a specific DAC channel.
         *
         * @param channel The DAC channel number for which the object is to be created.
         * @return A pointer to the newly created IDACChannel object for the specified channel.
         */
        virtual IDACChannel<ValueT>* CreateChannelObject(uint8_t channel) = 0;
    };

    template <std::unsigned_integral ValueT>
    class DACChannel_base: public IDACChannel<ValueT>
    {
    private:
        IDAC<ValueT> *parent_;
        uint8_t channel_;
    public:

        DACChannel_base(IDAC<ValueT>* parent, const uint8_t channel)
        {
            parent_ = parent;
            channel_ = channel;
        }

        bool WriteDACVoltage(float value) override
        {
            return parent_->WriteDACVoltage(channel_, value);
        }
        bool WriteDAC(ValueT value) override
        {
            return parent_->WriteDAC(channel_, value);
        }
        ValueT GetMaxDAValue() override
        {
            return parent_->GetMaxDAValue();
        }

    };
} // namespace LowLevelEmbedded
#endif // LLE_ANALOG_H
