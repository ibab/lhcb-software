#if !defined(i386) && !defined(_WIN32)
#define GAUDIKERNEL_KERNEL_H    // disable include
#endif

#include "dis.hxx"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiOnline/DimTaskFSM.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"
#include <iostream>
#include <cstdarg>

#define ST_NAME_UNKNOWN     "UNKNOWN"
#define ST_NAME_ERROR       "ERROR"
#define ST_NAME_NOT_READY   "NOT_READY"
#define ST_NAME_READY       "READY"
//#define ST_NAME_STOPPED     "STOPPED"
#define ST_NAME_STOPPED     "READY"
#define ST_NAME_RUNNING     "RUNNING"
#define TR_COMPLETED        "Transition Completed"
#define TR_EXECUTING        "Executing Transition"
#define TR_FAILED           "Transition FAILED"
#define TR_UNKNOWN          "Unknown Transition"

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
      else if ( cmd == "stop"       ) m_target->disable();
      else if ( cmd == "reset"      ) IOCSENSOR.send(m_target, LHCb::DimTaskFSM::TERMINATE);
      else if ( cmd == "unload"     ) {
        m_target->cancel();
        IOCSENSOR.send(m_target, LHCb::DimTaskFSM::UNLOAD);
      }
      else   {
        m_target->declareSubState(TR_UNKNOWN);
        m_target->declareState(ST_NAME_ERROR);
        return;
      }
      m_target->declareSubState(TR_EXECUTING);
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
: m_name("Exec"), m_stateName(ST_NAME_UNKNOWN), 
  m_subStateName(TR_COMPLETED),
  m_haveEventLoop(false), m_refCount(1)
{
  m_propertyMgr  = new PropertyMgr(this);
  m_procName = RTL::processName();
  std::string svcname= m_procName+"/status";
  m_command = new Command(m_procName, this);
  m_service = new DimService(svcname.c_str(),(char*)m_stateName.c_str());
  svcname= m_procName+"/fsm_status";
  m_subStateService = new DimService(svcname.c_str(),(char*)m_subStateName.c_str());
  propertyMgr().declareProperty("HaveEventLoop",m_haveEventLoop);
  propertyMgr().declareProperty("Name",m_procName);
  //::lib_rtl_install_printer(printout,this);
}

LHCb::DimTaskFSM::~DimTaskFSM()  {
  delete m_subStateService;
  delete m_service;
  delete m_command;
  m_subStateService = 0;
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
    case ST_ERROR:
      return declareState(ST_NAME_ERROR);
    case ST_NOT_READY:
      return declareState(ST_NAME_NOT_READY);
    case ST_READY:
      return declareState(ST_NAME_READY);
    case ST_STOPPED:
      return declareState(ST_NAME_STOPPED);
    case ST_RUNNING:
      return declareState(ST_NAME_RUNNING);
    case ST_UNKNOWN:
    default:
      return declareState(ST_NAME_UNKNOWN);
  }
}

StatusCode LHCb::DimTaskFSM::declareSubState(const std::string& new_state)  {
  m_subStateName = new_state;
  m_subStateService->updateService((char*)m_subStateName.c_str());
  return StatusCode::SUCCESS;
}

/// Declare FSM sub-state
StatusCode LHCb::DimTaskFSM::declareSubState(SubState new_state)  {
  switch(new_state)   {
    case SUCCESS_ACTION:
      return declareSubState(TR_COMPLETED);
    case EXEC_ACTION:
      return declareSubState(TR_EXECUTING);
    case FAILED_ACTION:
      return declareSubState(TR_FAILED);
    case UNKNOWN_ACTION:
    default:
      return declareSubState(TR_UNKNOWN);
  }
}

StatusCode LHCb::DimTaskFSM::cancel()  {
  // Todo:  Need to somehow issue MBM Cancel!
  return StatusCode::SUCCESS;
}

void LHCb::DimTaskFSM::handle(const Event& ev)  {
  if(ev.eventtype == IocEvent)  {
#define _CASE(x)  case x: action = #x;
    const char* action = "UNKNOWN";
    StatusCode sc = StatusCode::FAILURE;
    try  {
      switch(ev.type) {
        _CASE(UNLOAD)       sc=unload();                              break;
        _CASE(CONFIGURE)    sc=configure();                           break;
        _CASE(INITIALIZE)   sc=initialize();                          break;
        _CASE(ENABLE)       sc=enable();                              break;
        _CASE(DISABLE)      sc=disable();                             break;
        _CASE(NEXTEVENT)    sc=nextEvent(1);                          break;
        _CASE(FINALIZE)     sc=finalize();                            break;
        _CASE(TERMINATE)    sc=terminate();                           break;
        _CASE(ERROR)        sc=declareState(ST_NAME_ERROR);           break;
        _CASE(STARTUP_DONE) sc = startupDone();                       break;
        default:  printErr(0,"Got Unkown action request:%d",ev.type); break;
      }
      sc.isSuccess() ? declareSubState(TR_COMPLETED) : declareSubState(TR_FAILED);
      return;
    }
    catch(const std::exception& e)  {
      std::string err="Exception executing action:";
      err += action;
      err += " [";
      err += e.what();
      err += "]";
      printErr(0,err.c_str());
      declareSubState(TR_FAILED);
    }
    catch(...)  {
      std::string err="Unknown exception executing action:";
      err += action;
      printErr(0,err.c_str());
      declareSubState(TR_FAILED);
    }
  }
  printErr(0,"Got Unkown event request.");
}

StatusCode LHCb::DimTaskFSM::startupDone()  {
  m_stateName = ST_NAME_NOT_READY;
  DimServer::start(m_procName.c_str());
  m_service->updateService((char*)m_stateName.c_str());
  return StatusCode::SUCCESS;
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
  return declareState(ST_NAME_STOPPED);
}

StatusCode LHCb::DimTaskFSM::terminate()  {
  declareState(ST_NAME_NOT_READY);
  //IOCSENSOR.send(this,UNLOAD);
  return StatusCode::SUCCESS;
}

StatusCode LHCb::DimTaskFSM::unload()  {
  declareState(ST_NAME_UNKNOWN);
  ::lib_rtl_sleep(100);
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
