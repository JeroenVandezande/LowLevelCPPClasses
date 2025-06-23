#include "TMC5130.h"
#include "TMC5130_Bits.h"
#include "TMC5130_Fields.h"
#include "TMC5130_Macros.h"
#include "TMC5130_Register.h"
#include "TMC5130_RegisterAccess.h"
#include "TMC5130_Utils.h"

#include <cmath>
#include <ulog.h>

namespace LowLevelEmbedded::Devices::MotorControllers
{

TMC5130::TMC5130(ISPIAccess *spiAccess) { this->_SPIAccess = spiAccess; }

void TMC5130::StopMovement()
{
  _writeInt(TMC5130_VMAX, 0);
  this->MotorState = msConstantVelocityRampingDown;
}

void TMC5130::StopMovement(int32_t dec)
{
  log_info("TMC5130: Stopping");
  _writeInt(TMC5130_VMAX, 0);
  _writeInt(TMC5130_AMAX, AccDecPPSToMotorUnits(dec));
  this->MotorState = msConstantVelocityRampingDown;
}

void TMC5130::MoveToPosition(int32_t position, int32_t acc, int32_t speedMax,
                             int32_t dec)
{
  log_info("TMC5130: Moving to position %ld with acc=%ld, speed=%ld, dec=%ld",
           position, acc, speedMax, dec);
  // Set Current for Motor
  _activateMoveCurrent();
  _writeInt(TMC5130_RAMPMODE, TMC5130_MODE_POSITION);
  _writeInt(TMC5130_VMAX, SpeedPPSToMotorUnits(speedMax));
  _writeInt(TMC5130_AMAX, AccDecPPSToMotorUnits(acc));
  _writeInt(TMC5130_DMAX, AccDecPPSToMotorUnits(dec));
  _writeInt(TMC5130_XTARGET, position);
  this->LastTargetPosition = position;
  this->MotorState = msTrajectory;
}

void TMC5130::StartMoveUntilSwitch(uint8_t switchId, bool polarity, int32_t acc,
                                   int32_t velocity)
{
  if (switchId == 254) // check if callback was assigned
  {
    if (!(this->GetCustomHomeSwitchActuatedCallback))
    {
      return;
    }
  }

  // Set Current for Motor
  _activateMoveCurrent();
  this->_stopSwitchID = switchId;
  this->_stopSwitchInverted = !polarity;
  // Set absolute velocity
  _writeInt(TMC5130_VMAX, SpeedPPSToMotorUnits(abs(velocity)));
  _writeInt(TMC5130_AMAX, AccDecPPSToMotorUnits(acc));
  // Set direction
  _writeInt(TMC5130_RAMPMODE,
            (velocity >= 0) ? TMC5130_MODE_VELPOS : TMC5130_MODE_VELNEG);
  this->MotorState = msConstantVelocityUntilSwitch;
}

void TMC5130::StartConstantVelocity(int32_t acc, int32_t velocity)
{
  log_info(
      "TMC5130: Starting constant velocity motion with acc=%ld, velocity=%ld",
      acc, velocity);
  // Set Current for Motor
  _activateRampUpCurrent();
  // Set absolute velocity
  _writeInt(TMC5130_VMAX, SpeedPPSToMotorUnits(abs(velocity)));
  _writeInt(TMC5130_AMAX, AccDecPPSToMotorUnits(acc));
  // Set direction
  _writeInt(TMC5130_RAMPMODE,
            (velocity >= 0) ? TMC5130_MODE_VELPOS : TMC5130_MODE_VELNEG);
  this->MotorState = msConstantVelocityRampUp;
}

/**
 * Initiates motor movement at a constant velocity for a specified duration. The
 * motor's acceleration, velocity, and runtime are configured, and the motor
 * state is updated to "TimedConstantVelocityRampUp". This function also sets up
 * motor-specific parameters required for timed movement.
 *
 * @param acc The acceleration value in PPS (Pulses Per Second), used to
 * configure the motor's accel/decel behavior.
 * @param velocity The target velocity in PPS (can be positive or negative) to
 * determine motor speed and direction.
 * @param timeInms The duration (in milliseconds) for which the motor should
 * move at the specified velocity.
 * @param ticksInms The current time tick value in milliseconds from a hardware
 * timer, used to manage timed operations.
 */
void TMC5130::StartTimedConstantVelocity(uint32_t acc, int32_t velocity,
                                         uint32_t timeInms, uint32_t ticksInms)
{
  this->_lastStartTimeInms = ticksInms;
  this->_targetMoveTimeInms = timeInms;
  // Set Current for Motor
  _activateRampUpCurrent();
  // Set absolute velocity
  _writeInt(TMC5130_VMAX, SpeedPPSToMotorUnits(abs(velocity)));
  _writeInt(TMC5130_AMAX, AccDecPPSToMotorUnits(acc));
  // Set direction
  _writeInt(TMC5130_RAMPMODE,
            (velocity >= 0) ? TMC5130_MODE_VELPOS : TMC5130_MODE_VELNEG);
  this->MotorState = msTimedConstantVelocityRampUp;
}

/**
 * Writes the motor driver's configuration settings to the relevant hardware
 * registers. This function determines the next actionable register based on
 * the current configuration state (restore or reset) and writes the appropriate
 * value. If all necessary registers are processed, the configuration state is
 * set to CONFIG_READY.
 *
 * This function handles:
 * - Register processing for restoring configuration values from the shadow
 *   register during CONFIG_RESTORE state.
 * - Register resetting using default values during CONFIG_RESET state.
 * - Transitions to CONFIG_READY state after completing all required writes.
 */
void TMC5130::_writeConfiguration()
{
  log_info("TMC5130 #%d: Writing configuration", ChipID);
  uint8_t *ptr = &(this->_config->configIndex);
  const int32_t *settings;

  if (this->_config->state == CONFIG_RESTORE)
  {
    settings = this->_config->shadowRegister;
    // Find the next restorable register
    while ((*ptr < TMC5130_REGISTER_COUNT) &&
           !TMC_IS_RESTORABLE(this->_registerAccess[*ptr]))
    {
      (*ptr)++;
    }
  }
  else
  {
    settings = this->_registerResetState;
    // Find the next resettable register
    while ((*ptr < TMC5130_REGISTER_COUNT) &&
           !TMC_IS_RESETTABLE(this->_registerAccess[*ptr]))
    {
      (*ptr)++;
    }
  }

  if (*ptr < TMC5130_REGISTER_COUNT)
  {
    _writeInt(*ptr, settings[*ptr]);
    (*ptr)++;
  }
  else // Finished configuration
  {
    this->_config->state = CONFIG_READY;
  }
}

/**
 * Executes periodic operations for the TMC5130 motor driver based on the
 * current motor state and elapsed time since the last call. Handles tasks such
 * as state transitions, speed adjustments, and invoking configured callbacks.
 *
 * @param elapsedTimeinMs The time elapsed since the last invocation of this
 * method, in milliseconds.
 */
void TMC5130::PeriodicJob(uint32_t elapsedTimeinMs)
{
  if (this->_config->state != CONFIG_READY)
  {
    _writeConfiguration();
    return;
  }
  volatile int32_t xactual = _readInt(TMC5130_XACTUAL);
  volatile int32_t rampstatus = _readInt(TMC5130_RAMPSTAT);
  volatile MotorState_t tempMotorState = this->MotorState;
  bool switchState = false;
  switch (tempMotorState)
  {
  case msIdle:
  case msConstantVelocity:
    // do nothing
    break;
  case msStopped:
    _activateIdleCurrent();
    this->MotorState = msIdle;
    this->LastStoppedPosition = xactual;
    this->IntReason = irStopped;
    if (this->SetInterruptCallback) // check if callback was assigned
    {
      this->SetInterruptCallback(*this);
    }
    break;
  case msStalled:
    _activateIdleCurrent();
    this->IntReason = irStalled;
    if (this->SetInterruptCallback) // check if callback was assigned
    {
      this->SetInterruptCallback(*this);
    }
    this->MotorState = msIdle;
    break;

  case msConstantVelocityRampingDown:
    if (rampstatus & TMC5130_RS_VZERO_MASK) // check if speed is zero
    {
      this->MotorState = msStopped;
    }
    break;
  case msTrajectory:
    if (rampstatus & TMC5130_RS_VZERO_MASK) // check if speed is zero and that
    // position is reached.
    {
      int32_t delta = this->LastTargetPosition - xactual;
      if (abs(delta) < 10)
      {
        _writeInt(TMC5130_XTARGET, xactual);
        this->MotorState = msStopped;
      }
    }
    break;
  case msConstantVelocityRampUp:
    if (rampstatus & TMC5130_RS_VELREACHED_MASK) // check if speed is reached.
    {
      this->MotorState = msConstantVelocity;
      _activateMoveCurrent();
    }
    break;
  case msTimedConstantVelocityRampUp:
    if (rampstatus & TMC5130_RS_VELREACHED_MASK) // check if speed is reached.
    {
      this->MotorState = msTimedConstantVelocity;
      _activateMoveCurrent();
    }
    break;
  case msTimedConstantVelocity:
    if ((elapsedTimeinMs - this->_lastStartTimeInms) >=
        this->_targetMoveTimeInms)
    {
      _writeInt(TMC5130_AMAX, 65535);
      _writeInt(TMC5130_VMAX, 0);
      this->MotorState = msStopped;
    }
    break;
  case msConstantVelocityUntilSwitch:
    switch (this->_stopSwitchID)
    {
    case 0:
      switchState = ((rampstatus & STOPSWITCH0MASK) != 0);
      break;
    case 1:
      switchState = ((rampstatus & STOPSWITCH1MASK) != 0);
      break;
    case 254: // Custom Callback!
      switchState = this->GetCustomHomeSwitchActuatedCallback(*this);
      break;
    }
    if (this->_stopSwitchInverted)
    {
      if (!switchState)
      {
        _writeInt(TMC5130_AMAX, 65535);
        _writeInt(TMC5130_VMAX, 0);
        this->MotorState = msStopped;
      }
    }
    else
    {
      if (switchState)
      {
        _writeInt(TMC5130_AMAX, 65535);
        _writeInt(TMC5130_VMAX, 0);
        this->MotorState = msStopped;
      }
    }
    break;
  }
}

// Register access permissions:
//   0x00: none (reserved)
//   0x01: read
//   0x02: write
//   0x03: read/write
//   0x13: read/write, seperate functions/values for reading or writing
//   0x21: read, flag register (read to clear)
//   0x42: write, has hardware presets on reset
static const uint8_t tmc5130_defaultRegisterAccess[TMC5130_REGISTER_COUNT] = {
    //  0     1     2     3     4     5     6     7     8     9     A     B C D
    //  E     F
    0x03, 0x21, 0x01, 0x02, 0x13, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x00 - 0x0F
    0x02, 0x02, 0x01, 0x02, 0x02, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x10 - 0x1F
    0x03, 0x03, 0x01, 0x06, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x00, 0x02, 0x06, 0x02, 0x03, 0x00, 0x00, // 0x20 - 0x2F
    0x00, 0x00, 0x00, 0x02, 0x03, 0x21, 0x01, 0x00,
    0x03, 0x03, 0x02, 0x21, 0x01, 0x00, 0x00, 0x00, // 0x30 - 0x3F
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x40 - 0x4F
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x50 - 0x5F
    0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x01, 0x01, 0x03, 0x02, 0x02, 0x01, // 0x60 - 0x6F
    0x42, 0x01, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // 0x70 - 0x7F
};

// Register constants (only required for 0x42 registers, since we do not have
// any way to find out the content but want to hold the actual value in the
// shadow register so an application (i.e. the TMCL IDE) can still display
// the values. This only works when the register content is constant.
static const TMCRegisterConstant tmc5130_RegisterConstants[] = {
    // Use ascending addresses!
    {0x60, (uint8_t)0xAAAAB554}, // MSLUT[0]
    {0x61, 0x4A9554AA},          // MSLUT[1]
    {0x62, 0x24492929},          // MSLUT[2]
    {0x63, 0x10104222},          // MSLUT[3]
    {0x64, (uint8_t)0xFBFFFFFF}, // MSLUT[4]
    {0x65, (uint8_t)0xB5BB777D}, // MSLUT[5]
    {0x66, 0x49295556},          // MSLUT[6]
    {0x67, 0x00404222},          // MSLUT[7]
    {0x68, (uint8_t)0xFFFF8056}, // MSLUTSEL
    {0x69, 0x00F70000},          // MSLUTSTART
    {0x70, 0x00050480}           // PWMCONF
};

/**
 * Resets the TMC5130 motor controller to its default state. This function
 * clears the dirty bits, wipes shadow registers, and updates the configuration
 * state to CONFIG_RESET, preparing the controller for reconfiguration.
 *
 * @return True if the reset was successful, or false if the controller was not
 * in the CONFIG_READY state.
 */
bool TMC5130::Reset()
{
  log_info("TMC5130 #%d: Resetting controller", ChipID);
  if (this->_config->state != CONFIG_READY)
  {
    return false;
  }
  // Reset the dirty bits and wipe the shadow registers
  size_t i;
  for (i = 0; i < TMC5130_REGISTER_COUNT; i++)
  {
    this->_registerAccess[i] &= ~TMC_ACCESS_DIRTY;
    this->_config->shadowRegister[i] = 0;
  }

  this->_config->state = CONFIG_RESET;
  this->_config->configIndex = 0;

  bool result = true;
  log_info("TMC5130 #%d: Reset %s", ChipID, result ? "successful" : "failed");
  return true;
}

/**
 * Initializes the TMC5130 motor controller with the specified configuration and
 * register reset states. This function sets the initial motor state to idle,
 * prepares the configuration structure, and populates the shadow register and
 * access states for the motor controller. It is designed to prepare the motor
 * driver for operation and ensure a consistent startup state.
 *
 * @param config A pointer to the ConfigurationTypeDef structure that holds the
 * configuration data for the motor controller.
 * @param registerResetState A pointer to an array of int32_t values
 * representing the default reset state for each register, used to initialize
 * shadow registers.
 */
void TMC5130::Init(ConfigurationTypeDef *config,
                   const int32_t *registerResetState)
{
  this->LastStoppedPosition = 0;
  this->MotorState = msIdle;
  this->_config = config;
  this->_config->configIndex = 0;
  this->_config->state = CONFIG_READY;

  size_t i;
  for (i = 0; i < TMC5130_REGISTER_COUNT; i++)
  {
    this->_registerAccess[i] = tmc5130_defaultRegisterAccess[i];
    this->_registerResetState[i] = registerResetState[i];
  }
  log_info("TMC5130 #%d: Initializing motor controller", ChipID);
}

void TMC5130::_writeDatagram(uint8_t address, uint8_t x1, uint8_t x2,
                             uint8_t x3, uint8_t x4)
{
  uint8_t write_address = address | TMC5130_WRITE_BIT;
  uint8_t data[5] = {write_address, x1, x2, x3, x4};
  this->_SPIAccess->ReadWriteSPI(&data[0], 5, this->ChipID, SPIMode::Mode3);
  int32_t value = (x1 << 24) | (x2 << 16) | (x3 << 8) | x4;
  // Write to the shadow register and mark the register dirty
  address = TMC_ADDRESS(address);
  this->_config->shadowRegister[address] = value;
  this->_registerAccess[address] |= TMC_ACCESS_DIRTY;
}

int32_t TMC5130::_readInt(uint8_t address)
{
  address = TMC_ADDRESS(address);

  // register not readable -> shadow register copy
  if (!TMC_IS_READABLE(this->_registerAccess[address]))
    return this->_config->shadowRegister[address];

  uint8_t data[5] = {0, 0, 0, 0, 0};

  data[0] = address;
  this->_SPIAccess->ReadWriteSPI(&data[0], 5, this->ChipID, SPIMode::Mode3);
  data[0] = address;
  this->_SPIAccess->ReadWriteSPI(&data[0], 5, this->ChipID, SPIMode::Mode3);

  return (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];
}

void TMC5130::_writeInt(uint8_t address, int32_t value)
{
  _writeDatagram(address, BYTE(value, 3), BYTE(value, 2), BYTE(value, 1),
                 BYTE(value, 0));
}

#define ONE_OVER_SQRT_OF_TWO 0.70711f

inline float calculate_full_scale_current(float senseResistorValue, float Vfs)
{
  return ((Vfs / (senseResistorValue + 0.02)) * ONE_OVER_SQRT_OF_TWO) * 1000.0;
}

inline uint16_t CalculateDigitalCurrent(uint16_t currentInmA,
                                        float senseResistorValue, float Vfs)
{
  float fullScaleCurrentInmA = calculate_full_scale_current(senseResistorValue, Vfs);
  return (uint16_t)roundf((currentInmA * 32) / fullScaleCurrentInmA) - 1;
}

inline float CalculateAnalogCurrentVoltage(uint16_t currentInmA,
                                           float senseResistorValue, float Vfs)
{
  float fullScaleCurrentInmA = calculate_full_scale_current(senseResistorValue, Vfs);
  return (2.5 / fullScaleCurrentInmA) * currentInmA;
}

bool TMC5130::_activate_current(uint16_t current_in_mA)
{
  if (this->SenseResistorInOhms == 0)
  {
    log_error("Sense Resistor is not defined");
    return false;
  }

  int32_t chopConf = _readInt(TMC5130_CHOPCONF);
  float Vfs = 0.32;
  if ((chopConf & TMC5130_VSENSE_MASK) > 0)
  {
    Vfs = 0.18;
  }

  if (this->CurrentSetVoltageChannel)
  {
    this->CurrentSetVoltageChannel->WriteDACVoltage(
        CalculateAnalogCurrentVoltage(current_in_mA,
                                      SenseResistorInOhms, Vfs));
  }
  else
  {
    int32_t value =
        4 << 16; // 4 is a good IHOLDDELAY value for most cases, this will do a
                 // smooth transition from move to idle current
    value |= (CalculateDigitalCurrent(current_in_mA,
                                      SenseResistorInOhms, Vfs)
              << 8);
    value |= CalculateDigitalCurrent(Motor_Idle_CurrentInmA,
                                     SenseResistorInOhms, Vfs);
    _writeInt(TMC5130_IHOLD_IRUN, value);
  }
  return true;
}
void TMC5130::_activateIdleCurrent()
{
  _activate_current(Motor_Idle_CurrentInmA);
  log_info("Motor Current Set to %d mA", Motor_Idle_CurrentInmA);
}

void TMC5130::_activateRampUpCurrent()
{
  _activate_current(Motor_RampUp_CurrentInmA);
  log_info("Motor Current Set to %d mA", Motor_RampUp_CurrentInmA);
}

void TMC5130::_activateMoveCurrent()
{
  _activate_current(Motor_FullSpeed_CurrentInmA);
  log_info("Motor Current Set to %d mA", Motor_FullSpeed_CurrentInmA);
}

} // namespace LowLevelEmbedded::Devices::MotorControllers
