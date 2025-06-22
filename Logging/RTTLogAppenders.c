#include "RTTLogAppenders.h"
#include "SEGGER_RTT.h"
#include <ulog.h>

void MicrologAnsiColorRTTOutput_callback(ulog_Event *ev, void *arg)
{
  static char buffer[128];
  const int result = ulog_event_to_cstr(ev, buffer, sizeof(buffer));
  if (result == 0)
  {
    SEGGER_RTT_WriteString(0, buffer);
    SEGGER_RTT_WriteString(0, "\n");
  }
}