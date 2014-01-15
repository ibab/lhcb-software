#include <cstdio>
#include <cstring>
#include <cstdarg>
#include "RTL/rtl.h"
#include "RTL/ConsoleDisplay.h"

using namespace SCR;

RTL::ConsoleDisplay::ConsoleDisplay(const char* title) : m_currLine(0), m_continue(true) {
  int width, height;
  ::scrc_create_pasteboard (&m_pasteboard, 0, &height, &width);
  ::scrc_create_display (&m_display, height-2, width-2, SCR::NORMAL, SCR::ON, title);
  ::scrc_paste_display  (m_display, m_pasteboard, 2, 2);
  ::scrc_end_pasteboard_update(m_pasteboard);
  ::scrc_fflush(m_pasteboard);
  ::scrc_set_cursor(m_display, 2, 10);
  ::scrc_cursor_off(m_pasteboard);
}

RTL::ConsoleDisplay::~ConsoleDisplay()  {
}

size_t RTL::ConsoleDisplay::draw_line(int flags, const char* format,...)  {
  va_list args;
  char buffer[1024];
  va_start( args, format );
  /* size_t len = */ ::vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  ::scrc_put_chars(m_display,buffer,flags,m_currLine,1,1);
  return ++m_currLine;
}

size_t RTL::ConsoleDisplay::draw_line()  {
  for(int i=0; i < term_width()-1; ++i)
    scrc_put_chars(m_display,"_", NORMAL, m_currLine, i+1, 0);
  return ++m_currLine;
}

void RTL::ConsoleDisplay::begin_update()  {
  ::scrc_begin_pasteboard_update(m_pasteboard);
  m_currLine = 2;
}

void RTL::ConsoleDisplay::end_update() {
  ::scrc_end_pasteboard_update(m_pasteboard);
}

void RTL::ConsoleDisplay::run(int millsecs_to_sleep)  {
  while ( 1 )  {
    begin_update();
    put_info();
    end_update();
    if ( !stop() )  {
      lib_rtl_sleep(millsecs_to_sleep);
    }
    else  {
      break;
    }
  }
}
