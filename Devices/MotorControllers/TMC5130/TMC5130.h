//
// Created by Jeroe on 2025-06-18.
//

#pragma once

#include "LLE_Analog.h"
#include "LLE_SPI.h"
#include "TMC5130_Config.h"
#include <functional>

namespace LowLevelEmbedded::Devices::MotorControllers
{
typedef enum
{
  msIdle,
  msStopped,
  msConstantVelocity,
  msConstantVelocityRampingDown,
  msTrajectory,
  msStalled,
  msTimedConstantVelocity,
  msConstantVelocityRampUp,
  msConstantVelocityUntilSwitch,
  msTimedConstantVelocityRampUp
} MotorState_t;

typedef enum
{
  irNone,
  irStopped,
  irStalled
} MotorIntReason_t;

class TMC5130
{
public:
  static const int STOPSWITCH0MASK = 1;
  static const int STOPSWITCH1MASK = 2;

  TMC5130(ISPIAccess *spiAccess);

  // configuration methods

  /**
   * @brief Configures the general configuration (GCONF) register of the TMC5130
   * driver.
   *
   * This function sets the global configuration flags in the GCONF register
   * according to the parameters provided. These settings control reference
   * sources, internal sensing, and motor behavior.
   *
   * @param clockFrequencyInHz
   *       The clock frequency fed to the TMC5130, typically 16Mhz
   * @param useAnalogCurrentReference
   *        - `true`: Use voltage supplied to AIN as current reference
   * (`I_scale_analog = 1`)
   *        - `false`: Use internal reference voltage (`I_scale_analog = 0`)
   *
   * @param useInternalSenseResistors
   *        - `true`: Enable internal sense resistors; use current into AIN as
   * reference (`internal_Rsense = 1`)
   *        - `false`: Normal operation with external sense resistors
   * (`internal_Rsense = 0`)
   *
   * @param enableStealthChopMode
   *        - `true`: Enable StealthChop voltage PWM mode (`en_pwm_mode = 1`)
   *        - `false`: Disable StealthChop, use standard mode (`en_pwm_mode =
   * 0`)
   *
   * @param reverseMotorDirection
   *        - `true`: Invert motor direction (`shaft = 1`)
   *        - `false`: Use normal motor direction (`shaft = 0`)
   *
   * @return Reference to the configured `TMC5130` instance for method chaining.
   */
  TMC5130 &Configure_GeneralConfiguration(uint32_t clockFrequencyInHz,
                                          bool useAnalogCurrentReference,
                                          bool useInternalSenseResistors,
                                          bool enableStealthChopMode,
                                          bool reverseMotorDirection);

  /**
   * @brief Configures the motor settings for the TMC5130 driver.
   *
   * This function sets various motor configuration parameters including sense
   * resistors, step resolution, and current limits during different operation
   * modes such as idle, ramp-up, and ramp-down phases.
   *
   * @param senseResistorValue
   *        The value of the external sense resistor in ohms.
   *
   * @param fullStepsPerRotation
   *        Number of full steps per rotation of the motor.
   *
   * @param idleCurrentInmA
   *        The current in milliamperes applied to the motor during idle
   * periods.
   *
   * @param fullSpeedCurrentInmA
   *        The current in milliamperes applied during full speed operation.
   *
   * @param rampupCurrentInmA
   *        The current in milliamperes applied during the ramp-up phase.
   *
   * @param rampDownCurrentInmA
   *        The current in milliamperes applied during the ramp-down phase.
   *
   * @return Reference to the configured `TMC5130` instance for method chaining.
   */
  TMC5130 &Configure_MotorConfiguration(float senseResistorValue,
                                        uint16_t fullStepsPerRotation,
                                        uint16_t idleCurrentInmA,
                                        uint16_t fullSpeedCurrentInmA,
                                        uint16_t rampupCurrentInmA,
                                        uint16_t rampDownCurrentInmA);

  /**
 * @brief Set TPWMTHRS – the StealthChop‑to‑SpreadCycle switch‑over speed.
 *
 * The TPWMTHRS register (addr 0x13) stores a 20‑bit *TSTEP* value.
 * When the actual step period **TSTEP ≥ TPWMTHRS** (i.e. the motor
 * is running **slower** than the threshold), StealthChop voltage‑PWM
 * mode is enabled (provided @ref en_pwm_mode is set in GCONF).
 * At higher speeds the driver automatically reverts to SpreadCycle.
 *
 * @param[in] rpm  Shaft speed in revolutions‑per‑minute at which the change‑over
 *                 should occur.  The routine converts this to *TSTEP* using the
 *                 configured micro‑step resolution, steps‑per‑revolution and the
 *                 driver clock (f\_CLK), then writes the result to TPWMTHRS.
 *                 Passing 0 disables StealthChop for the entire speed range.
 *
 * @returns Reference to *this* so the call can be chained fluently.
 *
 */
  TMC5130& Configure_StealthChopThresholdRPM(uint32_t rpm);

