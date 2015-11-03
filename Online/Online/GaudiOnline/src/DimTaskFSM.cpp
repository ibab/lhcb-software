#include "GaudiKernel/Algorithm.h"
#define NO_LONGLONG_TYPEDEF
#include "dis.hxx"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiOnline/DimTaskFSM.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"
#include "RTL/strdef.h"
#include <iostream>
#include <cstdarg>

#define ST_NAME_UNKNOWN     "UNKNOWN"
#define ST_NAME_ERROR       "ERROR"
#define ST_NAME_NOT_READY   "NOT_READY"
#define ST_NAME_READY       "READY"
#define ST_NAME_ACTIVE      "ACTIVE"
//#define ST_NAME_STOPPED     "STOPPED"
#define ST_NAME_STOPPED     "READY"
#define ST_NAME_RUNNING     "RUNNING"
#define ST_NAME_PAUSED      "PAUSED"

using namespace LHCb;

static ITaskFSM* s_dimtask_instance = 0;
extern "C" void* DimTaskFSM_instance() {
  return s_dimtask_instance;
}

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
    typedef DimTaskFSM Target;
    /// Command target
    Target* m_target;
  public:
    /// Constructor
    Command(const std::string& nam, DimTaskFSM* target)
      : DimCommand(nam.c_str(), (char*)"C"), m_target(target) { }
    /// DimCommand overload: handle DIM commands
    virtual void commandHandler()   {
      // Decouple as quickly as possible from the DIM command loop !
      std::string cmd = getString();
      //m_target->output(MSG::DEBUG,std::string("Received DIM command:"+cmd).c_str());
      if      ( cmd == "load"  ) {  // Ignore!
        return;
      }
      else if ( cmd == "configure"  ) {
        m_target->setTargetState(Target::ST_READY);
        IOCSENSOR.send(m_target, Target::CONFIGURE);
      }
      else if ( cmd == "activate"   ) {
        m_target->setTargetState(Target::ST_ACTIVE);
        IOCSENSOR.send(m_target, Target::ACTIVATE);
      }
      else if ( cmd == "go"         ) {
        m_target->setTargetState(Target::ST_RUNNING);
        IOCSENSOR.send(m_target, Target::GO);
      }
      else if ( cmd == "start"      ) {
        m_target->setTargetState(Target::ST_RUNNING);
        IOCSENSOR.send(m_target, Target::INITIALIZE);
      }
      else if ( cmd == "stop"       ) {
        m_target->setTargetState(Target::ST_STOPPED);
        IOCSENSOR.send(m_target, Target::DISABLE);
      }
      else if ( cmd == "reset"      ) {
        m_target->setTargetState(Target::ST_UNKNOWN);
        IOCSENSOR.send(m_target, Target::TERMINATE);
      }
      else if ( cmd == "pause"      ) {
        m_target->setTargetState(Target::ST_PAUSED);
        IOCSENSOR.send(m_target, DimTaskFSM::PAUSE);
      }
      else if ( cmd == "continue"   ) {
        m_target->setTargetState(Target::ST_RUNNING);
        IOCSENSOR.send(m_target, DimTaskFSM::CANCEL);
        IOCSENSOR.send(m_target, DimTaskFSM::CONTINUE);
      }
      else if ( cmd == "unload"     ) {
        m_target->setTargetState(Target::ST_UNKNOWN);
        IOCSENSOR.send(m_target, DimTaskFSM::CANCEL);
        IOCSENSOR.send(m_target, DimTaskFSM::UNLOAD);
      }
      else if ( cmd == "recover"    ) {
        m_target->setTargetState(Target::ST_UNKNOWN);
        IOCSENSOR.send(m_target, DimTaskFSM::CANCEL);
        IOCSENSOR.send(m_target, DimTaskFSM::UNLOAD);
      }
      else if ( cmd == "cancel"     ) {
        IOCSENSOR.send(m_target, DimTaskFSM::CANCEL);
      }
      else if ( cmd == "RESET"      ) {
        m_target->setTargetState(Target::ST_UNKNOWN);
        IOCSENSOR.send(m_target, DimTaskFSM::CANCEL);
        IOCSENSOR.send(m_target, DimTaskFSM::UNLOAD);
      }
      else if ( cmd == "error" )  {
        m_target->setTargetState(Target::ST_ERROR);
        m_target->cancel();
        m_target->declareState(Target::ST_ERROR);
        return;
      }
      else if ( cmd == "stop_trigger" )   {
        std::string* value = new std::string("DAQ_STOP_TRIGGER");
        IOCSENSOR.send(m_target, DimTaskFSM::FIRE_INCIDENT, value);
        return;
      }
      else if ( cmd == "start_trigger" )   {
        std::string* value = new std::string("DAQ_START_TRIGGER");
        IOCSENSOR.send(m_target, DimTaskFSM::FIRE_INCIDENT, value);
        return;
      }
      else if ( cmd == "!state" )  {
        std::string old_state = m_target->stateName();
        m_target->clearState();
        m_target->_declareState(old_state);
        return;
      }
      else if ( ::strncmp(cmd.c_str(),"incident__",10) == 0 )  {
        IOCSENSOR.send(m_target, DimTaskFSM::FIRE_INCIDENT, new std::string(cmd.c_str()+10));
        return;
      }
      else   {
        m_target->declareState(Target::ST_ERROR);
        m_target->declareSubState(Target::UNKNOWN_ACTION);
        return;
      }
      m_target->declareSubState(Target::EXEC_ACTION);
    }
  };

  /// Static printer (RTL overload)
  static size_t printout(void* context, int level, const char* fmt, va_list args)  {
    std::string format = fmt;
    DimTaskFSM* p = (DimTaskFSM*)context;
    char buffer[2048];
    size_t len = ::vsnprintf(buffer, sizeof(buffer), format.substr(0,format.length()-1).c_str(), args);
    p->output(level, buffer);
    return len;
  }
}

