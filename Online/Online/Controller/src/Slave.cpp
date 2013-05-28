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
    MakeName(SLAVETIMEOUT);
    MakeName(SLAVE_UNLOAD_TIMEOUT);
    MakeName(SLAVE_TERMINATE_TIMEOUT);
    MakeName(SLAVE_KILL_TIMEOUT);
    MakeName(SLAVE_START_TIMEOUT);
#undef  MakeName
  }
  return "UNKNOWN";
}

/// Class Constructor
Slave::Slave(const Type *typ, const string& nam, Machine* machine, bool internal)
  : TypedObject(typ,nam), m_timerID(0,0), m_tmo(5), 
    m_meta(SLAVE_LIMBO), m_machine(machine), m_state(0), 
    m_rule(0), m_internal(internal), m_alive(false)
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

/// Retrieve reference to current State structure name
const char* Slave::c_state ()  const
{
  return m_state ? m_state->c_name() : "----";  
}

/// Retrieve reference to managing machine structure name
const char* Slave::c_machine ()  const
{
  return m_machine ? m_machine->c_name() : "----";  
}

/// Send IOC interrupt to slave
FSM::ErrCond Slave::send(int code, const State* state)   const {
  IocSensor::instance().send((Interactor*)this,code,(void*)state); 
  return FSM::SUCCESS;
}

FSM::ErrCond Slave::notifyMachine(int meta_state)  {
  display(DEBUG,"Slave %s> notify machine: %s State:%s",c_name(),m_machine->c_name(),metaStateName());
  IocSensor::instance().send((Interactor*)m_machine,meta_state,(void*)this); 
  return FSM::SUCCESS;
}

/// Callback on alive signs of slave process
FSM::ErrCond Slave::iamHere()  {
  m_rule  = 0;
  m_alive = true;
  m_meta  = SLAVE_ALIVE;
  m_state = type()->initialState();
  if ( !m_machine->currTrans() )
    return notifyMachine(SLAVE_TRANSITION);
  return m_machine->checkAliveSlaves();
}

/// Callback on sudden death of slave process
FSM::ErrCond Slave::iamDead()  {
  m_rule  = 0;
  m_alive = false;
  m_meta  = SLAVE_LIMBO;
  m_state = type()->initialState();
  if ( !m_machine->currTrans() )
    return notifyMachine(SLAVE_TRANSITION);
  return m_machine->checkSlaves();
}

/// Callback, when transition was executed successfully
FSM::ErrCond Slave::transitionDone(const State* state)  {
  m_rule  = 0;
  m_alive = true;
  m_state = state;
  m_meta  = SLAVE_ALIVE;
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
    // State change by slave. Handle the request.
    return notifyMachine(SLAVE_TRANSITION);
  }
  return FSM::FAIL;
}

/// Callback, when transition failed (called on receipt of answer message)
FSM::ErrCond Slave::transitionFailed()  {
  m_meta  = SLAVE_FAILED;
  m_state = m_rule ? m_rule->currState() : 0;
  m_rule  = 0;
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
    display(INFO,"%s> Send request \"%s\" to target process:%s",
	    m_machine->c_name(), tr->c_name(), c_name());
    if ( isInternal() ) {
      return send(SLAVE_FINISHED,tr->to());
    }
    ErrCond ret = sendRequest(tr);
    startTimer(SLAVETIMEOUT,tr);
    return ret;
  }
  return FSM::TRANNOTFOUND;
}

/// Start slave process
FSM::ErrCond Slave::startSlave()  {
  inquireState();
  if ( isInternal() )  {
    send(SLAVE_ALIVE,0);
    return FSM::WAIT_ACTION;
  }
  FSM::ErrCond ret = start();
  startTimer(SLAVE_START_TIMEOUT);
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

/// Handle timeout according to timer ID
void Slave::handleTimeout()  {
  int st = currentState();
  display(ALWAYS,"%s> Slave %s Received new message TIMEOUT. State:%08X [%s] timer value:%08X [%s]",
	  RTL::processName().c_str(),c_name(),st,metaStateName(),
	  m_timerID.second,_metaStateName(m_timerID.second));
  if ( isLimbo() )  {
    transitionFailed();
  }
  else if ( SLAVE_EXECUTING == st )  {
    m_timerID.second == SLAVETIMEOUT ? (void)transitionFailed() : handleUnloadTimeout();
  }
}

/// Handle timeout on unload transition according to timer ID
void Slave::handleUnloadTimeout()  {
  if ( m_timerID.second == SLAVE_UNLOAD_TIMEOUT )   {
    display(ERROR,"%s> unload command unsuccessful. FAILURE - [%s]. State:%s",
	    c_name(),"Insufficient Implementation",metaStateName());	  
  }
}

/// Handle state updates for a particular slave
void Slave::handleState(const string& msg)  {
  string        m = msg;
  int          st = currentState();
  bool   starting = st == SLAVE_LIMBO || st == SLAVE_STARTING;
  stopTimer();
  if ( m.empty() )    {  // No-link ?
    if ( !starting )  {
      display(NOLOG,"%s::%s> Slave DEAD. Curr State:%s",
	      RTL::processName().c_str(),c_name(),metaStateName());
      iamDead();
    }
    return;
  }
  else if ( m == "UNKNOWN" ) {
#if 0
    display(ALWAYS,"%s::%s> IGNORE Slave state:%s",
	    RTL::processName().c_str(),c_name(),m.c_str());
    return;
#endif
    m = "OFFLINE";
  }
  const State* state = type()->state(m);
  const Transition* transition = (state && m_state) ? m_state->findTrans(state) : 0;
  // After fork slaves do not answer with OFFLINE of NOT_READY!
  starting &= (m == "OFFLINE");// || m == "NOT_READY");

  display(INFO,"%s> Received new message from %p %s %s  starting:%s state:%p transition:%p",
	  m_machine->c_name(), this, c_name(), m.c_str(),
	  starting ? "YES" : "NO", state,transition);

  if ( starting )
    iamHere();
  else if ( transition )
    transitionDone(state);
  else if ( state )
    transitionSlave(state);
  else
    transitionFailed();
}


/// IOC handler
void Slave::handle(const Event& event)   {
  const State* state = (const State*)event.data;
  switch(event.type)  {
  case Slave::SLAVE_ALIVE:
    iamHere();
    break;
  case Slave::SLAVE_LIMBO:
    iamDead();
    break;
  case Slave::SLAVE_FINISHED:
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

/// Virtual method -- must be overloaded -- Send transition request to the slave
FSM::ErrCond Slave::sendRequest(const Transition* tr)  {
  throw runtime_error(name()+"> Slave::sendRequest -- invalid base class implementation called. Transition="+
		      string(tr ? tr->name() : "----"));
}
