#include "RTTLogAppenders.h"
#include "SEGGER_RTT.h"
#include <ulog.h>

void resetColor() { SEGGER_RTT_WriteString(0, "\x1B[0m\x1B[0K"); }

void setColor(const int severity)
   {
     switch (severity)
     {
     case LOG_FATAL:
       SEGGER_RTT_WriteString(0, "\x1B[97m\x1B[41m"); // white on red background
       break;

     case LOG_ERROR:
       SEGGER_RTT_WriteString(0, "\x1B[91m"); // red
       break;

     case LOG_WARN:
       SEGGER_RTT_WriteString(0, "\x1B[93m"); // yellow
       break;

     case LOG_INFO:
       SEGGER_RTT_WriteString(0, "\x1B[92m"); // green
       break;

     case LOG_DEBUG:
     case LOG_TRACE:
       SEGGER_RTT_WriteString(0, "\x1B[96m"); // cyan
      break;

     default:
       break;
     }
   }

void MicrologAnsiColorRTTOutput_callback(ulog_Event *ev, void *arg)
{
  static char buffer[128];
  const int result = ulog_event_to_cstr(ev, buffer, sizeof(buffer));
  if (result == 0)
  {
    setColor(ev->level);
    SEGGER_RTT_WriteString(0, buffer);
    SEGGER_RTT_WriteString(0, "\n");
    resetColor();
  }
}