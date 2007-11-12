/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                  ASCII BUFFER MANAGER                                 */
/*                  INSTALLATION PROGRAM FOR THE AEB                     */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#include <cstdlib>
#include <cstdarg>
#include "RTL/screen.h"
#include "MBM/Monitor.h"

#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

namespace MBM {

  struct AsciiMonitor : public Monitor {
    WINDOW*          m_window;
    int              m_color;

    virtual void setup_window() {
      m_window = initscreen();
      _setcursortype(_NOCURSOR);      // hide the cursor
      textcolor(YELLOW);              // change textcolor to YELLOW
      textbackground(BLUE);           // change backgroundcolor to BLUE
    }
    virtual void reset_window() {
      _setcursortype(_NORMALCURSOR);
    }

    virtual size_t draw_line()  {
      print_char(1, m_currLine, TEE_LEFT);
      for(size_t i=1; i < TERM_WIDTH-1; ++i)
        print_char(i+1, m_currLine, HORZ_BAR);
      print_char(TERM_WIDTH, m_currLine, TEE_RIGHT);
      return ++m_currLine;
    }

    virtual size_t draw_bar()  {
      print_char(1, m_currLine, VERT_BAR);
      for(size_t i=1; i < TERM_WIDTH-1; ++i)
        print_char(i+1, m_currLine, FAT_VERT_BAR);
      print_char(TERM_WIDTH, m_currLine, VERT_BAR);      
      return ++m_currLine;
    }

    virtual size_t draw_line_normal(const char* format,...)  {
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

    virtual size_t draw_line_reverse(const char* format,...)  {
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

    virtual size_t draw_bar(int x, int y, float f1, int scale)  {
      for (int i=0, m=scale; i<m; ++i)  {
	print_char(i+x,y,f1*m > i ? FAT_VERT_BAR : DIM_VERT_BAR);
      }
      return 1;
    }

    virtual void begin_update()  {
      ::textcolor(YELLOW);
      m_currLine = 1;
      print_char(1,m_currLine,LEFT_UP_EDGE);
      for(size_t i=1; i < TERM_WIDTH-1; ++i)
        print_char(i+1,m_currLine,HORZ_BAR);
      print_char(TERM_WIDTH,m_currLine,RIGHT_UP_EDGE);      
      m_currLine = 2;
    }
    virtual void end_update() { 
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
    virtual size_t display_width() const {
      return TERM_WIDTH;
    }
    AsciiMonitor(int argc , char** argv) 
      : Monitor(argc,argv), m_window(0), m_color(YELLOW)  
    {
    }
  };
}

extern "C" int mbm_mon(int argc , char** argv) {
  MBM::AsciiMonitor mon(argc, argv);
  return mon.monitor();
}

extern "C" int mbm_ascii(int /* argc */, char** /* argv */) {
  for ( unsigned char i=0; i < 255; ++i)  {
    printf("%3d  %03X   \"%c\"\n", i,i,i);
  }
  return 1;
}
