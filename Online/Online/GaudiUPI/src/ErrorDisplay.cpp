// Include Files
#include "Gaudi/ErrorDisplay.h"
#include "Gaudi/PropertyMenu.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Message.h"
#include "GaudiKernel/SmartIF.h"
#include "Gaudi/DisplayFactory.h"
#include "Gaudi/DialogMenu.h"
#include "Gaudi/UpiSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"
#include "SCR/scr.h"
#include <sstream>
#include <cstdarg>
#include <ctime>

extern "C"  {
  int upic_set_message_window (int,int,int,int);
  int upic_get_message_window (int*,int*,int*,int*);
  int upic_get_screen_size(int* rows, int* cols);
  void upic_write_message(const char*, const char*);
  void upic_write_rendered_message(const char*, const char*,int);
  void upic_quit();
}

// Instantiation of a static factory class used by clients
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,ErrorDisplay)

enum TopMenuItems {
  CMD_COMMAND = 1,
  CMD_TERMINATE
};

/// Standard Constructor
LHCb::ErrorDisplay::Logger::Logger (LHCb::ErrorDisplay& iface) : m_logger(iface)  {
  _buf = ""; 
  _old = std::cout.rdbuf (this); 
} 
 
/// Standard Destructor
LHCb::ErrorDisplay::Logger::~Logger ()    { 
  std::cout.rdbuf(_old); 
} 

/// Callback when filling the stream buffer 
int LHCb::ErrorDisplay::Logger::overflow (int c)    {
  if (c == '\n')     {
    m_logger.report(MSG::ALWAYS,"",_buf);
    _buf = ""; 
  } 
  else     { 
    _buf += c; 
  } 
  return (c); 
} 

/// Callback on EOF
int LHCb::ErrorDisplay::Logger::underflow ()  { 
  return (EOF); 
} 

/// Standard Constructor.
LHCb::ErrorDisplay::ErrorDisplay( const std::string& nam, ISvcLocator* svc )
: DimErrorLogger(nam, svc), m_window(0), m_child(0)
{
  m_cmd = new DialogItem("%5s","   ^^^^  ^^^^^ ","Exit");
  m_cmd->addList("Exit");
  m_cmd->addList("Setup");
}
    
/// Destructor.
LHCb::ErrorDisplay::~ErrorDisplay() { 
  print("UPI Service destructed.");
  if ( m_log ) delete m_log;
  if ( m_cmd ) delete m_cmd;
}

/// Initialize the service.
StatusCode LHCb::ErrorDisplay::initialize() {
  StatusCode sc = DimErrorLogger::initialize();
  if ( sc.isSuccess() )  {
    if ( 0 == m_log )  {
      UpiSensor::instance();
      DisplayFactory f("UPI");
      m_log = new Logger(*this);
    }
    showTopMenu();
  }
  return sc;
}
  
/// Finalise the service.
StatusCode LHCb::ErrorDisplay::finalize() {
  setNewChild(0);
  if ( m_window ) delete m_window;
  m_window = 0;
  return DimErrorLogger::finalize();
}

void LHCb::ErrorDisplay::showTopMenu()  {
  int srows, scols, cols, rows, col, row;
  DialogMenuCreator win(this);
  m_window = win->create(RTL::processName(),RTL::nodeNameShort(),name());
  m_window->addButtonLine(CMD_COMMAND,m_cmd);
  win->map();
  upic_get_screen_size(&srows, &scols);
  upic_get_message_window(&rows, &cols,&row, &col);
  upic_set_message_window (srows-5, scols-2, 6, 2);
}

void LHCb::ErrorDisplay::setNewChild(Interactor* c)  
{
  if ( m_child ) delete m_child;
  m_child = c;
}

/// Show message in the message window
void LHCb::ErrorDisplay::print(const char* fmt, ...)   {
  char buffer[1024];
  int len;
  va_list arguments;
  va_start( arguments, fmt);
  len = vsprintf(buffer,fmt,arguments);
  buffer[len] = 0;
  va_end ( arguments );
  upic_write_message(buffer,"");
}

void LHCb::ErrorDisplay::reportMessage(int typ, const std::string& src, const std::string& msg)  {
  std::string buff;
  std::ostringstream os;
  time_t t = time (0);
  const char* fmt = "% F%42W%S%7W%R%T %0W%M";
  char* timestr = ctime(&t);
  timestr[19] = ' ';
  timestr[20] = 0;
  Message m(src,typ,msg);
  m.setFormat(fmt);
  os << timestr+4 << " " << m;
  switch(typ)  {
    case MSG::WARNING:
      ::upic_write_rendered_message(os.str().c_str(),"",SCR::BOLD);
      break;
    case MSG::ERROR:
      ::upic_write_rendered_message(os.str().c_str(),"",SCR::INVERSE);
      break;
    case MSG::FATAL:
      ::upic_write_rendered_message(os.str().c_str(),"",SCR::BOLD|SCR::INVERSE);
      break;
    case MSG::VERBOSE:
    case MSG::DEBUG:
    case MSG::INFO:
    case MSG::ALWAYS:
    default:
      ::upic_write_rendered_message(os.str().c_str(),"",SCR::NORMAL);
      break;
  }
}

/// Interactor overload: handle menu interupts
void LHCb::ErrorDisplay::handle (const Event& e)   {
  if ( e.eventtype == UpiEvent )  {
    switch ( e.eventtype )  {
    case UpiEvent:
      switch(e.command_id)  {
      case CMD_COMMAND:
        switch(e.param_id)  {
          case 1:
            IOCSENSOR.send(this, CMD_TERMINATE);
            break;
          case 2:
            setNewChild(new Gaudi::PropertyMenu(name(),this,this));
            break;
        }
        break;
      case Gaudi::SubMenu::CMD_CLOSE:           // Close of submenus
        m_window->setCursor(CMD_COMMAND);
        break;
      default:
        break;
      }
    default:
      break;
    }
  }
  else if ( e.eventtype == IocEvent )  {
    switch(e.type)  {
      case CMD_TERMINATE:
        m_window->destroy();
        release();
        upic_quit();
        exit(0);
        return;
      case Gaudi::SubMenu::CMD_CLOSE:
        setNewChild(0);
        break;
    }
  }
}
