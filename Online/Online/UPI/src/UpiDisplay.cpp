/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                  UPI BUFFER MANAGER                                   */
/*                  INSTALLATION PROGRAM FOR THE AEB                     */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#include <cstdlib>
#include <cstdarg>
#include "CPP/Event.h"
#include "MBM/Monitor.h"
#include "UPI/upidef.h"
#include "UPI/UpiSensor.h"
#include "UPI/UpiDisplay.h"
#include "CPP/TimeSensor.h"
#include "CPP/Interactor.h"
#include "SCR/scr.h"
#include <stdexcept>

#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

using namespace UPI;

static const int TEE_LEFT       = '-';
static const int TEE_RIGHT      = '-';
static const int HORZ_BAR       = '-';
static const int VERT_BAR       = ' ';
static const int FAT_VERT_BAR   = 'X';
static const int DIM_VERT_BAR   = '.';
static const int LEFT_LOW_EDGE  = '-';
static const int RIGHT_LOW_EDGE = '-';
static const int LEFT_UP_EDGE   = '-';
static const int RIGHT_UP_EDGE  = '-';

UPI::ReallyClose::ReallyClose(int menu, int cmd) {
  m_menuID = UpiSensor::newID();
  upic_open_detached_menu(m_menuID,menu,cmd,"Confirm",RTL::processName().c_str(),"Really quit the application?");
  upic_set_param(m_buff,1,"A2","NO", 0, 0, 0, 0, 0);
  upic_set_param(m_buff,2,"A3","YES", 0, 0, 0, 0, 0);
  upic_add_command(1,"Really Close ? ^^   ^^^","");
  upic_close_menu();
  UpiSensor::instance().add(this,m_menuID);
}
UPI::ReallyClose::~ReallyClose() {
  UpiSensor::instance().remove(this,m_menuID);
  upic_delete_menu(m_menuID);
}
void UPI::ReallyClose::handle(const Event& ev) {
  if (ev.eventtype==UpiEvent && ev.command_id == 1 ) {
    if ( ev.param_id == 2 ) {
      upic_write_message("Close window.","");
      upic_quit();
      exit(0);
    }
    else {
      upic_hide_menu(m_menuID);
    }
  }
}

void UPI::ReallyClose::invoke() {
  upic_set_cursor(m_menuID,1, 2);
}

void UpiDisplay::setup_window() {
}

void UpiDisplay::reset_window() {
}

void UpiDisplay::print_char(int x, int y, int val) {
  if ( y <= m_area.height && x <= m_area.width ) m_lines[y][x-1]=char(val);
}

size_t UpiDisplay::draw_line()  {
  print_char(1, m_currLine, TEE_LEFT);
  for(int i=1; i < m_area.width-1; ++i)
    print_char(i+1, m_currLine, HORZ_BAR);
  print_char(m_area.width, m_currLine, TEE_RIGHT);
  return ++m_currLine;
}

size_t UpiDisplay::draw_bar()  {
  print_char(1, m_currLine, VERT_BAR);
  for(int i=1; i < m_area.width-1; ++i)
    print_char(i+1, m_currLine, FAT_VERT_BAR);
  print_char(m_area.width, m_currLine, VERT_BAR);      
  return ++m_currLine;
}

size_t UpiDisplay::draw_line_normal(const char* format,...)  {
  va_list args;
  char buffer[1024];
  va_start( args, format );
  int len = ::vsprintf(buffer, format, args);
  print_char(1, m_currLine, VERT_BAR);
  for(int j=0; j<len && j<m_area.width-1; ++j)
    print_char(j+2, m_currLine, buffer[j]);
  for(int i=len; i < m_area.width-1; ++i)
    print_char(i+2, m_currLine, ' ');
  print_char(m_area.width, m_currLine, VERT_BAR);
  return ++m_currLine;
}

size_t UpiDisplay::draw_line_bold(const char* format,...)  {
  va_list args;
  char buffer[1024];
  va_start( args, format );
  int len = ::vsprintf(buffer, format, args);
  print_char(1, m_currLine, VERT_BAR);
  for(int j=0; j<len && j<m_area.width-1; ++j)
    print_char(j+2, m_currLine, buffer[j]);
  for(int i=len; i < m_area.width-1; ++i)
    print_char(i+2, m_currLine, ' ');
  print_char(m_area.width, m_currLine, VERT_BAR);
  return ++m_currLine;
}

