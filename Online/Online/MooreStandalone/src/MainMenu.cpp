// Framework include files
#include "CPP/Interactor.h"
#include "MBM/Monitor.h"

// C/C++ include files
#include <cstring>

class IocSensor;

namespace MooreTest  {

  /** @class MooreMainMenu MooreMainMenu.h MooresStandalone/MooreMainMenu....h
   *
   *   Class definition of MooreMainMenu.
   *
   *
   *  @author Markus Frank
   *  @version 1.0
   */
  class MooreMainMenu : public Interactor  {
  public:
    /// Thread descriptor of the message pump
    lib_rtl_thread_t m_exec;
    char m_logFile[1024];
    char m_resFile[1024];
    char m_myOpts[1024];
    int m_id;
    FILE* m_log;
    MBM::Monitor* m_mbmMon;
    IocSensor* m_ioc;

    /// Default constructor
    MooreMainMenu();
    /// Default destructor
    virtual ~MooreMainMenu();
    void handleUPI(int cmd, int par, int index);

    virtual  void handle (const Event& event);

    /// Output and printing helper function
    static size_t write_message(void* param, int level, const char* fmt, va_list args);
    void startExecution();
    static int  run(void* param);
    static void backSpaceCallBack (int menu,int /* cmd */,int par,const void* param);
  };
}

#include "RTL/rtl.h"
#include "CPP/Event.h"
#include "CPP/IocSensor.h"
#include "CPP/TimeSensor.h"
#include "UPI/upi_menu.h"
#include "UPI/UpiSensor.h"
#include "UPI/UpiDisplay.h"
#include "MBM/Monitor.h"

extern int runMoore(const std::string& opts);
extern "C" MBM::Monitor* mbm_upi(int argc , char** argv);
extern "C" void scrc_resetANSI();

namespace MooreTest  {

  enum Commands  {
    CMD_BACKSPACE = -1,
    CMD_MBMMON_CLOSE = UPI::UpiDisplay::CMD_CLOSE,
    COMMENT_CONTEXT = 200000,
    COMMENT0,
    COM_RESULT,
    CMD_PAR_RESULT,
    COM_LOG,
    CMD_PAR_LOG,
    COM_OPTS,
    CMD_PAR_OPTS,
    COM_OPT_LINE,
    CMD_MBMMON,
    COM_START_LINE,
    CMD_START,
    CMD_TERMINATE,
    CMD_CLEAN_STRING,
    CMD_WRITE_MESSAGE,
  };

  static void clean_str(char* n,size_t len)  {
    n[len-1] = 0;
    if ( ::strchr(n,' ') ) *::strchr(n,' ') = 0;
  }
  static char* string_copy(char* to, const char* from, size_t len)  {
    char* c = ::strncpy(to,from,len);
    to[len-1] = 0;
    return c;
  }

