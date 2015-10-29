/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/
// Framework include files
#include "FiniteStateMachine/Type.h"
#include "FiniteStateMachine/Slave.h"
#include "FiniteStateMachine/Machine.h"
#include "CPP/IocSensor.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <stdexcept>

using namespace FiniteStateMachine;
using namespace std;

/// Access meta state as string
static const char* _metaStateName(int meta)   {
  switch(meta)  {
#define MakeName(x) case Slave::x: return #x
    MakeName(SLAVE_NONE);
    MakeName(SLAVE_LIMBO);
    MakeName(SLAVE_STARTING);
    MakeName(SLAVE_ALIVE);
    MakeName(SLAVE_FAILED);
    MakeName(SLAVE_KILLED);
    MakeName(SLAVE_DEAD);
    MakeName(SLAVE_EXECUTING);
    MakeName(SLAVE_TRANSITION);
    MakeName(SLAVE_FINISHED);
    // Timeout handling
    MakeName(SLAVE_TIMEOUT);
    MakeName(SLAVE_UNLOAD_TIMEOUT);
    MakeName(SLAVE_TERMINATE_TIMEOUT);
    MakeName(SLAVE_KILL_TIMEOUT);
    MakeName(SLAVE_START_TIMEOUT);
#undef  MakeName
  }
  return "SLAVE_UNKNOWN";
}

/// Class Constructor
Slave::Slave(const Type *typ, const string& nam, Machine* machine, bool internal)
  : TypedObject(typ,nam), m_timerID(0,0), m_tmo(5), 
    m_meta(SLAVE_LIMBO), m_status(SLAVE_LIMBO), m_machine(machine), m_state(0), 
    m_rule(0), m_instanceTag(0), m_internal(internal), m_mayStart(!internal), 
    m_alive(false), m_answered(false)
{
  m_state = type()->initialState();
}

/// Standatrd destructor  
Slave::~Slave()    {
}

/// Access meta state as string
const char* Slave::metaStateName() const  {
  return _metaStateName(m_meta);
}

/// Access meta state as string
const char* Slave::metaName(SlaveState st)   {
  return _metaStateName(st);
}

/// Retrieve reference to current State structure name
const char* Slave::statusName ()  const
{
  return _metaStateName(m_status);
}

/// Retrieve reference to current State structure name
const char* Slave::c_state ()  const
{
  return State::c_name(m_state);
}

/// Retrieve reference to managing machine structure name
const char* Slave::c_machine ()  const
{
  return Machine::c_name(m_machine);
}

/// Send IOC interrupt to slave
FSM::ErrCond Slave::send(int code, const State* state)   const {
  const Interactor* actor = this;
  IocSensor::instance().send(const_cast<Interactor*>(actor),code,(void*)state); 
  return FSM::SUCCESS;
}

FSM::ErrCond Slave::notifyMachine(int meta_state)  {
  display(DEBUG,c_name(),"Slave [%s] notify machine: %s",metaStateName(),m_machine->c_name());
  IocSensor::instance().send((Interactor*)m_machine,meta_state,(void*)this); 
  return FSM::SUCCESS;
}

/// Callback on alive signs of slave process
FSM::ErrCond Slave::iamHere()  {
  m_alive  = true;
  m_meta   = SLAVE_ALIVE;
  m_status = SLAVE_ALIVE;
  m_state  = type()->initialState();
  if ( !m_machine->currTrans() )
    return notifyMachine(SLAVE_TRANSITION);
  return m_machine->checkAliveSlaves();
}

/// Callback on sudden death of slave process
FSM::ErrCond Slave::iamDead()  {
  m_rule   = 0;
  m_alive  = false;
  m_meta   = SLAVE_LIMBO;
  m_status = SLAVE_LIMBO;
  setInternal(!m_mayStart);
  m_state  = type()->initialState();
  if ( !m_machine->currTrans() )
    return notifyMachine(SLAVE_TRANSITION);
  return m_machine->checkSlaves();
}

/// Callback, when transition was executed successfully
FSM::ErrCond Slave::transitionDone(const State* state)  {
  m_alive  = true;
  m_rule   = 0;
  m_state  = state;
  m_meta   = SLAVE_ALIVE;
  m_status = SLAVE_FINISHED;
  if ( !m_machine->currTrans() )
    return notifyMachine(SLAVE_TRANSITION);
  return m_machine->checkSlaves();
}