  /**
   * @brief Configures the TMC5130 driver to use the default sine wave lookup
   * table.
   *
   * This function sets the internal registers to reference the default sine
   * wave lookup table for stepping control. This ensures consistent
   * microstepping behavior based on the default waveform predefined by the
   * driver.
   *
   * @return Reference to the configured `TMC5130` instance for method chaining.
   */
  TMC5130 & Configure_DefaultSineLookupTable();

  /**
 * @brief Set **TCOOLTHRS** – CoolStep / StallGuard activation speed.
 *
 * The TCOOLTHRS register (address 0x14) stores a 20-bit *TSTEP* limit.
 * When the motor turns **faster** than this limit
 * (that is: the measured step period *TSTEP* is **smaller** than
 * TCOOLTHRS), the driver enables the smart-energy CoolStep current
 * scaling and the StallGuard load-sensor logic.
 * At lower speeds these functions are disabled automatically.
 *
 * @param[in] rpm  Shaft speed, given in revolutions-per-minute, at which
 *                 CoolStep should start operating.
 *                 Passing 0 disables CoolStep for the entire speed range.
 *
 * @return Reference to *this* object so calls can be chained fluently.
 *
 * @note The conversion to the register value uses
 *       `TSTEP = f_CLK / (rpm × stepsPerRev)`, where *stepsPerRev*
 *       already includes the selected micro-step resolution.
 *       The result is clipped to the 20-bit range 0…0xFFFFF.
 */
  TMC5130& Configure_CoolStepThresholdRPM(uint32_t rpm);

  /**
 * @brief Set **THIGH** – the “VHIGH” speed boundary for high-velocity mode.
 *
 * The THIGH register (addr 0x15) holds a 20-bit *TSTEP* value called **VHIGH** in
 * the datasheet.  When the measured step period **TSTEP ≤ THIGH** (i.e. the
 * motor is moving **faster** than the threshold), the driver switches to
 * *high-velocity mode*:
 *
 *  * **CoolStep & StallGuard2** are disabled.
 *  * StealthChop is disabled automatically.
 *  * If `CHOPCONF.vhighfs = 1`, the driver jumps from microstepping to
 *    **full-step** at the next 45 ° position.
 *  * If `CHOPCONF.vhighchm = 1`, the chopper changes to classic constant
 *    off-time (`chm = 1`, `fd = 0`) with an automatic TOFF doubling.
 *
 * @param[in] rpm  Shaft speed *in revolutions per minute* that should be
 *                 treated as the beginning of high-velocity operation.
 *                 Pass **0 rpm** to disable the VHIGH mechanism entirely
 *                 (THIGH and both *vhigh* flags become irrelevant).
 *
 * @return Reference to **this** object for convenient call-chaining.
 *
 * @note The routine converts RPM → *TSTEP* using the configured micro-
 *       step setting, steps-per-revolution, and the driver clock (*f*<sub>CLK</sub>),
 *       then writes the resulting 20-bit value to THIGH.
 */
  TMC5130& setHighVelocityThresholdRPM(uint32_t rpm);

  /**
   * @brief A callback function to handle interrupts generated by the TMC5130.
   *
   * This variable allows users to set a custom interrupt handler for the
   * TMC5130 device. The function is called whenever an interrupt is triggered,
   * passing a reference to the TMC5130 object as an argument. Users should
   * define a suitable handler function to respond to interrupts based on their
   * specific application needs.
   */
  std::function<void(TMC5130 &)> SetInterruptCallback;

  /**
   * @brief A callback function used to determine if a custom home switch has
   * been actuated.
   *
   * This variable holds a callable object (e.g., a lambda, function, or other
   * object with a call operator) that accepts a reference to a `TMC5130` object
   * and returns a boolean indicating whether the home switch is actuated.
   *
   * The function should return `true` if the switch is triggered, and `false`
   * otherwise.
   */
  std::function<bool(TMC5130 &)> GetCustomHomeSwitchActuatedCallback;

  uint8_t ChipID;
  MotorIntReason_t IntReason;
  MotorState_t MotorState;
  int32_t LastStoppedPosition;
  int32_t LastTargetPosition;
  uint16_t Motor_Idle_CurrentInmA;
  uint16_t Motor_RampUp_CurrentInmA;
  uint16_t Motor_FullSpeed_CurrentInmA;
  float SenseResistorInOhms = 0;
  IDACChannel *CurrentSetVoltageChannel;

  // Initialize a TMC5130 IC.
  // This function requires:
  //     - registerResetState: An int32_t array with 128 elements. This holds
  //     the values to be used for a reset.
  void Init(const int32_t *registerResetState);

  // Reset the TMC5130.
  bool Reset();

