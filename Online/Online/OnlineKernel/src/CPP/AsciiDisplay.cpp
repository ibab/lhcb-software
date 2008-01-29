/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                  ASCII GRAPHICS DISPLAY                               */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#include <cstdlib>
#include "CPP/AsciiDisplay.h"

#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

void AsciiDisplay::setup_window() {
  m_window = initscreen();
  _setcursortype(_NOCURSOR);      // hide the cursor
  textcolor(YELLOW);              // change textcolor to YELLOW
  textbackground(BLUE);           // change backgroundcolor to BLUE
}

void AsciiDisplay::reset_window() {
  _setcursortype(_NORMALCURSOR);
}

size_t AsciiDisplay::draw_line()  {
  print_char(1, m_currLine, TEE_LEFT);
  for(size_t i=1; i < TERM_WIDTH-1; ++i)
    print_char(i+1, m_currLine, HORZ_BAR);
  print_char(TERM_WIDTH, m_currLine, TEE_RIGHT);
  return ++m_currLine;
}

size_t AsciiDisplay::draw_bar()  {
  print_char(1, m_currLine, VERT_BAR);
  for(size_t i=1; i < TERM_WIDTH-1; ++i)
    print_char(i+1, m_currLine, FAT_VERT_BAR);
  print_char(TERM_WIDTH, m_currLine, VERT_BAR);      
  return ++m_currLine;
}

size_t AsciiDisplay::draw_line_normal(const char* format,...)  {
  int flags = NORMAL;
  va_list args;
  char buffer[1024];
  va_start( args, format );
  size_t len = ::vsprintf(buffer, format, args);
  print_char(1, m_currLine, VERT_BAR);
  for(size_t j=0; j<len && j<TERM_WIDTH-1; ++j)
    print_char(j+2, m_currLine, flags|buffer[j]);
  for(size_t i=len; i < TERM_WIDTH-1; ++i)
    print_char(i+2, m_currLine, ' '|flags);
  print_char(TERM_WIDTH, m_currLine, VERT_BAR);
  return ++m_currLine;
}

size_t AsciiDisplay::draw_line_reverse(const char* format,...)  {
  int flags = REVERSE;
  va_list args;
  char buffer[1024];
  va_start( args, format );
  size_t len = ::vsprintf(buffer, format, args);
  print_char(1, m_currLine, VERT_BAR);
  for(size_t j=0; j<len && j<TERM_WIDTH-1; ++j)
    print_char(j+2, m_currLine, flags|buffer[j]);
  for(size_t i=len; i < TERM_WIDTH-1; ++i)
    print_char(i+2, m_currLine, ' '|flags);
  print_char(TERM_WIDTH, m_currLine, VERT_BAR);
  return ++m_currLine;
}

size_t AsciiDisplay::draw_line_bold(const char* format,...)  {
  int flags = BOLD;
  va_list args;
  char buffer[1024];
  va_start( args, format );
  size_t len = ::vsprintf(buffer, format, args);
  print_char(1, m_currLine, VERT_BAR);
  for(size_t j=0; j<len && j<TERM_WIDTH-1; ++j)
    print_char(j+2, m_currLine, flags|buffer[j]);
  for(size_t i=len; i < TERM_WIDTH-1; ++i)
    print_char(i+2, m_currLine, ' '|flags);
  print_char(TERM_WIDTH, m_currLine, VERT_BAR);
  return ++m_currLine;
}

size_t AsciiDisplay::draw_bar(int x, int y, float f1, int scale)  {
  for (int i=0, m=scale; i<m; ++i)  {
    print_char(i+x,y,f1*m > i ? FAT_VERT_BAR : DIM_VERT_BAR);
  }
  return 1;
}

void AsciiDisplay::begin_update()  {
  ::textcolor(YELLOW);
  m_currLine = 1;
  print_char(1,m_currLine,LEFT_UP_EDGE);
  for(size_t i=1; i < TERM_WIDTH-1; ++i)
    print_char(i+1,m_currLine,HORZ_BAR);
  print_char(TERM_WIDTH,m_currLine,RIGHT_UP_EDGE);      
  m_currLine = 2;
}

void AsciiDisplay::end_update() { 
  while(m_currLine<TERM_HEIGHT-1) {
    //char buffer[1024];
    //sprintf(buffer," Line:%d total:%d ------",m_currLine, TERM_HEIGHT);
    //draw_line(NORMAL,buffer);
    draw_line_normal("");
  }
  print_char(1,m_currLine,LEFT_LOW_EDGE);
  for(size_t i=1; i < TERM_WIDTH-1; ++i)
    print_char(i+1,m_currLine,HORZ_BAR);
  print_char(TERM_WIDTH,m_currLine,RIGHT_LOW_EDGE);      
  m_currLine++;
  refresh();
}

size_t AsciiDisplay::width() const {
  int width = TERM_WIDTH;
  return width;
}

size_t AsciiDisplay::height() const {
  int height = TERM_HEIGHT;
  return height;
}

AsciiDisplay::AsciiDisplay() 
  : MonitorDisplay(), m_window(0), m_color(YELLOW)  
{
}
