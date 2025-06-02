//
// Created by Jeroe on 2025-06-02.
//

#pragma once

#include <plog/Appenders/IAppender.h>
#include <plog/Formatters/FuncMessageFormatter.h>
#include "Segger_RTT/SEGGER_RTT.h"

namespace plog
{

template <class Formatter> class AnsiColorRTTAppender : public IAppender
{
public:
  virtual void write(const Record &record) PLOG_OVERRIDE
  {
    setColor(record.getSeverity());
    SEGGER_RTT_WriteString(0, Formatter::format(record).c_str());
    resetColor();
  }

protected:
  void resetColor() { SEGGER_RTT_WriteString(0, "\x1B[0m\x1B[0K"); }

  void setColor(Severity severity)
  {
    switch (severity)
    {
    case fatal:
      SEGGER_RTT_WriteString(0, "\x1B[97m\x1B[41m"); // white on red background
      break;

    case error:
      SEGGER_RTT_WriteString(0, "\x1B[91m"); // red
      break;

    case warning:
      SEGGER_RTT_WriteString(0, "\x1B[93m"); // yellow
      break;

    case info:
      SEGGER_RTT_WriteString(0, "\x1B[92m"); // green
      break;

    case debug:
    case verbose:
      SEGGER_RTT_WriteString(0, "\x1B[96m"); // cyan
      break;

    default:
      break;
    }
  }
};

template <class Formatter> class TextRTTAppender : public IAppender
{
public:
  virtual void write(const Record &record) PLOG_OVERRIDE
  {
    SEGGER_RTT_WriteString(0, Formatter::format(record).c_str());
  }

};
} // namespace plog