  MooreMainMenu::MooreMainMenu() : Interactor(), m_exec(0), m_id(0), m_log(0), m_mbmMon(0)  {
    m_id = UpiSensor::newID();
    m_ioc = &IocSensor::instance();
    string_copy(m_resFile,"MooreTest.txt",sizeof(m_resFile));
    string_copy(m_logFile,"MooreTest.log",sizeof(m_logFile));
    string_copy(m_myOpts,"../options/MooreUPI.opts",sizeof(m_myOpts));
    ::upic_open_menu(m_id,0,0,"MooreTest","Standalone","MooreTest");
    ::upic_declare_callback(m_id,CALL_ON_BACK_SPACE,(Routine)backSpaceCallBack,this);
    //::upic_add_comment(COM_RESULT,       "Test Result file name:","");
    //::upic_set_param(m_resFile,1,"A30",m_resFile,0,0,0,0,0);
    //::upic_add_command(CMD_PAR_RESULT,   " ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
    ::upic_add_comment(COM_LOG,          "Test log file name:","");
    ::upic_set_param(m_logFile,1,"A30",m_logFile,0,0,0,0,0);
    ::upic_add_command(CMD_PAR_LOG,      " ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
    ::upic_add_comment(COM_OPTS,         " Steering options:","");
    ::upic_set_param(m_myOpts,1,"A30",m_myOpts,0,0,0,0,0);
    ::upic_add_command(CMD_PAR_OPTS,     " ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^","");
    ::upic_add_comment(COM_OPT_LINE,     "-------------------------------","");
    ::upic_add_command(CMD_MBMMON,       " MBM Monitor>","");
    ::upic_add_comment(COM_START_LINE,   "-------------------------------","");
    ::upic_add_command(CMD_START,        " Start>","");
    ::upic_add_command(CMD_TERMINATE,    " Terminate>","");

    ::upic_close_menu();
    ::upic_set_cursor(m_id,CMD_PAR_RESULT,0);
    UpiSensor::instance().add(this,m_id);
  }

  /// Default destructor
  MooreMainMenu::~MooreMainMenu()  {
  }

  void MooreMainMenu::backSpaceCallBack (int menu,int /* cmd */,int par,const void* param)  {
    Event ev;
    ev.target     = (Interactor*)param;
    ev.eventtype  = UpiEvent;
    ev.menu_id    = menu;
    ev.command_id = CMD_BACKSPACE;
    ev.param_id   = par;
    ev.index_id   = 0;
    ev.target->handle(ev);
  }
  void MooreMainMenu::handleUPI(int cmd, int par, int index)  {
    char buff[132];
    sprintf(buff,"CMD=%d,Param=%d,IDX=%d",cmd,par,index);
    ::upic_write_message(buff,"");
    switch(cmd)   {
    case CMD_PAR_LOG:
      clean_str(m_logFile,sizeof(m_logFile));
      break;
    case CMD_PAR_RESULT:
      clean_str(m_resFile,sizeof(m_resFile));
      break;
    case CMD_PAR_OPTS:
      clean_str(m_myOpts,sizeof(m_myOpts));
      break;
    case CMD_MBMMON:
    case CMD_START:
    case CMD_TERMINATE:
    case CMD_MBMMON_CLOSE:
      m_ioc->send(this,cmd);
      break;
    default:
      break;
    }
  }
  void MooreMainMenu::handle (const Event& event)   {
    switch ( event.eventtype )    {
    case IocEvent:
      switch(event.type)  {
      case CMD_MBMMON_CLOSE:
	if ( m_mbmMon )  {
	  delete m_mbmMon;
	  m_mbmMon = 0;
	}
	break;
      case CMD_MBMMON:
	if ( 0 == m_mbmMon )   {
	  const char *argv[] = {"Monitor","-s",0};
	  UPI::UpiDisplay* disp =  new UPI::UpiDisplay(132,55);
	  m_mbmMon = new MBM::Monitor(2, (char**)argv, disp);
	  disp->setClient(this);
	  m_mbmMon->monitor();
	  TimeSensor::instance().add(m_mbmMon,2,0);
	}
	break;
      case CMD_START:
	::upic_disable_command(m_id,CMD_PAR_LOG);
	::upic_disable_command(m_id,CMD_PAR_RESULT);
	::upic_disable_command(m_id,CMD_START);
	startExecution();
	break;
      case CMD_TERMINATE:
	::upic_delete_menu(m_id);
	::scrc_resetANSI();
	exit(0);
	break;
      case CMD_WRITE_MESSAGE:
	::upic_write_message((char*)event.data,"");
	if ( m_log )  {
	  ::fprintf(m_log,"%s\n",(char*)event.data);
	}
	//::upic_refresh_screen();
 	::free(event.data);
	break;
      default:
	break;
      }
      break;

    case UpiEvent:
      handleUPI(event.command_id, event.param_id, event.index_id);
      break;
    default:
      break;
    }
  }

  /// Output and printing helper function
  size_t MooreMainMenu::write_message(void* param, int, const char* fmt, va_list args)   {
    char str[4096];                              //   string (like printf)
    size_t il=0, len = vsnprintf(str+il,sizeof(str)-il-2,fmt,args);
    str[il+len]   = '\0';
    str[il+len+1] = '\0';
    MooreMainMenu* menu = (MooreMainMenu*)param;
    menu->m_ioc->send(menu,CMD_WRITE_MESSAGE,strdup(str));
    return il;
  }


  void MooreMainMenu::startExecution()   {
    ::upic_write_message("+++++ Starting thread with moore execution.......","");
    clean_str(m_logFile,sizeof(m_logFile));
    if ( ::strlen(m_logFile) > 0 )  {
      m_log = ::fopen(m_logFile,"w");
    }
    int ret = ::lib_rtl_start_thread(run,this,&m_exec);
    if ( !lib_rtl_is_success(ret) ) {
      std::string err = RTL::errorString();
      ::upic_write_message(("Failed to start execution thread....  ["+err+"]").c_str(),"");
    }
  }

  int MooreMainMenu::run(void* param)   {
    MooreMainMenu* m = (MooreMainMenu*)param;
    int ret = runMoore(m->m_myOpts);
    if ( m->m_log ) ::fclose(m->m_log);
    m->m_log = 0;
    return ret;
  }

  void help() {}
}

extern "C" int moore_standalone(int argc, char** argv)  {
  RTL::CLI cli(argc, argv, MooreTest::help);
  if ( cli.getopt("debug",1) ) lib_rtl_start_debugger();
  int status = ::upic_attach_terminal();
  if(status != UPI_NORMAL)
    exit(EXIT_FAILURE);
  Sensor& s = UpiSensor::instance();
  MooreTest::MooreMainMenu t;
  ::lib_rtl_install_printer(MooreTest::MooreMainMenu::write_message,&t);
  s.run();
  return 1;
}
