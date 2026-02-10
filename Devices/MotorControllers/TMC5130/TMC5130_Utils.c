//
// Created by Jeroen Vandezande on 24/02/2021.
//

#include <stdint.h>
#include <math.h>
#include "TMC5130_Utils.h"

#define CONTROLLERCLOCK 16000000.0
#define POW2_24 (1u  << 24)
#define POW2_41 (1ULL << 41)
#define VELOCITY_TIME_REFERENCE POW2_24 / CONTROLLERCLOCK
#define ACC_DEC_TIME_REFERENCE POW2_41 / (CONTROLLERCLOCK * CONTROLLERCLOCK)


inline int32_t SpeedPPSToMotorUnits(const int32_t aPPS)
{
	return (int32_t) round((float)aPPS * VELOCITY_TIME_REFERENCE);
}

inline int32_t AccDecPPSToMotorUnits(const int32_t aPPS)
{
	return (int32_t) round((float)aPPS * ACC_DEC_TIME_REFERENCE);
}

inline int32_t RPMToPPS(const float aRPM, const uint32_t stepsPerRevolution)
{
  float rps = aRPM / 60.0; //rotations per second
  float pps = rps * stepsPerRevolution; //steps per round
  return pps;
}

inline int32_t RPMToTStep(const float aRPM, const uint32_t stepsPerRevolution)
{
	float pps = RPMToPPS(aRPM, stepsPerRevolution); //steps per round
	return SpeedPPSToMotorUnits(pps);
}

inline uint32_t mSToMotorUnits(const uint16_t aTimeInmS)
{
	return (CONTROLLERCLOCK / 1000) * aTimeInmS;
}