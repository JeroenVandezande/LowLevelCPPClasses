//
// Created by Jeroen Vandezande on 24/02/2021.
//

#include <stdint.h>
#include <math.h>
#include "TMC5130_Utils.h"

#define CONTROLLERCLOCK 16000000
#define VELOCITY_TIME_REFERENCE 1.048576
#define ACC_DEC_TIME_REFERENCE 0.00858993


inline uint32_t SpeedPPSToMotorUnits(uint32_t aPPS)
{
	return (uint32_t) round((float)aPPS * VELOCITY_TIME_REFERENCE);
}

inline uint32_t AccDecPPSToMotorUnits(uint32_t aPPS)
{
	return (uint32_t) round((float)aPPS * ACC_DEC_TIME_REFERENCE);
}

inline int32_t RPMToTStep(float aRPM)
{
	float rps = aRPM / 60.0; //rotations per second
	float pps = rps * STEPS_PER_REVOLUTION; //steps per round
	return SpeedPPSToMotorUnits(pps);
}

inline uint32_t mSToMotorUnits(uint16_t aTimeInmS)
{
	return (CONTROLLERCLOCK / 1000) * aTimeInmS;
}