  /**
   * Performs periodic maintenance and control updates for the TMC5130 motor
   * driver. This function must be called regularly to handle tasks such as
   * state management and trajectory updates.
   *
   * @param elapsedTimeinMs The time in milliseconds since the last invocation
   * of the periodic job.
   */
  void PeriodicJob(uint32_t elapsedTimeinMs);

  /**
   * Initiates a timed movement in constant velocity mode for the TMC5130 motor
   * driver with specified acceleration, velocity, and duration. This function
   * configures motor parameters such as current, velocity, acceleration, and
   * direction, and sets the motor to begin moving based on these settings.
   *
   * @param acc The acceleration value for the movement, in appropriate motor
   * units.
   * @param velocity The target velocity for the movement, in pulses per second.
   *        Positive or negative values determine the direction.
   * @param timeInms The duration for which the motor should maintain the
   * constant velocity, in milliseconds.
   * @param ticksInms The system tick value in milliseconds at the start of the
   * movement.
   */
  void StartTimedConstantVelocity(uint32_t acc, int32_t velocity,
                                  uint32_t timeInms, uint32_t ticksInms);

  /**
   * Starts the motor movement in a constant velocity mode with the specified
   * acceleration and velocity. This function sets up the motor parameters such
   * as acceleration, velocity, and motion direction, as well as enabling
   * StallGuard for monitoring. The motor state is updated accordingly.
   *
   * @param acc The acceleration value in PPS (Pulses Per Second), which
   * determines the rate of change of velocity.
   * @param velocity The target velocity in PPS. The sign of this value dictates
   * the direction of motion (positive for forward, negative for reverse).
   */
  void StartConstantVelocity(int32_t acc, int32_t velocity);

  /**
   * Initiates a motor movement towards a switch with the specified switch ID
   * and polarity, using the provided acceleration and velocity parameters. The
   * motor will stop once the designated switch is activated. This function is
   * typically used for homing operations where the position is referenced by
   * trigger switches.
   *
   * @param switchId The ID of the switch to be targeted. Must correspond to a
   * valid switch identifier recognized by the motor controller. (254 = custom
   * switch)
   * @param polarity The polarity of the switch, indicating whether the switch
   * is triggered on a high or low signal.
   * @param acc The acceleration for the movement in pulses per second squared,
   * determining how quickly the motor reaches the set velocity.
   * @param velocity The target velocity for the movement in pulses per second.
   * The sign of this value determines the direction of movement.
   */
  void StartMoveUntilSwitch(uint8_t switchId, bool polarity, int32_t acc,
                            int32_t velocity);

  /**
   * Stops the movement of the TMC5130 motor immediately. This function halts
   * any ongoing motion and updates the motor's state to reflect the stopped
   * condition. It ensures that the motor is brought to a controlled stop while
   * maintaining the internal state consistency.
   */
  void StopMovement();

  /**
   * Stops the motor movement gradually by applying the specified deceleration.
   * This function ensures the motor comes to a controlled stop.
   *
   * @param dec The deceleration value to apply, which determines how quickly
   * the movement slows down.
   */
  void StopMovement(int32_t dec);

  /**
   * Moves the TMC5130 motor to a specified position using the given motion
   * parameters for acceleration, maximum speed, and deceleration. This function
   * plans and executes the motion while ensuring adherence to the provided
   * constraints.
   *
   * @param position The target position for the motor, expressed in encoder
   * units or microsteps, depending on the system configuration.
   * @param acc The acceleration rate for reaching the target speed, expressed
   * in pulses per second squared.
   * @param speedMax The maximum speed allowed during the movement, in pulses
   * per second.
   * @param dec The deceleration rate for stopping smoothly at the target
   * position, in pulses per second squared.
   */
  void MoveToPosition(int32_t position, int32_t acc, int32_t speedMax,
                      int32_t dec);

private:
  ISPIAccess *_SPIAccess;
  uint8_t _stopSwitchID;
  bool _stopSwitchInverted;
  int32_t _lastStartTimeInms;
  int32_t _targetMoveTimeInms;
  int32_t _registerResetState[TMC5130_REGISTER_COUNT] = {};
  uint8_t _registerAccess[TMC5130_REGISTER_COUNT] = {};
  int32_t _shadowRegister[TMC5130_REGISTER_COUNT] = {};
  ConfigState _configState = CONFIG_READY;
  uint8_t _configIndex = 0;
  // uint8_t (*_resetCallback)(void) = nullptr;
  // uint8_t (*_restoreCallback)(void) = nullptr;

  void _writeDatagram(uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3,
                      uint8_t x4);
  void _writeInt(uint8_t address, int32_t value);
  int32_t _readInt(uint8_t address);
  void _writeConfiguration();
  void _activateMoveCurrent();
  void _activateRampUpCurrent();
  bool _activate_current(uint16_t current_in_mA);
  void _activateIdleCurrent();
};
} // namespace LowLevelEmbedded::Devices::MotorControllers
