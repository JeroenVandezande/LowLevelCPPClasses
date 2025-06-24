/*
 * Config.h
 *
 *  Created on: 13.06.2018
 *      Author: LK
 */

#ifndef TMC_HELPERS_CONFIG_H_
#define TMC_HELPERS_CONFIG_H_

#include "TMC5130_Constants.h"
#include "TMC5130_Types.h"

// Callback functions have IC-dependent parameters
// To store the function pointers we use this dummy type, which is never
// called without casting it to the IC-specific type first.
// (Casting between function pointers is allowed by the C standard)
typedef void (*tmc_callback_config)(void);

// States of a configuration
typedef enum {
	CONFIG_READY,
	CONFIG_RESET,
	CONFIG_RESTORE
} ConfigState;

#endif /* TMC_HELPERS_CONFIG_H_ */
