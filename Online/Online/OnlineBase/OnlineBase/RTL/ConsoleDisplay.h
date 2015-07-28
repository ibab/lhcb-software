#ifndef ONLINEKERNEL_RTL_CONSOLE_DISPLAY_H
#define ONLINEKERNEL_RTL_CONSOLE_DISPLAY_H
#include "SCR/scr.h"

namespace RTL {

  /** @class ConsoleDisplay ConsoleDisplay.h RTL/ConsoleDisplay.h
   *
   */
  class ConsoleDisplay  {
  protected:
    enum { NORMAL = SCR::NORMAL, REVERSE=SCR::INVERSE, INVERSE=SCR::INVERSE };
    SCR::Pasteboard* m_pasteboard;
    SCR::Display*    m_display;
    size_t           m_currLine;
    bool             m_continue;
    int              m_width, m_height;
  public:
    explicit ConsoleDisplay(const char* title = "");
    virtual ~ConsoleDisplay();
    int term_width()    const    {    return m_width;     }
    int term_height()   const    {    return m_height;    }
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
