#include "GaudiOnline/DimTaskFSM.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"
#include <iostream>
#include <cstdarg>
#include "dis.hxx"

#define ST_NAME_UNKNOWN     "UNKNOWN"
#define ST_NAME_ERROR       "ERROR"
#define ST_NAME_NOT_READY   "NOT_READY"
#define ST_NAME_READY       "READY"
#define ST_NAME_RUNNING     "RUNNING"

namespace  {
  /** @class Command
    *
    *  @author M.Frank
    */
  class Command : public DimCommand  {
    /// Command target
    Interactor* m_target;
  public:
    /// Constructor
    Command(const std::string& nam, Interactor* target) 
    : DimCommand(nam.c_str(), "C"), m_target(target) { }
    /// DimCommand overload: handle DIM commands
    virtual void commandHandler()   {
      // Decauple as quickly as possible from the DIM command loop !
      std::string cmd = getString();
      std::cout << "Received DIM command:" << cmd << std::endl;
      if      ( cmd == "config"     ) IOCSENSOR.send(m_target, LHCb::DimTaskFSM::CONFIGURE);
      else if ( cmd == "start"      ) IOCSENSOR.send(m_target, LHCb::DimTaskFSM::INITIALIZE);
      else if ( cmd == "stop"       ) IOCSENSOR.send(m_target, LHCb::DimTaskFSM::DISABLE);
      else if ( cmd == "reset"      ) IOCSENSOR.send(m_target, LHCb::DimTaskFSM::TERMINATE);
      else if ( cmd == "unload"     ) IOCSENSOR.send(m_target, LHCb::DimTaskFSM::UNLOAD);
    }
  };
}

LHCb::DimTaskFSM::DimTaskFSM(bool loop) 
: m_stateName(ST_NAME_UNKNOWN), m_haveEventLoop(loop)
{
  char txt[64];
  ::lib_rtl_get_process_name(txt, sizeof(txt));
  m_procName = txt;
  std::string svcname= m_procName+"/status";
  print("Task name:'%s'\n",m_procName.c_str());
  m_command = new Command(m_procName, this);
	m_service = new DimService(svcname.c_str(),(char*)m_stateName.c_str());
  DimServer::start(m_procName.c_str());
  declareState(ST_NAME_UNKNOWN);
}

LHCb::DimTaskFSM::~DimTaskFSM()  {
  delete m_service;
  m_service = 0;
}

StatusCode LHCb::DimTaskFSM::run() const  {
  IOCSENSOR.run();
  return StatusCode::SUCCESS;
}

/// Printout overload
size_t LHCb::DimTaskFSM::print(const char* format,...)   {
  va_list args;
  char buffer[1024];
  va_start( args, format );
  size_t len = ::vsprintf(buffer, format, args);
  if ( len > sizeof(buffer) )  {
    // !! memory corruption !!
    output("Memory corruption...");
    output(buffer);
    exit(0);
  }
  output(buffer);
  return len;
}

void LHCb::DimTaskFSM::output(const char* s)  {
  std::cout << s << std::endl;
}

StatusCode LHCb::DimTaskFSM::printErr(int flag, const std::string& msg)  {
  print("Error: %s", msg.c_str());
  if ( flag ) error();
  return StatusCode::FAILURE;
}

StatusCode LHCb::DimTaskFSM::declareState(const std::string& new_state)  {
  m_stateName = new_state;
  output(std::string("Declare state:"+new_state).c_str());
  m_service->updateService((char*)m_stateName.c_str());
  return StatusCode::SUCCESS;
}

StatusCode LHCb::DimTaskFSM::cancel()  {
  // Todo:  Need to somehow issue MBM Cancel!
  return StatusCode::SUCCESS;
}

void LHCb::DimTaskFSM::handle(const Event& ev)  {
  if(ev.eventtype == IocEvent)  {
    switch(ev.type) {
      case UNLOAD:      unload();                                  return;
      case CONFIGURE:   config();                                  return;
      case INITIALIZE:  init();                                    return;
      case ENABLE:      enable();                                  return;
      case DISABLE:     disable();                                 return;
      case PROCESS:     process();                                 return;
      case FINALIZE:    finalize();                                return;
      case TERMINATE:   terminate();                               return;
      case ERROR:       declareState(ST_NAME_ERROR);               return;
      default:          printErr(0,"Got Unkown action request.");  return;
    }
  }
  printErr(0,"Got Unkown event request.");
}

StatusCode LHCb::DimTaskFSM::config()  {
  return declareState(ST_NAME_READY);
}
	
StatusCode LHCb::DimTaskFSM::init()  {
  IOCSENSOR.send(this, ENABLE);
  return StatusCode::SUCCESS;
}
	
StatusCode LHCb::DimTaskFSM::enable()  {
  m_continue = true;
  IOCSENSOR.send(this, PROCESS);
  return declareState(ST_NAME_RUNNING);
}

StatusCode LHCb::DimTaskFSM::rearm()  {
  if ( m_haveEventLoop ) IOCSENSOR.send(this, PROCESS);
  return StatusCode::SUCCESS;
}

StatusCode LHCb::DimTaskFSM::process()  {
  if ( m_continue )  {
    rearm();
  }
  return StatusCode::SUCCESS;
}
	
StatusCode LHCb::DimTaskFSM::disable()  {
  m_continue = false;
  cancel();
  IOCSENSOR.send(this, FINALIZE);
  return StatusCode::SUCCESS;
}

StatusCode LHCb::DimTaskFSM::finalize()  {
  m_continue = false;
  cancel();
  return declareState(ST_NAME_READY);
}

StatusCode LHCb::DimTaskFSM::terminate()  {
  return declareState(ST_NAME_NOT_READY);
}

StatusCode LHCb::DimTaskFSM::unload()  {
  declareState(ST_NAME_UNKNOWN);
  ::exit(0);
  return StatusCode::SUCCESS;
}

/// Invoke transition to error state                ( ****      -> Error   )
StatusCode LHCb::DimTaskFSM::error()  {
  m_continue = false;
  cancel();
  IOCSENSOR.send(this, ERROR);
  return StatusCode::SUCCESS;
}
