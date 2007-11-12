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
#include "UPI/UpiSensor.h"
#include "UPI/upidef.h"
#include "CPP/TimeSensor.h"
#include "CPP/Interactor.h"
#include "SCR/scr.h"

#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

namespace MBM {

  static const int TERM_HEIGHT    =  44;
  static const int TERM_WIDTH     = 126;
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
  enum { CMD_CLOSE=1 };
  struct UpiMonitor : public Monitor, public Interactor {
    int              m_menuID, m_lastCMD;
    char             m_lines[TERM_HEIGHT][TERM_WIDTH+1];

    virtual void setup_window() {
    }

    virtual void reset_window() {
    }

    void print_char(int x, int y, int val) {
      if ( y <= TERM_HEIGHT && x <= TERM_WIDTH ) m_lines[y][x-1]=char(val);
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
      va_list args;
      char buffer[1024];
      va_start( args, format );
      size_t len = ::vsprintf(buffer, format, args);
      print_char(1, m_currLine, VERT_BAR);
      for(size_t j=0; j<len && j<TERM_WIDTH-1; ++j)
        print_char(j+2, m_currLine, buffer[j]);
      for(size_t i=len; i < TERM_WIDTH-1; ++i)
        print_char(i+2, m_currLine, ' ');
      print_char(TERM_WIDTH, m_currLine, VERT_BAR);
      return ++m_currLine;
    }

    virtual size_t draw_line_reverse(const char* format,...)  {
      va_list args;
      char buffer[1024];
      va_start( args, format );
      size_t len = ::vsprintf(buffer, format, args);
      print_char(1, m_currLine, VERT_BAR);
      for(size_t j=0; j<len && j<TERM_WIDTH-1; ++j)
        print_char(j+2, m_currLine, buffer[j]);
      for(size_t i=len; i < TERM_WIDTH-1; ++i)
        print_char(i+2, m_currLine, ' ');
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
      ::memset(m_lines,0,sizeof(m_lines));
      m_currLine = 0;
      print_char(1,m_currLine,LEFT_UP_EDGE);
      for(size_t i=1; i < TERM_WIDTH-1; ++i)
        print_char(i+1,m_currLine,HORZ_BAR);
      print_char(TERM_WIDTH,m_currLine,RIGHT_UP_EDGE);      
      m_currLine = 1;
    }
    virtual void end_update() { 
      print_char(1,m_currLine,LEFT_LOW_EDGE);
      for(size_t j=1; j < TERM_WIDTH-1; ++j)
        print_char(j+1,m_currLine,HORZ_BAR);
      print_char(TERM_WIDTH,m_currLine,RIGHT_LOW_EDGE);      
      m_currLine++;
      for(int i=1,last=m_currLine;i<last;++i) {
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
      for(int k=m_currLine, klast=m_lastCMD;k<=klast;++k) {
        int cmd = CMD_CLOSE+1+k;
	bool isCmd = cmd%20 == 0;
	isCmd ? upic_delete_command(m_menuID,cmd) : upic_delete_comment(m_menuID,cmd);
      }
      m_lastCMD = CMD_CLOSE+m_currLine;
    }
    virtual size_t display_width() const {
      return TERM_WIDTH;
    }
    UpiMonitor(int argc , char** argv) : Monitor(argc,argv)    {
      m_menuID = UpiSensor::newID();
      upic_open_window();
      upic_open_menu(m_menuID,0,0,"MBM Monitor",RTL::processName().c_str(),RTL::nodeName().c_str());
      upic_add_command(CMD_CLOSE,"Close","");
      upic_close_menu();
      UpiSensor::instance().add(this,m_menuID);
      updateDisplay();
      m_lastCMD = CMD_CLOSE;
    }
    virtual ~UpiMonitor() {
      upic_delete_menu(m_menuID);
    }
    virtual int monitor() {
      int status = initMonitor();
      if ( !lib_rtl_is_success(status) ) {
	exit(status);
      }
      setup_window();
      TimeSensor::instance().add(this,1,0);
      UpiSensor::instance().run();
      reset_window();
      return 1;
    }
    void handle(const Event& ev) {
      switch(ev.eventtype) {
      case UpiEvent:
	if ( ev.command_id == CMD_CLOSE ) {
          upic_write_message("Close window.","");
	  upic_quit();
	  exit(0);
	}
	break;
      case TimeEvent:
	updateDisplay();
	TimeSensor::instance().add(this,2,0);
	break;
      default:	
	{
	  char txt[132];
	  sprintf(txt,"Unknown event received of type: %d",ev.eventtype);
	  upic_write_message(txt,"");
	}
	break;
      }
    }
  };
}

extern "C" int run_mbm_upi(int argc , char** argv) {
  UpiSensor::instance();
  MBM::UpiMonitor mon(argc, argv);
  return mon.monitor();
}

extern "C" MBM::Monitor* mbm_upi(int argc , char** argv) {
  MBM::UpiMonitor* mon = new MBM::UpiMonitor(argc, argv);
  return mon;
}
