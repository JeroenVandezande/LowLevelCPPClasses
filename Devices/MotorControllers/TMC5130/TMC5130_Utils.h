//
// Created by Jeroen Vandezande on 24/02/2021.
//

#ifndef _TMC5130_UTILS_H_
#define _TMC5130_UTILS_H_

#ifdef __cplusplus
extern "C"
{
#endif

  int32_t RPMToPPS(float aRPM, uint32_t stepsPerRevolution);
  int32_t RPMToTStep(float aRPM, uint32_t stepsPerRevolution);
  int32_t SpeedPPSToMotorUnits(int32_t aPPS);
  int32_t AccDecPPSToMotorUnits(int32_t aPPS);
  uint32_t mSToMotorUnits(uint16_t aTimeInmS);

#ifdef __cplusplus
}
#endif

#endif //_TMC5130_UTILS_H_
