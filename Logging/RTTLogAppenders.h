#ifndef LOWLEVELCPP_LOG_H_
#define LOWLEVELCPP_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ulog.h"
#include "SEGGER_RTT.h"

void MicrologAnsiColorRTTOutput_callback(ulog_Event *ev, void *arg);

#ifdef __cplusplus
}
#endif

#endif
