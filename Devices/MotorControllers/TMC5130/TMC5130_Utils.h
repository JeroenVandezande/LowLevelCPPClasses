//
// Created by Jeroen Vandezande on 24/02/2021.
//

#ifndef _TMC5130_UTILS_H_
#define _TMC5130_UTILS_H_

#define STEPS_PER_REVOLUTION 51200

#ifdef __cplusplus
extern "C"
{
#endif

int32_t RPMToTStep(float aRPM);
uint32_t SpeedPPSToMotorUnits(uint32_t aPPS);
uint32_t AccDecPPSToMotorUnits(uint32_t aPPS);
uint32_t mSToMotorUnits(uint16_t aTimeInmS);

#ifdef __cplusplus
}
#endif

#endif //_TMC5130_UTILS_H_
