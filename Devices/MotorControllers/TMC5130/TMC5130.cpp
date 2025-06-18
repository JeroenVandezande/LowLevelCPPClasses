#include "TMC5130.h"
#include "TMC5130_Bits.h"
#include "TMC5130_Fields.h"
#include "TMC5130_Macros.h"
#include "TMC5130_Register.h"
#include "TMC5130_RegisterAccess.h"

namespace LowLevelEmbedded::Devices::MotorControllers
{
void TMC5130::_writeConfiguration()
{
  uint8_t ptr = this->_config->configIndex;
  const int32_t *settings;

  if (this->_config->state == CONFIG_RESTORE)
  {
    settings = this->_config->shadowRegister;
    // Find the next restorable register
    while ((ptr < TMC5130_REGISTER_COUNT) &&
           !TMC_IS_RESTORABLE(this->_registerAccess[ptr]))
    {
      ptr++;
    }
  }
  else
  {
    settings = this->_registerResetState;
    // Find the next resettable register
    while ((ptr < TMC5130_REGISTER_COUNT) &&
           !TMC_IS_RESETTABLE(this->_registerAccess[ptr]))
    {
      ptr++;
    }
  }

  if (ptr < TMC5130_REGISTER_COUNT)
  {
    _writeInt(ptr, settings[ptr]);
    ptr++;
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
  case msShaking:
    if (rampstatus & TMC5130_RS_VZERO_MASK) // check if speed is zero.
    {
      volatile int32_t timeDelta = elapsedTimeinMs - this->_lastStartTimeInms;
      if (timeDelta > 1000)
        timeDelta = 1;
      float timeError = timeDelta - this->_targetShakeSpeedInms;
      int32_t speedAdjust = (int32_t)(timeError * this->_shakerIValue);
      if (abs(speedAdjust) > 2)
      {
        int newspeed = this->_lastshakespeed + speedAdjust;
        if (newspeed > 0)
        {
          _writeInt(TMC5130_VMAX, newspeed);
          this->_lastshakespeed = newspeed;
        }
      }

      if (this->_IsInShakePositionA)
      {
        _writeInt(TMC5130_XTARGET, this->_ShakePositionB);
        this->_IsInShakePositionA = false;
      }
      else
      {
        _writeInt(TMC5130_XTARGET, this->_ShakePositionA);
        this->_IsInShakePositionA = true;
      }
      this->_lastStartTimeInms = elapsedTimeinMs;
      this->MotorState = msShakingRamp;
    }
    break;
  case msShakingRamp:
    if (!(rampstatus & TMC5130_RS_VZERO_MASK)) // check if speed is NOT zero
    {
      this->MotorState = msShaking;
      this->_lastStartTimeInms = elapsedTimeinMs;
    }
    break;
  case msIdle:
    break;
  case msStopped:
    _writeInt(TMC5130_TCOOLTHRS, 0);
    this->MotorState = msIdle;
    this->LastStoppedPosition = xactual;
    this->IntReason = irStopped;
    if (this->SetInterruptCallback) // check if callback was assigned
    {
      this->SetInterruptCallback(*this);
    }
    break;
  case msStalled:
    this->IntReason = irStalled;
    if (this->SetInterruptCallback) // check if callback was assigned
    {
      this->SetInterruptCallback(*this);
    }
    this->MotorState = msIdle;
    break;
  case msConstantVelocity:

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
  case msShakingHoming:
    if (rampstatus & TMC5130_RS_VZERO_MASK) // check if speed is zero.
    {
      _writeInt(TMC5130_VMAX, 4000);
      _writeInt(TMC5130_XTARGET, this->LastStoppedPosition);
      this->MotorState = msShakingStopping;
    }
    break;
  case msShakingStopping:
    if (rampstatus & TMC5130_RS_VZERO_MASK) // check if speed is zero.
    {
      this->MotorState = msIdle;
      this->IntReason = irStopped;
      if (this->SetInterruptCallback) // check if callback was assigned
      {
        this->SetInterruptCallback(*this);
      }
    }
    break;
  case msTimedConstantVelocityRampUp:
    if (rampstatus & TMC5130_RS_VELREACHED_MASK) // check if speed is reached.
    {
      this->MotorState = msTimedConstantVelocity;
      int32_t drv_stat = _readInt(TMC5130_DRVSTATUS);
      this->_Prev_SG_Result = (drv_stat & TMC5130_SG_RESULT_MASK);
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
    else
    {
      int32_t drv_stat = _readInt(TMC5130_DRVSTATUS);
      uint16_t sg_result = (drv_stat & TMC5130_SG_RESULT_MASK);
      uint16_t sg_result_prev = this->_Prev_SG_Result;
      if (sg_result < (sg_result_prev / this->StallguardDivider))
      {
        _writeInt(TMC5130_AMAX, 65535);
        _writeInt(TMC5130_VMAX, 0);
        this->MotorState = msStopped;
      }
      else
      {
        if (sg_result > sg_result_prev)
        {
          this->_Prev_SG_Result = sg_result;
        }
      }
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
    0x03, 0x21, 0x01, 0x02, 0x13, 0x02, ____, ____,
    ____, ____, ____, ____, ____, ____, ____, ____, // 0x00 - 0x0F
    0x02, 0x02, 0x01, 0x02, 0x02, 0x02, ____, ____,
    ____, ____, ____, ____, ____, ____, ____, ____, // 0x10 - 0x1F
    0x03, 0x03, 0x01, 0x06, 0x02, 0x02, 0x02, 0x02,
    0x02, ____, 0x02, 0x06, 0x02, 0x03, ____, ____, // 0x20 - 0x2F
    ____, ____, ____, 0x02, 0x03, 0x21, 0x01, ____,
    0x03, 0x03, 0x02, 0x21, 0x01, ____, ____, ____, // 0x30 - 0x3F
    ____, ____, ____, ____, ____, ____, ____, ____,
    ____, ____, ____, ____, ____, ____, ____, ____, // 0x40 - 0x4F
    ____, ____, ____, ____, ____, ____, ____, ____,
    ____, ____, ____, ____, ____, ____, ____, ____, // 0x50 - 0x5F
    0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x01, 0x01, 0x03, 0x02, 0x02, 0x01, // 0x60 - 0x6F
    0x42, 0x01, 0x02, 0x01, ____, ____, ____, ____,
    ____, ____, ____, ____, ____, ____, ____, ____ // 0x70 - 0x7F
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

bool TMC5130::Reset()
{
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

  return true;
}

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
  this->_IsInShakePositionA = false;
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

} // namespace LowLevelEmbedded::Devices::MotorControllers