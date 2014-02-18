// $Id: EventServerRunable.h,v 1.7 2008-10-14 08:37:21 frankb Exp $
//====================================================================
//  MooreTestSvc
//--------------------------------------------------------------------
//
//  Description: GUI to run Moore standalone on a single node.

//  Author     : M.Frank
//====================================================================
// $Header: $
#include "RTL/rtl.h"
#include "CPP/Event.h"
#include "CPP/IocSensor.h"
#include "CPP/TimeSensor.h"
#include "UPI/upi_menu.h"
#include "UPI/UpiSensor.h"
#include "UPI/UpiDisplay.h"
#include "MBM/Monitor.h"
#include "MooreMainMenu.h"
#include "UpiMooreMonitor.h"
#include "Internals.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Property.h"
#include <cstdlib>

extern "C" void scrc_resetANSI();

using namespace LHCb;
using namespace std;

static MooreMainMenu* s_mainMenu = 0;



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
  CMD_EVTMON,
  CMD_RATEMON,
  CMD_ALLMON,
  COM_START_LINE,
  CMD_START,
  CMD_STOP,
  CMD_STOP_CLOSE,
  CMD_ENABLE,
  CMD_TERMINATE,
  CMD_REFEED,
  CMD_CLEAN_STRING,
  CMD_WRITE_MESSAGE,
  CMD_EXIT = MooreTest::CMD_MOORE_EXIT,
  CMD_REFRESH,
  CMD_LAST_OF_MENU
};

namespace LHCb {
  /** class MessageLogger

      Small helper class to catch output of a running Gaudi task

      @author M.Frank
      @version 1.0

  */
  class MessageLogger : public std::streambuf     { 
  public: 
    /// Standard Constructor
    MessageLogger (Interactor* iface); 
    /// Standard destructor
    ~MessageLogger (); 
    /// Callback to fill stream buffer
    int overflow (int c); 
    /// Callback indicating EOF
    int underflow (); 

  private:  
    /// Save old stream buffer
    std::streambuf* _old; 
    /// String currently beeing assembled
    std::string _buf; 
    /// Connection to java environment
    Interactor*      _writer;
  }; 
  /// Standard Constructor
  MessageLogger::MessageLogger (Interactor* iface) : _buf(""), _writer(iface)  {
    _old = std::cout.rdbuf (this); 
  } 
 
  /// Standard Destructor
  MessageLogger::~MessageLogger ()    { 
    std::cout.rdbuf(_old); 
  } 

  /// Callback when filling the stream buffer 
  int MessageLogger::overflow (int c)    {
    if (c == '\n')     {
      IocSensor::instance().send(_writer,CMD_WRITE_MESSAGE,strdup(_buf.c_str()));
      _buf = ""; 
      return c;
    }
    _buf += c;
    return c;
  } 

  /// Callback on EOF
  int MessageLogger::underflow ()  { 
    return (EOF); 
  } 
}

static void clean_str(char* n,size_t len)  {
  n[len-1] = 0;
  if ( ::strchr(n,' ') ) *::strchr(n,' ') = 0;
}
static char* string_copy(char* to, const char* from, size_t len)  {
  char* c = ::strncpy(to,from,len);
  to[len-1] = 0;
  return c;
}

