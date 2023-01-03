//
// Created by Jeroen Vandezande on 09/12/2022.
//

#include "SerialLeds.h"

namespace LowLevelEmbedded
{
	namespace Devices
	{
		namespace LedControllers
		{

#define IN_PI42TAS_FillBuffer(COLOR)\
{\
    uint8_t bits = COLOR;\
    for (int pos = 7 ; pos >= 0 ; pos--) \
    {\
        if (bits & (1 << pos)) \
        {\
            *Ptr++ = LOGIC_LED_1;   \
        } \
        else \
        {                           \
            *Ptr++ = LOGIC_LED_0;\
        }\
    }\
}\

		}
	}
} // LowLevelEmbedded