/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                  SCR   GRAPHICS DISPLAY                               */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#include <cstdio>
#include "SCR/scr.h"
#include "SCR/ScrDisplay.h"

#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif
using namespace SCR;
static Pasteboard* pb = 0;

//static const int TEE_LEFT       = '-';
//static const int TEE_RIGHT      = '-';
static const int HORZ_BAR       = scrc_horizontal_bar();
//static const int VERT_BAR       = scrc_vertical_bar();
static const int FAT_VERT_BAR   = ' ';
static const int DIM_VERT_BAR   = '.';
//static const int LEFT_LOW_EDGE  = scrc_bottom_left_corner();
//static const int RIGHT_LOW_EDGE = scrc_bottom_right_corner();
//static const int LEFT_UP_EDGE   = scrc_top_left_corner();
//static const int RIGHT_UP_EDGE  = scrc_top_right_corner();

static int s_border = 0;

/// Set global pasteborard area
void ScrDisplay::setPasteboard(Pasteboard* pasteboard) {
  pb = pasteboard;
}

/// Set global border type
void ScrDisplay::setBorder(int b) {
  s_border = b;
}

/// Access to SCR Pasteboard
Pasteboard* ScrDisplay::pasteboard() const {
  return pb;
}

void ScrDisplay::print_char(int x, int y, int attr, int val) {
  ::scrc_put_char(m_display,char(val),attr,y,x);
}

void ScrDisplay::setup_window() {
  //::scrc_create_pasteboard (&pb, device, &pb_rows, &pb_cols);
  if ( 0 == pb ) {
    int w = 0, h = 0;
    ::scrc_create_pasteboard (&pb, 0, &h, &w);
    m_area.width  = w-2;
    m_area.height = h-2;
  }
  m_pb = pb;
  ::scrc_create_display (&m_display, m_area.height, m_area.width, NORMAL|s_border, ON, m_title.c_str());
  ::scrc_paste_display (m_display, pb, m_position.y, m_position.x);
  //::scrc_end_pasteboard_update (pb);
  //::scrc_fflush (pb);
}

/// Default Constructor with display sizes
void ScrDisplay::setup_window(const Position& pos, const Area& a, const std::string& title) {
  m_position = Position(pos.x+2,pos.y+2);
  m_area     = a;
  m_title    = title;
  setup_window();
}

MonitorDisplay* ScrDisplay::createSubDisplay(const Position& pos, const Area& a, const std::string& title) {
  ScrDisplay* d = new ScrDisplay();
  d->setup_window(pos,a,title);
  return d;
}

void ScrDisplay::reset_window() {
  ::scrc_clear_screen(pb);
  ::scrc_delete_pasteboard(pb);
  ::scrc_setANSI();
}

size_t ScrDisplay::draw_line()  {
  for(int i=0; i < m_area.width; ++i)
    print_char(i+1, m_currLine, NORMAL|GRAPHIC, HORZ_BAR);
  return ++m_currLine;
}

size_t ScrDisplay::draw_bar()  {
  for(int i=0; i < m_area.width; ++i)
    print_char(i+1, m_currLine, INVERSE, FAT_VERT_BAR);
  return ++m_currLine;
}

size_t ScrDisplay::draw_line_normal(const char* format,...)  {
  int flags = NORMAL;
  va_list args;
  char buffer[1024];
  va_start( args, format );
  int len = ::vsprintf(buffer, format, args);
  va_end(args);
  for(int j=0; j<len && j<m_area.width; ++j)
    print_char(j+1, m_currLine, flags, buffer[j]);
  for(int i=len; i < m_area.width; ++i)
    print_char(i+1, m_currLine, flags, ' ');
  return ++m_currLine;
}

size_t ScrDisplay::draw_line_reverse(const char* format,...)  {
  int flags = INVERSE;
  va_list args;
  char buffer[1024];
  va_start( args, format );
  int len = ::vsprintf(buffer, format, args);
  va_end(args);
  for(int j=0; j<len && j<m_area.width; ++j)
    print_char(j+1, m_currLine, flags, buffer[j]);
  for(int i=len; i < m_area.width; ++i)
    print_char(i+1, m_currLine, flags, ' ');
  return ++m_currLine;
}

