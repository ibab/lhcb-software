#ifndef ONLINEKERNEL_RTL_CONSOLE_DISPLAY_H
#define ONLINEKERNEL_RTL_CONSOLE_DISPLAY_H
#include "RTL/screen.h"

namespace RTL {

  /** @class ConsoleDisplay ConsoleDisplay.h RTL/ConsoleDisplay.h
    *
    */
  class ConsoleDisplay  {
  protected:
    WINDOW* m_win;
    size_t m_currLine;
    int m_textColor;
    int m_bkgColor;
    bool m_continue;
  public:
    ConsoleDisplay();
    virtual ~ConsoleDisplay();
    virtual size_t draw_line();
    virtual size_t draw_line(int flags, const char* format,...);
    virtual void begin_update();
    virtual void end_update();
    virtual void put_info() = 0;
    virtual void run(int millsecs_to_sleep);
    virtual bool stop() const     { return !m_continue; }
    void setContinuous(bool val)  { m_continue = val;  }
  };
}
#endif // ONLINEKERNEL_RTL_CONSOLE_DISPLAY_H