MooreMainMenu::MooreMainMenu(const string& opts) 
: Interactor(), m_exec(0), m_id(0), m_log(0), m_mbmMon(0), 
  m_evtMon(0), m_rateMon(0), m_allMon(0),
  m_svcLocator(0), m_terminate(false)
{
  char cwd[PATH_MAX];
  m_id = UpiSensor::newID();
  m_ioc = &IocSensor::instance();
  m_logger = new MessageLogger(this);
  string_copy(m_resFile,"MooreTest.txt",sizeof(m_resFile));
  string_copy(m_logFile,"MooreTest.log",sizeof(m_logFile));
  string_copy(m_myOpts,opts.c_str(),sizeof(m_myOpts));
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
  ::upic_add_command(CMD_EVTMON,       " Event Monitor>","");
  ::upic_add_command(CMD_RATEMON,      " Rate Monitor>","");
  ::upic_add_command(CMD_ALLMON,       " Full Monitor>","");
  ::upic_add_comment(COM_START_LINE,   "-------------------------------","");
  ::upic_add_command(CMD_START,        " Start>","");
  ::upic_add_command(CMD_STOP,         " Stop>","");
  ::upic_add_command(CMD_STOP_CLOSE,   " Stop and close child menus>","");
  ::upic_add_command(CMD_TERMINATE,    " Terminate>","");

  ::upic_close_menu();
  ::upic_disable_command(m_id,CMD_STOP);
  ::upic_disable_command(m_id,CMD_STOP_CLOSE);
  ::upic_disable_command(m_id,CMD_EVTMON);
  ::upic_disable_command(m_id,CMD_RATEMON);
  ::upic_disable_command(m_id,CMD_ALLMON);
  ::upic_disable_command(m_id,CMD_MBMMON);
  ::upic_set_cursor(m_id,CMD_PAR_RESULT,0);

  ::snprintf(cwd,sizeof(cwd),"Current working directory: ");
  ::getcwd(cwd+strlen(cwd),sizeof(cwd)-strlen(cwd));
  ::upic_write_message(cwd,"");

  s_mainMenu = this;
  UpiSensor::instance().add(this,m_id);
}

/// Default destructor
MooreMainMenu::~MooreMainMenu()  {
}

/// Destroy windows, but do not destruct object
void MooreMainMenu::destroy()  {
  if ( m_mbmMon ) delete m_mbmMon;
  if ( m_evtMon ) handleMonitor(m_evtMon,0);
  if ( m_allMon ) handleMonitor(m_allMon,0);
  if ( m_rateMon ) handleMonitor(m_rateMon,0);
  ::upic_delete_menu(m_id);
  if ( m_logger ) delete m_logger;
  m_mbmMon = 0;
  m_allMon = 0;
  m_evtMon = 0;
  m_rateMon = 0;
  m_logger = 0;
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
  case CMD_STOP:
  case CMD_STOP_CLOSE:
  case CMD_START:
  case CMD_MBMMON:
  case CMD_ALLMON:
  case CMD_EVTMON:
  case CMD_RATEMON:
  case CMD_TERMINATE:
  case CMD_MBMMON_CLOSE:
    m_ioc->send(this,cmd);
    break;
  default:
    sprintf(buff,"Unhandled CMD=%d,Param=%d,IDX=%d",cmd,par,index);
    ::upic_write_message(buff,"");
    break;
  }
}

