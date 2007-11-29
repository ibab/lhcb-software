#include "GaudiUPI/UPIService.h"
#include "GaudiUPI/PropertyMenu.h"
#include "GaudiUPI/AlgorithmMenu.h"
#include "GaudiUPI/ServiceMenu.h"
#include "GaudiUPI/DialogMenu.h"
#include "GaudiUPI/DisplayFactory.h"
#include "UPI/UpiSensor.h"

// Include Files
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAlgorithm.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"

#include <cstdarg>

extern "C" void upic_write_message(const char*, const char*);

// Instantiation of a static factory class used by clients
DECLARE_NAMESPACE_SERVICE_FACTORY(Gaudi,UPIService)

enum TopMenuItems {
  COMMENT0 = 1,
  CMD_APPMGR,
  CMD_APPMGR_TERMINATE,
  COMMENT1            = 10000,
  CMD_SHOW_ALGORITHMS = 20000,
  CMD_SHOW_SERVICES   = 30000,
  CMD_SHOW_TOOLS      = 40000,
  COMMENT2,
  CMD_TERMINATE,
  CMD_UPDATE_STATE,
  CMD_DEBUG
};


/// Standard Constructor.
Gaudi::UPIService::UPIService( const std::string& nam, ISvcLocator* svc )
: Service(nam, svc), m_window(0), m_child(0), m_appMgr(0)
{
  if ( svc && nam != "DefaultName" ) {  // Does not work with genconf
    svc->queryInterface(IAppMgrUI::interfaceID(),(void**)&m_appMgr);
    addRef();
  }
}
    
/// Destructor.
Gaudi::UPIService::~UPIService() { 
  if ( name() != "DefaultName" ) {  // Does not work with genconf
    print("UPI Service destructed.");
    updateState(m_appMgr->stateName());
  }
}

/// Initialize the service.
StatusCode Gaudi::UPIService::initialize() {
  StatusCode sc = Service::initialize();
  //::lib_rtl_start_debugger();
  if ( sc.isSuccess() )  {
    UpiSensor::instance();
    DisplayFactory f("UPI");
    showTopMenu();
  }
  return sc;
}
  
/// Finalise the service.
StatusCode Gaudi::UPIService::finalize() {
  StatusCode sc = Service::finalize();
  setNewChild(0);
  print("UPI Service finalized.");
  updateState(m_appMgr->stateName());
  delete m_window;
  m_window = 0;
  return sc;
}

void Gaudi::UPIService::showTopMenu()  {
  DialogMenuCreator win(this);
  m_window = win->create(RTL::processName(),name(),RTL::processName());
  win->addCOM(COMMENT0,            "----------------------");
  win->addCMD(CMD_APPMGR,          "  ???                 ");
  win->addCMD(CMD_APPMGR_TERMINATE,"Terminate             ");
  win->addCOM(COMMENT1,            "----------------------");
  win->addCMD(CMD_SHOW_ALGORITHMS, "Show algorithms>      ");
  win->addCMD(CMD_SHOW_SERVICES,   "Show services>        ");
  win->addCMD(CMD_SHOW_TOOLS,      "Show tools>           ");
  win->addCOM(COMMENT2,            "----------------------");
  win->addCMD(CMD_DEBUG,           "Start debugger        ");
  win->addCMD(CMD_TERMINATE,       "Exit>                 ");
  win->map();
  IOCSENSOR.send(this, CMD_UPDATE_STATE);
}

void Gaudi::UPIService::updateState(const std::string& state)  {
  if ( m_window )  {
    if ( state == "Finalized" )
      m_window->enableCMD(CMD_APPMGR_TERMINATE);
    else
      m_window->disableCMD(CMD_APPMGR_TERMINATE);
    if ( state == "Offline" )
      m_window->replCMD(CMD_APPMGR,     "Configure             ");
    else if ( state == "Configured" )
      m_window->replCMD(CMD_APPMGR,     "Initialize            ");
    else if ( state == "Initialized" )
      m_window->replCMD(CMD_APPMGR,     "Finalize              ");
    else if ( state == "Finalized" )
      m_window->replCMD(CMD_APPMGR,     "Initialize            ");
  }
}

void Gaudi::UPIService::setNewChild(Interactor* c)  
{
  if ( m_child ) delete m_child;
  m_child = c;
}

/// Show message in the message window
void Gaudi::UPIService::print(const char* fmt, ...)   {
  char buffer[1024];
  int len;
  va_list arguments;
  va_start( arguments, fmt);
  len = vsprintf(buffer,fmt,arguments);
  buffer[len] = 0;
  va_end ( arguments );
  upic_write_message(buffer,"");
}

/// Interactor overload: handle menu interupts
void Gaudi::UPIService::handle (const Event& event)   {
  StatusCode sc = StatusCode::SUCCESS;
  std::string to, state = m_appMgr->stateName();
  if ( event.eventtype == UpiEvent )  {
    switch ( event.eventtype )    
    {
    case UpiEvent:
      switch(event.command_id)  
      {
      case CMD_APPMGR:
        if ( state == "Offline" )
          sc = m_appMgr->configure(),  to = "Configured";
        else if ( state == "Configured" )
          sc = m_appMgr->initialize(), to = "Initialized";
        else if ( state == "Initialized" )
          sc = m_appMgr->finalize(),   to = "Finalized";
        else if ( state == "Finalized" )
          sc = m_appMgr->initialize(), to = "Initialized";
        else if ( state == "Offline" )
          IOCSENSOR.send(this, CMD_TERMINATE);
        if ( !sc.isSuccess() )  {
          print("Failed to initialize application manager transition from %s to %s",
            state.c_str(), to.c_str());
        }
        IOCSENSOR.send(this, CMD_UPDATE_STATE);
        break;
      case CMD_APPMGR_TERMINATE:
        sc = m_appMgr->terminate(), to = "Offline";
        if ( !sc.isSuccess() )  {
          print("Failed to initialize application manager transition from %s to %s",
            state.c_str(), to.c_str());
        }
        IOCSENSOR.send(this, CMD_TERMINATE);
        break;
      case CMD_TERMINATE:
        IOCSENSOR.send(this, CMD_TERMINATE);
        break;
      case CMD_DEBUG:
        ::lib_rtl_start_debugger();
        break;
      case CMD_SHOW_TOOLS:
        break;
      case CMD_SHOW_SERVICES:
        setNewChild(new Gaudi::ServiceMenu(serviceLocator(), this));
        break;
      case CMD_SHOW_ALGORITHMS:
        setNewChild(new Gaudi::AlgorithmMenu(serviceLocator(), this));
        break;
      case Gaudi::SubMenu::CMD_CLOSE:           // Close of submenus
        m_window->setCursor(CMD_SHOW_ALGORITHMS);
        break;
      default:
        break;
      }
    default:
      break;
    }
  }
  else if ( event.eventtype == IocEvent )  {
    switch(event.type)  
    {
      case CMD_TERMINATE:
        m_window->destroy();
        release();
        exit(0);
        return;
      case Gaudi::SubMenu::CMD_CLOSE:
        print("Deleting child");
        if ( m_child ) delete m_child;
        m_child = 0;
        break;
      case CMD_UPDATE_STATE:
        updateState(state);
        break;
    }
  }
}
