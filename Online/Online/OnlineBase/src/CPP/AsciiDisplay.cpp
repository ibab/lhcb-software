/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                  ASCII GRAPHICS DISPLAY                               */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#include <cstdio>
#include <cstdlib>
#include "CPP/AsciiDisplay.h"

#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

using namespace SCR;

void AsciiDisplay::setup_window() {
#if 0
  m_window = initscreen();
  _setcursortype(_NOCURSOR);      // hide the cursor
  textcolor(YELLOW);              // change textcolor to YELLOW
  textbackground(BLUE);           // change backgroundcolor to BLUE
#endif
  ::scrc_create_pasteboard (&m_pasteboard, 0, &m_height, &m_width);
  ::scrc_create_display (&m_display, m_height-2, m_width-2, NORMAL, ON, m_title.c_str());
  ::scrc_paste_display  (m_display, m_pasteboard, 2, 2);
  ::scrc_end_pasteboard_update(m_pasteboard);
  ::scrc_fflush(m_pasteboard);
  ::scrc_set_cursor(m_display, 2, 10);
  ::scrc_cursor_off(m_pasteboard);
  //bg_blue(m_pasteboard);
  //fg_yellow(m_pasteboard);
}

void AsciiDisplay::reset_window() {
  //_setcursortype(_NORMALCURSOR);
}

size_t AsciiDisplay::draw_line()  {
#if 0
  print_char(1, m_currLine, TEE_LEFT);
  for(size_t i=1; i < TERM_WIDTH-1; ++i)
    print_char(i+1, m_currLine, HORZ_BAR);
  print_char(TERM_WIDTH, m_currLine, TEE_RIGHT);
  return ++m_currLine;
#endif
  for(int i=0; i < m_width-1; ++i)
    ::scrc_put_chars(m_display,"_", NORMAL, m_currLine, i+1, 0);
  return ++m_currLine;
}

size_t AsciiDisplay::draw_bar()  {
#if 0
  print_char(1, m_currLine, VERT_BAR);
  for(size_t i=1; i < TERM_WIDTH-1; ++i)
    print_char(i+1, m_currLine, FAT_VERT_BAR);
  print_char(TERM_WIDTH, m_currLine, VERT_BAR);      
  return ++m_currLine;
#endif
  for(int i=0; i < m_width-1; ++i)
    ::scrc_put_chars(m_display," ", INVERSE, m_currLine, i+1, 0);
  return ++m_currLine;
}

size_t AsciiDisplay::draw_line_normal(const char* format,...)  {
  int flags = NORMAL;
  va_list args;
  char buffer[1024];
  va_start( args, format );
  /* size_t len = */ ::vsprintf(buffer, format, args);
  va_end(args);
#if 0
  print_char(1, m_currLine, VERT_BAR);
  for(size_t j=0; j<len && j<TERM_WIDTH-1; ++j)
    print_char(j+2, m_currLine, flags|buffer[j]);
  for(size_t i=len; i < TERM_WIDTH-1; ++i)
    print_char(i+2, m_currLine, ' '|flags);
  print_char(TERM_WIDTH, m_currLine, VERT_BAR);
#endif
  ::scrc_put_chars(m_display,buffer,flags,m_currLine,1,1);
  return ++m_currLine;
}

size_t AsciiDisplay::draw_line_reverse(const char* format,...)  {
  //int flags = REVERSE;
  va_list args;
  char buffer[1024];
  va_start( args, format );
  /* size_t len = */ ::vsprintf(buffer, format, args);
  va_end(args);
#if 0
  print_char(1, m_currLine, VERT_BAR);
  for(size_t j=0; j<len && j<TERM_WIDTH-1; ++j)
    print_char(j+2, m_currLine, flags|buffer[j]);
  for(size_t i=len; i < TERM_WIDTH-1; ++i)
    print_char(i+2, m_currLine, ' '|flags);
  print_char(TERM_WIDTH, m_currLine, VERT_BAR);
#endif
  ::scrc_put_chars(m_display,buffer,INVERSE,m_currLine,1,1);
  return ++m_currLine;
}

size_t AsciiDisplay::draw_line_bold(const char* format,...)  {
  int flags = BOLD;
  va_list args;
  char buffer[1024];
  va_start( args, format );
  /* size_t len = */ ::vsprintf(buffer, format, args);
  va_end(args);
#if 0
  print_char(1, m_currLine, VERT_BAR);
  for(size_t j=0; j<len && j<TERM_WIDTH-1; ++j)
    print_char(j+2, m_currLine, flags|buffer[j]);
  for(size_t i=len; i < TERM_WIDTH-1; ++i)
    print_char(i+2, m_currLine, ' '|flags);
  print_char(TERM_WIDTH, m_currLine, VERT_BAR);
#endif
  ::scrc_put_chars(m_display,buffer,flags,m_currLine,1,1);
  return ++m_currLine;
}

size_t AsciiDisplay::draw_bar(int x, int y, float f1, int scale)  {
  for (int i=0, m=scale; i<m; ++i)  {
    const char* c = f1*m > i ? " " : ".";
    int flags = f1*m > i ? INVERSE : NORMAL;
    //print_char(i+x,y,f1*m > i ? FAT_VERT_BAR : DIM_VERT_BAR);
    ::scrc_put_chars(m_display,c,flags,y,i+x-1,0);
  }
  return 1;
}

void AsciiDisplay::begin_update()  {
#if 0
  ::textcolor(YELLOW);
  m_currLine = 0;
  print_char(1,m_currLine,LEFT_UP_EDGE);
  for(size_t i=1; i < TERM_WIDTH-1; ++i)
    print_char(i+1,m_currLine,HORZ_BAR);
  print_char(TERM_WIDTH,m_currLine,RIGHT_UP_EDGE);      
#endif
  ::scrc_begin_pasteboard_update(m_pasteboard);
  m_currLine = 1;
}

void AsciiDisplay::end_update() { 
  while(m_currLine<height()-1) {
    //char buffer[1024];
    //sprintf(buffer," Line:%d total:%d ------",m_currLine, TERM_HEIGHT);
    //draw_line(NORMAL,buffer);
    draw_line_normal("");
  }
#if 0
  print_char(1,m_currLine,LEFT_LOW_EDGE);
  for(size_t i=1; i < TERM_WIDTH-1; ++i)
    print_char(i+1,m_currLine,HORZ_BAR);
  print_char(TERM_WIDTH,m_currLine,RIGHT_LOW_EDGE);
#endif
  ::scrc_end_pasteboard_update(m_pasteboard);
  m_currLine++;
  //refresh();
}

size_t AsciiDisplay::width() const {
  //m_width = TERM_WIDTH;
  return m_width;
}

size_t AsciiDisplay::height() const {
  //m_height = TERM_HEIGHT;
  return m_height;
}

AsciiDisplay::AsciiDisplay() 
  : MonitorDisplay(), m_pasteboard(0), m_display(0), m_color(YELLOW), m_width(0), m_height(0)
{
}

/// Default Constructor with title
AsciiDisplay::AsciiDisplay(const std::string& title) 
  : MonitorDisplay(), m_pasteboard(0), m_display(0), m_color(YELLOW), m_width(0), m_height(0), m_title(title) 
{
}