/// Callback, when the slave invoked a transition himself
FSM::ErrCond Slave::transitionSlave(const State* state)  {
  if ( state )  {
    m_rule  = 0;
    m_alive = true;
    m_state = state;
    m_meta  = SLAVE_ALIVE;
    if ( state->isFailure() ) m_status = SLAVE_FAILED;
    else m_status = SLAVE_ALIVE;
    // State change by slave. Handle the request.
    return notifyMachine(SLAVE_TRANSITION);
  }
  return FSM::FAIL;
}

/// Callback, when transition failed (called on receipt of answer message)
FSM::ErrCond Slave::transitionFailed()  {
  m_meta   = SLAVE_FAILED;
  m_status = SLAVE_FAILED;
  m_state  = m_rule ? m_rule->currState() : m_state;
  return notifyMachine(SLAVE_FAILED);
}

/// Invoke transition on slave. Noop if slave is already in target state
FSM::ErrCond Slave::apply(const Rule* rule)  {
  const Transition* tr = rule->transition();
  if ( !tr ) tr = m_state->findTrans(rule->targetState());
  m_rule = 0;
  if ( tr )  {
    m_rule = rule;
    m_meta = SLAVE_EXECUTING;
    display(INFO,c_name(),"%s sending '%s' to move from %s to %s. Rule:%s",
	    m_machine->c_name(),tr->c_name(),State::c_name(m_state),State::c_name(tr->to()),
	    Rule::c_name(m_rule));
    m_answered = false;
    if ( isInternal() ) {
      return send(SLAVE_FINISHED,tr->to());
    }
    ErrCond ret = sendRequest(tr);
    startTimer(SLAVE_TIMEOUT,tr);
    return ret;
  }
  return FSM::TRANNOTFOUND;
}

/// Start slave process
FSM::ErrCond Slave::startSlave(const Transition* tr)  {
  FSM::ErrCond ret = FSM::FAIL;
  inquireState();
  if ( isInternal() )  {
    send(SLAVE_ALIVE,tr->to());
    return FSM::WAIT_ACTION;
  }
  else if ( m_mayStart )  {  // Formerly internal slaves may NOT be started
    ret = start();
    startTimer(SLAVE_START_TIMEOUT,tr);
  }
  else  {                    // Here simply set timeout
    startTimer(1,tr);
  }
  return ret;
}

/// Start slave process
FSM::ErrCond Slave::killSlave()  {
  if ( isInternal() )  {
    send(SLAVE_LIMBO,0);
    return FSM::WAIT_ACTION;
  }
  ErrCond ret = kill();
  startTimer(SLAVE_UNLOAD_TIMEOUT);
  return ret;
}

/// Add entry in transition timeout table (timeout in seconds)
Slave& Slave::addTimeout(const Transition* param, int value)  {
  m_timeouts[param] = value;
  return *this;
}

/// Remove entry in transition timeout table
Slave& Slave::removeTimeout(const Transition* param)  {
  TimeoutTable::iterator i=m_timeouts.find(param);
  if ( i != m_timeouts.end() ) m_timeouts.erase(i);
  return *this;
}

static inline bool _onTimeoutKill(const Rule* r) {
  if ( r && r->transition() && r->transition()->onTimeoutKill() ) return true;
  return false;
}

/// Handle timeout according to timer ID
void Slave::handleTimeout()  {
  int st = currentState();
  display(INFO,c_name(),"Slave TIMEOUT. State:%08X [%s] value:%08X [%s]",
	  st,metaStateName(),m_timerID.second,_metaStateName(m_timerID.second));
  if ( isInternal() )  {
    display(DEBUG,c_name(),"Received timeout from internal slave [Ignored].");
    return;
  }
  else if ( m_timerID.second == SLAVE_UNLOAD_TIMEOUT ) {
    handleUnloadTimeout();
  }
  else if ( m_timerID.second == SLAVE_TERMINATE_TIMEOUT ) {
    handleUnloadTimeout();
  }
  else if ( isLimbo() )  {  // Slave died in between
    transitionFailed();
    m_status = SLAVE_TIMEOUT;
  }
  else if ( m_timerID.second == SLAVE_TIMEOUT && SLAVE_EXECUTING == st )  {
    if ( _onTimeoutKill(m_rule) ) {
      forceKill();
      m_status = SLAVE_TIMEOUT;
    }
    else {
      transitionFailed();
      m_status = SLAVE_TIMEOUT;
    }
  }
  else if ( SLAVE_EXECUTING == st )  {
    handleUnloadTimeout();
  }
}