DimTaskFSM::DimTaskFSM(IInterface*)
  : m_name(RTL::processName()), m_stateName(ST_NAME_UNKNOWN), m_prevStateName(ST_NAME_UNKNOWN),
    m_command(0), m_service(0), m_fsmService(0), m_haveEventLoop(false), m_refCount(1)
{
  s_dimtask_instance = this;
  m_propertyMgr  = new PropertyMgr(this);
  m_monitor.targetState = m_monitor.state = ST_NOT_READY;
  m_monitor.lastCmd     = m_monitor.doneCmd = (int)::time(0);
  m_monitor.metaState   = SUCCESS_ACTION;
  m_monitor.pid         = ::lib_rtl_pid();
  m_monitor.partitionID = -1;
  m_monitor.pad         = 0;
  propertyMgr().declareProperty("HaveEventLoop",m_haveEventLoop);
  propertyMgr().declareProperty("Name",m_name);
  ::lib_rtl_install_printer(printout,this);
  connectDIM().ignore();
}

DimTaskFSM::~DimTaskFSM()  {
  s_dimtask_instance = 0;
  disconnectDIM();
  ::lib_rtl_install_printer(0,0);
}

StatusCode DimTaskFSM::connectDIM(DimCommand* cmd) {
  std::string svcname;
  m_name        = RTL::processName();
  m_monitor.pid = ::lib_rtl_pid();
  svcname       = m_name+"/status";

  dim_init();
  DimServer::autoStartOff();
  m_command = cmd ? cmd : new Command(m_name, this);
  m_service = new DimService(svcname.c_str(),(char*)m_stateName.c_str());
  svcname   = m_name+"/fsm_status";
  m_fsmService = new DimService(svcname.c_str(),(char*)"L:2;I:1;C",&m_monitor,sizeof(m_monitor));
  DimServer::autoStartOn();
  DimServer::start(m_name.c_str());
  return StatusCode::SUCCESS;
}

