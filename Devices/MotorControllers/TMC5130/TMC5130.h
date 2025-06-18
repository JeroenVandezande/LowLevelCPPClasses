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
  static const int STOPSWITCH0MASK = 1;
  static const int STOPSWITCH1MASK = 2;

  std::function<void(TMC5130 &)> SetInterruptCallback;
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

  /**
   * Performs periodic maintenance and control updates for the TMC5130 motor
   * driver. This function must be called regularly to handle tasks such as
   * state management and trajectory updates.
   *
   * @param elapsedTimeinMs The time in milliseconds since the last invocation
   * of the periodic job.
   */
  void PeriodicJob(uint32_t elapsedTimeinMs);

  /// Moves the motor for a set period of time
  /// \param acc Unsigned Acceleration Value
  /// \param velocity Unsigned Velocity Max. Range +-(2^23-512)
  /// \param timeInms Time in ms the motor should move
  /// \param ticksInms Current tick value from the Systick timer or an equivalent 1ms timer
  void StartTimedConstantVelocity(uint32_t acc, int32_t velocity, uint32_t timeInms, uint32_t ticksInms);


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
  void _writeConfiguration();
};

} // namespace LowLevelEmbedded::Devices::MotorControllers

#endif