void MooreMainMenu::handle (const Event& e)   {
  switch ( e.eventtype )    {
  case IocEvent:
    switch(e.type)  {
    case CMD_MBMMON_CLOSE:
      if ( m_mbmMon )  {
	delete m_mbmMon;
	m_mbmMon = 0;
      }
      break;
    case CMD_MBMMON:
      if ( 0 == m_mbmMon )   {
	const char *argv[] = {"Monitor","-s",0};
	UPI::UpiDisplay* disp =  new UPI::UpiDisplay(132,66);
	m_mbmMon = new MBM::Monitor(2, (char**)argv, disp);
	disp->setClient(this);
	m_mbmMon->monitor();
	m_mbmMon->updateDisplay();
	m_mbmMon->updateDisplay();
	TimeSensor::instance().add(m_mbmMon,1,0);
      }
      break;
    case MooreTest::CMD_REMOVE_MOORE_CLIENT:
      if ( e.data == m_evtMon ) { m_evtMon = 0; }
      else if ( e.data == m_allMon ) { m_allMon = 0; }
      else if ( e.data == m_rateMon ) { m_rateMon = 0; }
      break;
    case CMD_REFEED:
      if ( e.data ) reconnectMonitor((UpiMooreMonitor*)e.data);
      break;
    case CMD_RATEMON:
      if ( !handleMonitor(m_rateMon,0) ) ::upic_set_cursor(m_id,CMD_RATEMON,0);
      break;
    case CMD_EVTMON:
      if ( !handleMonitor(m_evtMon,1) ) ::upic_set_cursor(m_id,CMD_EVTMON,0);
      break;
    case CMD_ALLMON:
      if ( !handleMonitor(m_allMon,2) ) ::upic_set_cursor(m_id,CMD_ALLMON,0);
      break;
    case CMD_ENABLE:
      enableTesting();
      break;
    case CMD_START:
      startExecution();
      break;
    case CMD_STOP_CLOSE:
      closeMenus();
      finishExecution();
      break;
    case CMD_STOP:
      finishExecution();
      break;
    case CMD_TERMINATE:
      terminate();
      break;
    case CMD_EXIT:
      if ( m_terminate )  {
	//destroy();
	::scrc_resetANSI();
	_exit(0);
      }
      break;
    case CMD_WRITE_MESSAGE:
      if ( e.data )   {
	::upic_write_message((char*)e.data,"");
	if ( m_log )  {
	  ::fprintf(m_log,"%s\n",(char*)e.data);
	}
	::free(e.data);
      }
      break;
    default:
      break;
    }
    break;

  case UpiEvent:
    handleUPI(e.command_id, e.param_id, e.index_id);
    break;
  case TimeEvent:
    m_ioc->send(this,CMD_EXIT);
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
  menu->write_message(str);
  return il;
}

/// Output and printing helper function
void MooreMainMenu::write_message(const char* str)   {
  m_ioc->send(this,CMD_WRITE_MESSAGE,strdup(str));
}

bool MooreMainMenu::startExecution()   {
  ::upic_disable_command(m_id,CMD_PAR_LOG);
  //::upic_disable_command(m_id,CMD_PAR_RESULT);
  ::upic_disable_command(m_id,CMD_START);
  ::upic_write_message("+++++ Starting thread with moore execution.......","");
  if ( m_exec )  {
    ::lib_rtl_delete_thread(m_exec);
    m_exec = 0;
  }
  clean_str(m_logFile,sizeof(m_logFile));
  if ( ::strlen(m_logFile) > 0 )  {
    m_log = ::fopen(m_logFile,"w");
  }
  int ret = ::lib_rtl_start_thread(run,this,&m_exec);
  if ( !lib_rtl_is_success(ret) ) {
    string err = RTL::errorString();
    ::upic_write_message(("Failed to start execution thread....  ["+err+"]").c_str(),"");
    return false;
  }
  ::lib_rtl_sleep(2000);
  ::upic_enable_command(m_id,CMD_MBMMON);
  ::upic_enable_command(m_id,CMD_EVTMON);
  ::upic_enable_command(m_id,CMD_RATEMON);
  ::upic_enable_command(m_id,CMD_ALLMON);
  ::upic_enable_command(m_id,CMD_STOP);
  ::upic_enable_command(m_id,CMD_STOP_CLOSE);
  ::upic_set_cursor(m_id,CMD_STOP,0);
  m_ioc->send(this,CMD_REFEED,m_evtMon);
  m_ioc->send(this,CMD_REFEED,m_allMon);
  m_ioc->send(this,CMD_REFEED,m_rateMon);
  return true;
}

bool MooreMainMenu::finishExecution()   {
  if ( sendInterrupt("Runable",MooreTest::CMD_MOORE_EXIT,this) )   {
    write_message("Successfully send STOP command to tester.");
    return true;
  }
  write_message("CANNOT send STOP command to tester. [Invalid Gaudi-state]");
  return false;
}

bool MooreMainMenu::enableTesting()   {
  if ( m_log ) ::fclose(m_log);
  m_log = 0;
  ::upic_enable_command(m_id,CMD_PAR_LOG);
  //::upic_enable_command(m_id,CMD_PAR_RESULT);
  ::upic_enable_command(m_id,CMD_START);
  ::upic_disable_command(m_id,CMD_STOP);
  ::upic_disable_command(m_id,CMD_STOP_CLOSE);
  ::upic_disable_command(m_id,CMD_EVTMON);
  ::upic_disable_command(m_id,CMD_RATEMON);
  ::upic_disable_command(m_id,CMD_ALLMON);
  ::upic_disable_command(m_id,CMD_MBMMON);
  ::upic_set_cursor(m_id,CMD_START,0);
  return true;
}

/// Close all monitor menues in one go....
void MooreMainMenu::closeMenus()   {
  if ( m_rateMon ) m_ioc->send(m_rateMon,MooreTest::CMD_CLOSE);
  if ( m_evtMon  ) m_ioc->send(m_evtMon,MooreTest::CMD_CLOSE);
  if ( m_allMon  ) m_ioc->send(m_allMon,MooreTest::CMD_CLOSE);
  m_allMon = 0;
  m_evtMon = 0;
  m_rateMon = 0;
}

void MooreMainMenu::terminate()   {
  closeMenus();
  m_terminate = true;
  if ( !finishExecution() ) TimeSensor::instance().add(this,1,0);
}

bool MooreMainMenu::reconnectMonitor(UpiMooreMonitor* s)  {
  if ( s )  {
    if ( m_svcLocator )   {
      SmartIF<IService> svc = m_svcLocator->service("LHCb::MooreMonitorSvc",true);
      if ( svc )  {
	Interactor* a = dynamic_cast<Interactor*>(svc.get());
	if ( a )   {
	  s->setFeed(a);
	}
      }
    }
  }
  return false;
}

bool MooreMainMenu::handleMonitor(UpiMooreMonitor*& m, int typ)    {
  if ( m != 0 )   {
    m_ioc->send(m,MooreTest::CMD_CLOSE);
    m = 0;
  }
  else if ( m_svcLocator )   {
    SmartIF<IService> svc = m_svcLocator->service("LHCb::MooreMonitorSvc",true);
    if ( svc )  {
      Interactor* a = dynamic_cast<Interactor*>(svc.get());
      if ( a )   {
	m = new UpiMooreMonitor(typ, this, a);
	return true;
      }
    }
  }
  return false;
}

bool MooreMainMenu::sendInterrupt(const string& service, int code, void* param)  {
  if ( m_svcLocator )   {
    SmartIF<IService> svc = m_svcLocator->service(service,false);
    if ( svc ) {
      Interactor* a = dynamic_cast<Interactor*>(svc.get());
      if ( a )   {
	m_ioc->send(a,code,param);
	return true;
      }
    }
  }
  return false;
}

int MooreMainMenu::run(void* param)   {
  MooreMainMenu* m = (MooreMainMenu*)param;
  int ret = m->runMoore(m->m_myOpts);
  return ret;
}

int MooreMainMenu::runMoore(const string& opts) {
  Gaudi::setInstance((IAppMgrUI*)0);
  IInterface* iface = Gaudi::createApplicationMgr();
  SmartIF<IProperty>     propMgr ( iface );
  SmartIF<IAppMgrUI>     appMgr  ( iface );

  if( !appMgr.isValid() || !propMgr.isValid() ) {
    write_message("Fatal error while creating the ApplicationMgr ");
    return 1;
  }

  propMgr->setProperty( "JobOptionsPath", opts ).ignore();
  propMgr->setProperty( "MessageSvcType","LHCb::GenericMessageSvc");

  // Run the application manager and process events
  StatusCode sc = StatusCode::SUCCESS;
  {
    SmartIF<ISvcLocator> loc(iface);
    m_svcLocator = loc.get();
    TimeSensor::instance().add(this,1,0);
    sc = appMgr->run();
    m_svcLocator = 0;
  }
  IntegerProperty returnCode("ReturnCode", 0);
  propMgr->getProperty(&returnCode).ignore();
  // Release Application Manager
  iface->release();
  // All done - exit
  if (sc.isFailure() && returnCode.value() == 0) {
    // propagate a valid error code in case of failure
    returnCode.setValue(1);
  }
  m_ioc->send(this,CMD_MBMMON_CLOSE);
  m_ioc->send(this,CMD_ENABLE);
  return returnCode.value();
}


extern "C" int log_write_message(const char* text)   {
  s_mainMenu->write_message(text);
  return 0x1;
}

static void help() {
  ::printf(" Usage: moore_standalone -opt [-opt]\n"
	   "    -o(ptions)=<file-name>  Main steering options. \n");
  _exit(0);
}

extern "C" int moore_standalone(int argc, char** argv)  {
  string opts;
  RTL::CLI cli(argc, argv, help);
  if ( cli.getopt("debug",1) ) lib_rtl_start_debugger();
  cli.getopt("options",1,opts);

  if ( opts.empty() )    {
    help();
  }
  int status = ::upic_attach_terminal();
  if(status != UPI_NORMAL)
    exit(EXIT_FAILURE);
  Sensor& s = UpiSensor::instance();
  MooreMainMenu t(opts);
  ::lib_rtl_install_printer(MooreMainMenu::write_message,&t);
  s.run();
  return 1;
}