StatusCode DimTaskFSM::disconnectDIM() {
  if ( m_fsmService ) delete m_fsmService;
  if ( m_service ) delete m_service;
  if ( m_command ) delete m_command;
  m_fsmService = 0;
  m_service = 0;
  m_command = 0;
  DimServer::stop();
  return StatusCode::SUCCESS;
}

/// IInterface implementation: DimTaskFSM::addRef()
unsigned long DimTaskFSM::addRef() {
  m_refCount++;
  return m_refCount;
}

/// IInterface implementation: DimTaskFSM::release()
unsigned long DimTaskFSM::release() {
  long count = --m_refCount;
  if( count <= 0) {
    delete this;
  }
  return count;
}

/// IInterface implementation: DimTaskFSM::queryInterface()
StatusCode DimTaskFSM::queryInterface(const InterfaceID& iid,void** ppvIf) {
  if( iid == IInterface::interfaceID() )
    return cast_success<DimTaskFSM,IInterface>(this,ppvIf);
  else if( iid == IRunable::interfaceID() )
    return cast_success<DimTaskFSM,IRunable>(this,ppvIf);
  else if( iid == IAppMgrUI::interfaceID() )
    return cast_success<DimTaskFSM,IAppMgrUI>(this,ppvIf);
  else if ( iid == IProperty::interfaceID() )
    return m_propertyMgr->queryInterface(iid, ppvIf);
  return StatusCode::FAILURE;
}

StatusCode DimTaskFSM::run()  {
  IOCSENSOR.run();
  return StatusCode::SUCCESS;
}

void DimTaskFSM::output(int /* level */, const char* s)  {
  std::cout << s << std::endl;
}

