#include "GaudiKernel/IMessageSvc.h"
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

DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,DimTaskFSM)

template <class T,class Q> static inline StatusCode cast_success(T* p, void** q)  {
  *q = (Q*)p;
  p->addRef();
  return StatusCode::SUCCESS;
}

namespace  {
  /** @class Command
    *
    *  @author M.Frank
    */
  class Command : public DimCommand  {
    /// Command target
    LHCb::DimTaskFSM* m_target;
  public:
    /// Constructor
    Command(const std::string& nam, LHCb::DimTaskFSM* target) 
    : DimCommand(nam.c_str(), "C"), m_target(target) { }
    /// DimCommand overload: handle DIM commands
    virtual void commandHandler()   {
      // Decauple as quickly as possible from the DIM command loop !
      std::string cmd = getString();
      m_target->output(MSG::DEBUG,std::string("Received DIM command:"+cmd).c_str());
      if      ( cmd == "configure"  ) IOCSENSOR.send(m_target, LHCb::DimTaskFSM::CONFIGURE);
      else if ( cmd == "start"      ) IOCSENSOR.send(m_target, LHCb::DimTaskFSM::INITIALIZE);
      else if ( cmd == "stop"       ) IOCSENSOR.send(m_target, LHCb::DimTaskFSM::DISABLE);
      else if ( cmd == "reset"      ) IOCSENSOR.send(m_target, LHCb::DimTaskFSM::TERMINATE);
      else if ( cmd == "unload"     ) IOCSENSOR.send(m_target, LHCb::DimTaskFSM::UNLOAD);
    }
  };

  /// Static printer (RTL overload)
  static size_t printout(void* context, const char* fmt, va_list args)  {
    std::string format = fmt;
    LHCb::DimTaskFSM* p = (LHCb::DimTaskFSM*)context;
    char buffer[2048];
    size_t len = ::vsprintf(buffer, format.substr(0,format.length()-1).c_str(), args);
    if ( len > sizeof(buffer) )  {
      // !! memory corruption !!
      p->output(MSG::ERROR,"Memory corruption...");
      p->output(MSG::WARNING,buffer);
      exit(0);
    }
    p->output(MSG::WARNING,buffer);
    return len;
  }
}

LHCb::DimTaskFSM::DimTaskFSM(IInterface*) 
: m_name("Exec"), m_stateName(ST_NAME_UNKNOWN), m_haveEventLoop(false), m_refCount(1)
{
  m_propertyMgr  = new PropertyMgr(this);
  m_procName = RTL::processName();
  std::string svcname= m_procName+"/status";
  m_command = new Command(m_procName, this);
	m_service = new DimService(svcname.c_str(),(char*)m_stateName.c_str());
  DimServer::start(m_procName.c_str());
  declareState(ST_NAME_NOT_READY);
  propertyMgr().declareProperty("HaveEventLoop",m_haveEventLoop);
  propertyMgr().declareProperty("Name",m_procName);
  ::lib_rtl_install_printer(printout,this);
}

LHCb::DimTaskFSM::~DimTaskFSM()  {
  delete m_service;
  delete m_command;
  m_service = 0;
  m_command = 0;
  ::lib_rtl_install_printer(0,0);
}

/// IInterface implementation: DimTaskFSM::addRef()
unsigned long LHCb::DimTaskFSM::addRef() {
  m_refCount++;
  return m_refCount;
}

/// IInterface implementation: DimTaskFSM::release()
unsigned long LHCb::DimTaskFSM::release() {
  long count = --m_refCount;
  if( count <= 0) {
    delete this;
  }
  return count;
}

/// IInterface implementation: LHCb::DimTaskFSM::queryInterface()
StatusCode LHCb::DimTaskFSM::queryInterface(const InterfaceID& iid,void** ppvIf) {
  if( iid == IID_IInterface )
    return cast_success<DimTaskFSM,IInterface>(this,ppvIf);
  else if( iid == IID_IRunable )
    return cast_success<DimTaskFSM,IRunable>(this,ppvIf);
  else if( iid == IID_IAppMgrUI )
    return cast_success<DimTaskFSM,IAppMgrUI>(this,ppvIf);
  else if ( iid == IID_IProperty )
    return m_propertyMgr->queryInterface(iid, ppvIf);
  else
    return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

StatusCode LHCb::DimTaskFSM::run()  {
  IOCSENSOR.run();
  return StatusCode::SUCCESS;
}

void LHCb::DimTaskFSM::output(int /* level */, const char* s)  {
  std::cout << s << std::endl;
}

StatusCode LHCb::DimTaskFSM::printErr(int flag, const char* format, ...)  {
  va_list args;
  char buffer[1024];
  sprintf(buffer,"Error: ");
  va_start( args, format );
  size_t len = ::vsprintf(&buffer[7], format, args);
  if ( len > sizeof(buffer) )  {
    // !! memory corruption !!
    output(MSG::ERROR,"Memory corruption...");
    output(MSG::ERROR,buffer);
    exit(0);
  }
  output(MSG::ERROR,buffer);
  if ( flag ) error();
  return StatusCode::FAILURE;
}

StatusCode LHCb::DimTaskFSM::declareState(const std::string& new_state)  {
  m_stateName = new_state;
  output(MSG::DEBUG,std::string("Declare state:"+new_state).c_str());
  m_service->updateService((char*)m_stateName.c_str());
  return StatusCode::SUCCESS;
}

StatusCode LHCb::DimTaskFSM::declareState(State new_state)  {
  switch(new_state)   {
    case ERROR:
      return declareState(ST_NAME_ERROR);
    case NOT_READY:
      return declareState(ST_NAME_NOT_READY);
    case READY:
      return declareState(ST_NAME_READY);
    case RUNNING:
      return declareState(ST_NAME_RUNNING);
    case UNKNOWN:
    default:
      return declareState(ST_NAME_UNKNOWN);
  }
}

StatusCode LHCb::DimTaskFSM::cancel()  {
  // Todo:  Need to somehow issue MBM Cancel!
  return StatusCode::SUCCESS;
}

void LHCb::DimTaskFSM::handle(const Event& ev)  {
  if(ev.eventtype == IocEvent)  {
    switch(ev.type) {
      case UNLOAD:      unload();                                  return;
      case CONFIGURE:   configure();                               return;
      case INITIALIZE:  initialize();                              return;
      case ENABLE:      enable();                                  return;
      case DISABLE:     disable();                                 return;
      case NEXTEVENT:   nextEvent(1);                              return;
      case FINALIZE:    finalize();                                return;
      case TERMINATE:   terminate();                               return;
      case ERROR:       declareState(ST_NAME_ERROR);               return;
      default:
        printErr(0,"Got Unkown action request:%d",ev.type);
        return;
    }
  }
  printErr(0,"Got Unkown event request.");
}

StatusCode LHCb::DimTaskFSM::configure()  {
  return declareState(ST_NAME_READY);
}
	
StatusCode LHCb::DimTaskFSM::initialize()  {
  IOCSENSOR.send(this, ENABLE);
  return StatusCode::SUCCESS;
}
	
StatusCode LHCb::DimTaskFSM::enable()  {
  m_continue = true;
  IOCSENSOR.send(this, NEXTEVENT);
  return declareState(ST_NAME_RUNNING);
}

StatusCode LHCb::DimTaskFSM::rearm()  {
  if ( m_haveEventLoop ) IOCSENSOR.send(this, NEXTEVENT);
  return StatusCode::SUCCESS;
}

StatusCode LHCb::DimTaskFSM::nextEvent(int /* num_event */)  {
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