/// Handle timeout on unload transition according to timer ID
void Slave::handleUnloadTimeout()  {
  if ( m_timerID.second == SLAVE_UNLOAD_TIMEOUT )   {
    display(ERROR,c_name(),"Unload command unsuccessful. FAILURE - [%s]. State:%s",
	    "Insufficient Implementation",metaStateName());	  
  }
}
namespace {
  struct Publish {
    Slave* slave;
    Publish(Slave* s) : slave(s) {}
    ~Publish() { slave->publishDebugInfo(); }
  };
}

/// Handle state updates for a particular slave
void Slave::handleState(const string& msg)  {
  Publish pub(this);
  string        m = msg;
  int          st = currentState();
  bool   starting = st == SLAVE_LIMBO || st == SLAVE_STARTING;
  stopTimer();
  if ( isInternal() )  {      // We ignore messages from slaves, which are set internal
    display(INFO,c_name(),"Received message %s from internal slave [Ignored].",m.c_str());
    return;
  }
  else if ( m.empty() )    {  // No-link ?
    if ( !starting )  {
      display(NOLOG,c_name(),"Slave DEAD. Curr State:%s",metaStateName());
      iamDead();
    }
    return;
  }
  else if ( m == "UNKNOWN" ) {
#if 0
    display(ALWAYS,c_name(),"IGNORE Slave state:%s",m.c_str());
    return;
#endif
    m = "OFFLINE";
  }
  try  {
    const State* state = type()->state(m);
    const Transition* transition = (state && m_state) ? m_state->findTrans(state) : 0;
    // After fork slaves do not answer with OFFLINE of NOT_READY!
    starting &= (m == "OFFLINE");

    display(DEBUG,c_name(),"Received message %s starting:%s state:%s transition:%s",
	    m.c_str(), starting ? "YES" : "NO", State::c_name(state), Transition::c_name(transition));
    if ( state && state->isFailure() ) {
      lib_rtl_sleep(10); // Put in some code to set breakpoint for debugging
    }
    m_answered = true;

    if ( starting )   {
      iamHere();
    }
    else if ( state && state->isFailure() )   {
      m_state = state;
      transitionFailed();
    }
    else if ( transition )
      transitionDone(state);
    else if ( state )
      transitionSlave(state);
    else
      transitionFailed();
    return;
  }
  catch(const exception& e)  {
    display(ERROR,c_name(),"EXCEPTION on message %s starting:%s '%s'",
	    m.c_str(), starting ? "YES" : "NO", e.what());
  }
  catch(...)  {
    display(ERROR,c_name(),"UNKNOWN EXCEPTION on message %s starting:%s",
	    m.c_str(), starting ? "YES" : "NO");
  }
  transitionFailed();
}

/// Publish debug information
void Slave::publishDebugInfo()  {
}

/// Publish tag information
void Slave::publishTag(const string& /* tag */)   {
}

/// IOC handler
void Slave::handle(const Event& event)   {
  const State* state = (const State*)event.data;
  Publish pub(this);
  switch(event.type)  {
  case Slave::SLAVE_ALIVE:
    display(DEBUG,c_name(),"Received IAMHERE from internal slave. rule:%s",Rule::c_name(m_rule));
    iamHere();
    send(Slave::SLAVE_FINISHED,state);
    break;
  case Slave::SLAVE_LIMBO:
    iamDead();
    break;
  case Slave::SLAVE_FINISHED:
    display(DEBUG,c_name(),"Received message from internal slave. rule:%s State:%s",
	    Rule::c_name(m_rule),State::c_name(state));
    transitionDone(state);
    break;
  default:
    break;
  }
}

/// Inquire slave state. The reply may come later!
FSM::ErrCond Slave::inquireState() {
  throw runtime_error(name()+"> Slave::start -- invalid base class implementation called.");
}

/// Start slave process. Base class implementation will throw an exception
FSM::ErrCond Slave::start()  {
  throw runtime_error(name()+"> Slave::start -- invalid base class implementation called.");
}

/// Kill slave process. Base class implementation will throw an exception
FSM::ErrCond Slave::kill()   {
  throw runtime_error(name()+"> Slave::kill -- invalid base class implementation called.");
}

/// Kill slave process. Base class implementation will throw an exception
FSM::ErrCond Slave::forceKill()   {
  throw runtime_error(name()+"> Slave::forceKill -- invalid base class implementation called.");
}

/// Virtual method -- must be overloaded -- Send transition request to the slave
FSM::ErrCond Slave::sendRequest(const Transition* tr)  {
  throw runtime_error(name()+"> Slave::sendRequest -- invalid base class implementation called. Transition="+
		      string(Transition::c_name(tr)));
}