size_t UpiDisplay::draw_line_reverse(const char* format,...)  {
  va_list args;
  char buffer[1024];
  va_start( args, format );
  int len = ::vsprintf(buffer, format, args);
  print_char(1, m_currLine, VERT_BAR);
  for(int j=0; j<len && j<m_area.width-1; ++j)
    print_char(j+2, m_currLine, buffer[j]);
  for(int i=len; i < m_area.width-1; ++i)
    print_char(i+2, m_currLine, ' ');
  print_char(m_area.width, m_currLine, VERT_BAR);
  return ++m_currLine;
}

size_t UpiDisplay::draw_bar(int x, int y, float f1, int scale)  {
  for (int i=0, m=scale; i<m; ++i)  {
    print_char(i+x,y,f1*m > i ? FAT_VERT_BAR : DIM_VERT_BAR);
  }
  return 1;
}

void UpiDisplay::begin_update()  {
  for(int i=0; i<m_area.height; ++i) memset(m_lines[i],0,m_area.width+1);
  m_currLine = 0;
  print_char(1,m_currLine,LEFT_UP_EDGE);
  for(int i=1; i < m_area.width-1; ++i)
    print_char(i+1,m_currLine,HORZ_BAR);
  print_char(m_area.width,m_currLine,RIGHT_UP_EDGE);      
  m_currLine = 1;
}

void UpiDisplay::end_update() { 
  print_char(1,m_currLine,LEFT_LOW_EDGE);
  for(size_t j=1,jlast=m_area.width-1; j < jlast; ++j)
    print_char(j+1,m_currLine,HORZ_BAR);
  print_char(m_area.width,m_currLine,RIGHT_LOW_EDGE);      
  m_currLine++;
  if ( m_currLine > size_t(m_area.height) ) m_currLine = size_t(m_area.height);
  for(size_t i=1,last=m_currLine;i<last;++i) {
    int cmd = CMD_CLOSE+1+i;
    bool isCmd = cmd%20 == 0;
    const char* ptr = m_lines[i]+1;
    if ( cmd > m_lastCMD ) {
      if ( isCmd ) 
	upic_insert_command(m_menuID,CMD_CLOSE,cmd,ptr,"");
      else
	upic_insert_comment(m_menuID,CMD_CLOSE,cmd,ptr,"");
      m_lastCMD = cmd;
    }
    else if ( isCmd ) 
      upic_replace_command(m_menuID,cmd,ptr,"");
    else
      upic_replace_comment(m_menuID,cmd,ptr,"");
  }
  for(size_t k=m_currLine, klast=m_lastCMD;k<=klast;++k) {
    int cmd = CMD_CLOSE+1+k;
    bool isCmd = cmd%20 == 0;
    isCmd ? upic_delete_command(m_menuID,cmd) : upic_delete_comment(m_menuID,cmd);
  }
  m_lastCMD = CMD_CLOSE+m_currLine;
}

void UpiDisplay::handle(const Event& ev) {
  switch(ev.eventtype) {
  case UpiEvent:
    if ( ev.command_id == CMD_CLOSE ) {
      m_quit->invoke();
      return;
    }
  default:  // Fall through: Handle request by client
    if ( m_client )  {
      m_client->handle(ev);
    }
    break;
  }
}

UpiDisplay::UpiDisplay(size_t w, size_t h)  
{
  m_area = Area(w,h);
  m_menuID = UpiSensor::newID();
  upic_open_window();
  upic_open_menu(m_menuID,0,0,"MBM Monitor",RTL::processName().c_str(),RTL::nodeName().c_str());
  upic_add_command(CMD_CLOSE,"Close","");
  upic_close_menu();
  UpiSensor::instance().add(this,m_menuID);
  m_lastCMD = CMD_CLOSE;
  m_lines = new char*[m_area.height+1];
  for(int i=0; i<=m_area.height; ++i) m_lines[i] = new char[m_area.width+1];
  m_quit = new ReallyClose(m_menuID,CMD_CLOSE);
}


/// No copy constructor allowed
UpiDisplay::UpiDisplay(const UpiDisplay& /* display */)  {
  throw std::runtime_error("UpiDisplay::UpiDisplay(const UPIDisplay& d) Call not allowed!");
}

/// No assignment allowed
UpiDisplay& UpiDisplay::operator=(const UpiDisplay& /* display */)   {
  throw std::runtime_error("UpiDisplay::operator=: Call not allowed!");
}

UpiDisplay::~UpiDisplay() {
  UpiSensor::instance().remove(this,m_menuID);
  upic_delete_menu(m_menuID);
  for(int i=0; i<=m_area.height; ++i) delete [] m_lines[i];
  delete [] m_lines;
  delete m_quit;
}