size_t ScrDisplay::draw_line_bold(const char* format,...)  {
  int flags = BOLD;
  va_list args;
  char buffer[1024];
  va_start( args, format );
  int len = ::vsprintf(buffer, format, args);
  va_end(args);
  for(int j=0; j<len && j<m_area.width; ++j)
    print_char(j+1, m_currLine, flags, buffer[j]);
  for(int i=len; i < m_area.width; ++i)
    print_char(i+1, m_currLine, flags, ' ');
  return ++m_currLine;
}

size_t ScrDisplay::draw_bar(int x, int y, float f1, int scale)  {
  for (int i=0, m=scale; i<m; ++i)  {
    if ( f1*m > i )
      print_char(i+x,y,INVERSE|UNDERLINE,FAT_VERT_BAR);
    else
      print_char(i+x,y,NORMAL|UNDERLINE,DIM_VERT_BAR);
  }
  return 1;
}

void ScrDisplay::begin_update()  {
  m_currLine = 1;
#if 0
  if ( m_pb ) {
    int rows=0, cols=0, r=0, c=0;
    ::scrc_get_console_dimensions(&rows,&cols);
    ::scrc_get_pasteboard_infos(m_pb,&r,&c);
    if ( rows != r || cols != c ) {
      ::scrc_change_pasteboard(m_pb,&rows,&cols);
      m_area.width  = cols-2;
      m_area.height = rows-2;
    }
  }
#endif
  ::scrc_begin_pasteboard_update(pb);
}

void ScrDisplay::end_update() { 
  while(m_currLine<=size_t(m_area.height)) {
    draw_line_normal("");
  }
  m_currLine++;
  ::scrc_end_pasteboard_update (pb);
  ::scrc_fflush (pb);
}

ScrDisplay::ScrDisplay(int w, int h) : MonitorDisplay(), m_pb(0), m_display(0)  {
  m_position.x = m_position.y = 2;
  m_area.height = h;
  m_area.width =  w;
  m_currLine = 1;
}

ScrDisplay::ScrDisplay() : MonitorDisplay(), m_pb(0), m_display(0)  {
  m_position.x = m_position.y = 2;
  m_area.height = 0;
  m_area.width =  0;
  m_currLine = 1;
}

/// Default destructor
ScrDisplay::~ScrDisplay() {
  if ( pb ) {
    ::scrc_begin_pasteboard_update(pb);
    if ( m_display )  {
      ::scrc_unpaste_display(m_display, pb);
      ::scrc_delete_display(m_display);
    }
    ::scrc_end_pasteboard_update(pb);
  }
  m_display = 0;
  m_pb = 0;
}

extern "C" int scr_ascii(int /* argc */, char** /* argv */) {
  Display* display = 0, *d1 = 0;
  int height, width;
  ::scrc_create_pasteboard (&pb, 0, &height, &width);
  ::scrc_create_display (&display, 20, 110, NORMAL, ON, "");
  ::scrc_paste_display (display, pb, 2, 2);
  ::scrc_end_pasteboard_update (pb);
  ::scrc_fflush (pb);
  ::scrc_begin_pasteboard_update (pb);
  for (char j=0, v='0', vv='0'; j < 100; ++j)  {
    vv = char('0'+(j/10));
    if ( vv != v ) {
      v = vv;
      ::scrc_put_char(display,char('0'+(j/10)),NORMAL,2,2+j);
    }
    ::scrc_put_char(display,char('0'+(j%10)),NORMAL,3,2+j);
  }
  for ( unsigned char i=0; i < 255; ++i)  {
    ::scrc_put_char(display,i,NORMAL,5+(i/100),2+(i%100));
  }
  for ( unsigned char i=0; i < 255; ++i)  {
    ::scrc_putes("(0", pb);
    ::scrc_put_char(display,i,NORMAL|GRAPHIC,10+(i/100),2+(i%100));
  }
  for(int k=0; k<5; ++k) {
    d1 = 0;
    ::scrc_create_display (&d1, 5, 30, NORMAL, ON, "");
    ::scrc_paste_display (d1, pb, 20+2*k, 20+10*k);
  }
  ::scrc_end_pasteboard_update (pb);
  ::scrc_fflush (pb);
  ::scrc_delete_pasteboard(pb);
  return 1;
}
