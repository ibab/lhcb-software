#ifndef ONLINE_ONLINEKERNEL_MBM_MONITOR_H
#define ONLINE_ONLINEKERNEL_MBM_MONITOR_H

#include <cstdarg>
#include "MBM/bmstruct.h"

namespace MBM {

  struct DisplayDescriptor;

  class Monitor {
  protected:
    DisplayDescriptor* m_bms;
    int                m_numBM;
    size_t             m_currLine;
    char               m_buffID[32];
    char*              m_bmid;
    lib_rtl_gbl_t      m_bm_all;
    BUFFERS*           m_buffers;
  public:
    virtual void setup_window() = 0;
    virtual void reset_window() = 0;
    virtual void begin_update() = 0;
    virtual void end_update() = 0;
    virtual size_t display_width() const = 0;
    virtual size_t draw_line() = 0;
    virtual size_t draw_bar() = 0;
    virtual size_t draw_line_normal(const char* format,...) = 0;
    virtual size_t draw_line_reverse(const char* format,...) = 0;
    virtual size_t draw_bar(int x, int y, float ratio,int full_scale) = 0;

    virtual int monitor();
    virtual int initMonitor();
    virtual int updateDisplay();
    virtual int show_information();
    virtual size_t draw_buffer(const char* name, CONTROL* ctrl);
    virtual void getOptions(int argc, char** argv);
    virtual int optparse (const char* c);
    int get_bm_list();
    int drop_bm_list();
    static size_t print(void* ctxt, const char* format, va_list args);
    Monitor(int argc , char** argv);
    virtual ~Monitor();
  };
}

#endif // ONLINE_ONLINEKERNEL_MBM_MONITOR_H