StatusCode DimTaskFSM::printErr(int flag, const char* format, ...)  {
  va_list args;
  char buffer[1024];
  ::snprintf(buffer,sizeof(buffer),"Error: ");
  va_start(args, format);
  size_t len = ::vsnprintf(&buffer[7], sizeof(buffer)-8, format, args);
  va_end(args);
  buffer[sizeof(buffer)-1] = 0;
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

/// Translate integer state to string name
std::string DimTaskFSM::stateName(int state) {
  switch(state) {
  case ST_NOT_READY:
    return ST_NAME_NOT_READY;
  case ST_READY:
    return ST_NAME_READY;
  case ST_ACTIVE:
    return ST_NAME_ACTIVE;
  case ST_RUNNING:
    return ST_NAME_RUNNING;
  case ST_STOPPED:
    return ST_NAME_STOPPED;
  case ST_PAUSED:
    return ST_NAME_PAUSED;
  case ST_ERROR:
    return ST_NAME_ERROR;
  case ST_UNKNOWN:
  default:
    return ST_NAME_UNKNOWN;
  }
}
#include <sstream>
/// Declare process state to DIM service
StatusCode DimTaskFSM::_declareState(const std::string& new_state)  {
  std::string old_state = m_stateName;
  m_prevStateName = m_stateName;
  m_stateName = new_state;
  output(MSG::DEBUG,std::string("PUBLISH state:"+new_state).c_str());
  m_service->updateService((char*)m_stateName.c_str());
  if ( new_state == ST_NAME_ERROR )  {
#if 0
    std::stringstream str;
    str << m_name << "PUBLISH state:" << new_state << " PID:" << ::lib_rtl_pid() << std::ends;
    output(MSG::ALWAYS,str.str().c_str());
    output(MSG::ALWAYS,str.str().c_str());
    ::lib_rtl_sleep(40000);
#endif
    declareSubState(FAILED_ACTION);
  }
  else if ( old_state == new_state )
    declareSubState(FAILED_ACTION);
  else
    declareSubState(SUCCESS_ACTION);
  return StatusCode::SUCCESS;
}

/// Declare process state to DIM service
StatusCode DimTaskFSM::declareState(State new_state)  {
  m_monitor.state = char(new_state);
  switch(new_state)   {
  case TR_ERROR:
  case ST_ERROR:
    return _declareState(ST_NAME_ERROR);
  case ST_NOT_READY:
    return _declareState(ST_NAME_NOT_READY);
  case ST_READY:
    return _declareState(ST_NAME_READY);
  case ST_ACTIVE:
    return _declareState(ST_NAME_ACTIVE);
  case ST_STOPPED:
    return _declareState(ST_NAME_STOPPED);
  case ST_RUNNING:
    return _declareState(ST_NAME_RUNNING);
  case ST_PAUSED:
    return _declareState(ST_NAME_PAUSED);
  case ST_UNKNOWN:
  default:
    m_monitor.state = ST_UNKNOWN;
    return _declareState(ST_NAME_UNKNOWN);
  }
}

/// Declare FSM sub-state
StatusCode DimTaskFSM::declareSubState(SubState new_state)  {
  m_monitor.metaState = char(new_state);
  switch(new_state)   {
  case SUCCESS_ACTION:
    m_monitor.doneCmd = (int)::time(0);
    break;
  case EXEC_ACTION:
    m_monitor.lastCmd = (int)::time(0);
    break;
  case FAILED_ACTION:
    m_monitor.doneCmd = (int)::time(0);
    break;
  case UNKNOWN_ACTION:
  default:
    m_monitor.doneCmd = (int)::time(0);
    m_monitor.metaState = ST_UNKNOWN;
    break;
  }
  // std::cout << "Update substate..." << std::endl;
  m_fsmService->updateService(&m_monitor,sizeof(m_monitor));
  return StatusCode::SUCCESS;
}

StatusCode DimTaskFSM::cancel()  {
  // Todo:  Need to somehow issue MBM Cancel!
  return StatusCode::SUCCESS;
}

void DimTaskFSM::handle(const Event& ev)  {
  if(ev.eventtype == IocEvent)  {
#define _CASE(x)  case x: action = #x;
    const char* action = "UNKNOWN";
    StatusCode sc = StatusCode::FAILURE;
    try  {
      switch(ev.type) {
        _CASE(UNLOAD)       sc=unload();                              break;
        _CASE(CONFIGURE)    sc=configure();                           break;
        _CASE(INITIALIZE)   sc=initialize();                          break;
        _CASE(ACTIVATE)     sc=activate();                            break;
        _CASE(GO)           sc=go();                                  break;
        _CASE(START)        sc=start();                               break;
        _CASE(ENABLE)       sc=enable();                              break;
        _CASE(DISABLE)      sc=disable();                             break;
        _CASE(CANCEL)       sc=cancel();                              break;
        _CASE(NEXTEVENT)    sc=nextEvent(1);                          break;
        _CASE(STOP)         sc=stop();                                break;
        _CASE(FINALIZE)     sc=finalize();                            break;
        _CASE(TERMINATE)    sc=terminate();                           break;
        _CASE(PAUSE)        sc=pause();                               break;
        _CASE(CONTINUE)     sc=continuing();                          break;
        _CASE(ERROR)        sc=declareState(ST_ERROR);                break;
        _CASE(STARTUP_DONE) sc = startupDone();                       break;
        _CASE(CONNECT_DIM)  sc = connectDIM();                        break;
        _CASE(FIRE_INCIDENT)sc = fireIncident(*(std::string*)ev.data);
        delete (std::string*)ev.data;                                 break;
      default:  printErr(0,"Got Unkown action request:%d",ev.type); break;
      }
      sc.isSuccess() ? declareSubState(SUCCESS_ACTION) : declareSubState(FAILED_ACTION);
      return;
    }
    catch(const std::exception& e)  {
      std::string err="Exception executing action:";
      err += action;
      err += " [";
      err += e.what();
      err += "]";
      printErr(0,err.c_str());
      declareSubState(FAILED_ACTION);
    }
    catch(...)  {
      std::string err="Unknown exception executing action:";
      err += action;
      printErr(0,err.c_str());
      declareSubState(FAILED_ACTION);
    }
  }
  printErr(0,"Got Unkown event request.");
}

StatusCode DimTaskFSM::startupDone()  {
  m_stateName = ST_NAME_NOT_READY;
  //  DimServer::autoStartOn();
  //  DimServer::start(m_name.c_str());
  //  ::dis_start_serving((char*)m_name.c_str());
  declareState(ST_NOT_READY);
  return StatusCode::SUCCESS;
}

StatusCode DimTaskFSM::configure()  {
  return declareState(ST_READY);
}

StatusCode DimTaskFSM::initialize()  {
  IOCSENSOR.send(this, START);
  return StatusCode::SUCCESS;
}

/// Reinitialize the application
StatusCode DimTaskFSM::reinitialize()   {
  return StatusCode::SUCCESS;
}

/// Callback on "activate" command
StatusCode DimTaskFSM::activate()  {
  return declareState(ST_ACTIVE);
}

/// Callback on "go" command
StatusCode DimTaskFSM::go()  {
  IOCSENSOR.send(this, ENABLE);
  return StatusCode::SUCCESS;
}

/// Start the application
StatusCode DimTaskFSM::start()   {
  IOCSENSOR.send(this, ENABLE);
  return StatusCode::SUCCESS;
}

/// Restart the application
StatusCode DimTaskFSM::restart()   {
  IOCSENSOR.send(this, STOP);
  IOCSENSOR.send(this, START);
  return StatusCode::SUCCESS;
}

/// Stop the application
StatusCode DimTaskFSM::stop()   {
  IOCSENSOR.send(this, FINALIZE);
  return StatusCode::SUCCESS;
}

/// Access FSM state
Gaudi::StateMachine::State DimTaskFSM::FSMState() const {
  return (Gaudi::StateMachine::State)0;
}

StatusCode DimTaskFSM::enable()  {
  m_continue = true;
  IOCSENSOR.send(this, NEXTEVENT);
  return declareState(ST_RUNNING);
}

StatusCode DimTaskFSM::rearm()  {
  if ( m_haveEventLoop ) IOCSENSOR.send(this, NEXTEVENT);
  return StatusCode::SUCCESS;
}

StatusCode DimTaskFSM::nextEvent(int /* num_event */)  {
  if ( m_continue )  {
    rearm();
  }
  return StatusCode::SUCCESS;
}

StatusCode DimTaskFSM::disable()  {
  m_continue = false;
  cancel();
  IOCSENSOR.send(this, STOP);
  return StatusCode::SUCCESS;
}

StatusCode DimTaskFSM::finalize()  {
  m_continue = false;
  cancel();
  return declareState(ST_STOPPED);
}

StatusCode DimTaskFSM::terminate()  {
  declareState(ST_NOT_READY);
  return StatusCode::SUCCESS;
}

StatusCode DimTaskFSM::unload()  {
  declareState(ST_UNKNOWN);
  ::lib_rtl_sleep(100);
  ::exit(0);
  return StatusCode::SUCCESS;
}

/// Pause the application  ( RUNNING -> PAUSED )
StatusCode DimTaskFSM::pause()  {
  return pauseProcessing();
}

/// Pause the application  ( RUNNING -> PAUSED )
StatusCode DimTaskFSM::pauseProcessing()  {
  m_continue = false;
  cancel();
  return declareState(ST_PAUSED);
}

/// Continue the application  ( PAUSED -> RUNNING )
StatusCode DimTaskFSM::continuing()  {
  return continueProcessing();
}

/// Continue the application  ( PAUSED -> RUNNING )
StatusCode DimTaskFSM::continueProcessing() {
  m_continue = true;
  rearm();
  return declareState(ST_RUNNING);
}

/// Invoke transition to error state                ( ****      -> Error   )
StatusCode DimTaskFSM::error()  {
  m_continue = false;
  cancel();
  IOCSENSOR.send(this, ERROR);
  return StatusCode::SUCCESS;
}

/// Fire an incident from network interrupt
StatusCode DimTaskFSM::fireIncident(const std::string& )   {
  return StatusCode::SUCCESS;
}
