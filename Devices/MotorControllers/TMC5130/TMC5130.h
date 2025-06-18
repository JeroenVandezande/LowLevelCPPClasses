//
// Created by Jeroe on 2025-06-18.
//

#ifndef TMC5130_H

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
  msShaking,
  msShakingRamp,
  msShakingHoming,
  msShakingStopping,
  msTimedConstantVelocity,
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
  std::function<void(TMC5130 &, MotorIntReason_t)> SetInterruptCallback;
  std::function<bool(TMC5130 &)> GetCustomHomeSwitchActuatedCallback;

  uint8_t ChipID;
  uint16_t IdleMotorCurrent;
  uint16_t MoveMotorCurrent;
  uint16_t ShakeMotorCurrent;
  MotorIntReason_t IntReason;
  MotorState_t MotorState;
  int32_t LastStoppedPosition;
  int32_t LastTargetPosition;
  uint16 MaxMotorCurrent;
  float StallguardDivider;

  // Initialize a TMC5130 IC.
  // This function requires:
  //     - config: A ConfigurationTypeDef struct, which will be used by the IC
  //     - registerResetState: An int32_t array with 128 elements. This holds
  //     the values to be used for a reset.
  void Init(ConfigurationTypeDef *config, const int32_t *registerResetState);

  // Reset the TMC5130.
  bool Reset();

private:
  ISPIAccess *_SPIAccess;
  bool _IsInShakePositionA;
  uint8_t _stopSwitchID;
  bool _stopSwitchInverted;
  int32_t _ShakePositionA;
  int32_t _ShakePositionB;
  int32_t _targetShakeSpeedInms;
  int32_t _lastStartTimeInms;
  int32_t _lastshakespeed;
  int32_t _targetMoveTimeInms;
  int32_t _IHOLD_IRUN_NORMAL;
  int32_t _IHOLD_IRUN_SHAKER;
  uint16_t _Prev_SG_Result;
  float _shakerIValue;
  ConfigurationTypeDef *_config = nullptr;
  int32_t _registerResetState[TMC5130_REGISTER_COUNT] = {};
  uint8_t _registerAccess[TMC5130_REGISTER_COUNT] = {};

  void _writeDatagram(uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3,
                      uint8_t x4);
  void _writeInt(uint8_t address, int32_t value);
  int32_t _readInt(uint8_t address);
};

} // namespace LowLevelEmbedded::Devices::MotorControllers

#endif
