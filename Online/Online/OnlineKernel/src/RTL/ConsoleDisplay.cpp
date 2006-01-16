#include <cstdio>
#include <cstring>
#include <cstdarg>
#include "RTL/rtl.h"
#include "RTL/ConsoleDisplay.h"

RTL::ConsoleDisplay::ConsoleDisplay() : m_currLine(0), m_continue(true) {
  m_bkgColor = BLUE;
  m_textColor = YELLOW;
  m_win = initscreen();
  _setcursortype(_NOCURSOR);            // hide the cursor
  textcolor(m_textColor);               // change textcolor to YELLOW
  textbackground(m_bkgColor);           // change backgroundcolor to BLUE
}

RTL::ConsoleDisplay::~ConsoleDisplay()  {
}

size_t RTL::ConsoleDisplay::draw_line(int flags, const char* format,...)  {
  va_list args;
  char buffer[1024];
  va_start( args, format );
  size_t len = ::vsprintf(buffer, format, args);
  print_char(1, m_currLine, VERT_BAR);
  for(size_t j=0; j<len && j<term_width()-1; ++j)
    print_char(j+2, m_currLine, flags|buffer[j]);
  for(size_t i=len; i < term_width()-1; ++i)
    print_char(i+2, m_currLine, ' '|flags);
  print_char(term_width(), m_currLine, VERT_BAR);
  return ++m_currLine;
}
size_t RTL::ConsoleDisplay::draw_line()  {
  print_char(1, m_currLine, TEE_LEFT);
  for(size_t i=1; i < term_width()-1; ++i)
    print_char(i+1, m_currLine, HORZ_BAR);
  print_char(term_width(), m_currLine, TEE_RIGHT);
  return ++m_currLine;
}

void RTL::ConsoleDisplay::begin_update()  {
  ::textcolor(m_textColor);
  m_currLine = 1;
  print_char(1,m_currLine,LEFT_UP_EDGE);
  for(size_t i=1; i < term_width()-1; ++i)
    print_char(i+1,m_currLine,HORZ_BAR);
  print_char(term_width(),m_currLine,RIGHT_UP_EDGE);      
  m_currLine = 2;
}

void RTL::ConsoleDisplay::end_update() {
  print_char(1,m_currLine,LEFT_LOW_EDGE);
  for(size_t i=1; i < term_width()-1; ++i)
    print_char(i+1,m_currLine,HORZ_BAR);
  print_char(term_width(),m_currLine,RIGHT_LOW_EDGE);      
  m_currLine++;
  refresh();
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

extern "C" int rtl_reset_terminal(int, char**)  {
  initscreen();
  _setcursortype(_NORMALCURSOR);
  textbackground(BLACK); 
  textcolor(LIGHTGREEN);
  refresh();
  clrscr();
  refresh();
  return 1;
}
