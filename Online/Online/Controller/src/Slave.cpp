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

// C/C++ include files
#include <stdexcept>

using namespace FiniteStateMachine;
using namespace std;

/// Class Constructor
Slave::Slave(const Type *typ, const string& nam, Machine* machine, bool internal)
  : TypedObject(typ,nam), m_meta(SLAVE_LIMBO), m_machine(machine), m_state(0), 
    m_rule(0), m_internal(internal), m_alive(false)
{
  m_state = type()->initialState();
}

/// Standatrd destructor  
Slave::~Slave()    {
}

/// Access meta state as string
const char* Slave::metaStateName() const  {
  switch(m_meta)  {
#define MakeName(x) case Slave::x: return #x
    MakeName(SLAVE_LIMBO);
    MakeName(SLAVE_STARTING);
    MakeName(SLAVE_ALIVE);
    MakeName(SLAVE_FAILED);
    MakeName(SLAVE_KILLED);
    MakeName(SLAVE_DEAD);
    MakeName(SLAVE_EXECUTING);
    MakeName(SLAVE_TRANSITION);
    MakeName(SLAVE_FINISHED);
    MakeName(SLAVE_START_TIMEOUT);
    MakeName(SLAVE_TRANSITION_TIMEOUT);
#undef  MakeName
  }
  return "UNKNOWN";
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
  //display(NOLOG,"Slave %s> notify machine: %s State:%s",c_name(),m_machine->c_name(),metaStateName());
  IocSensor::instance().send((Interactor*)m_machine,meta_state,(void*)this); 
  return FSM::SUCCESS;
}

/// Callback on alive signs of slave process
FSM::ErrCond Slave::iamHere()  {
  m_rule  = 0;
  m_alive = true;
  m_meta  = SLAVE_ALIVE;
  m_state = type()->initialState();
  return m_machine->checkAliveSlaves();
}

/// Callback on sudden death of slave process
FSM::ErrCond Slave::iamDead()  {
  m_rule  = 0;
  m_alive = false;
  m_meta  = SLAVE_LIMBO;
  m_state = type()->initialState();
  ErrCond ret = m_machine->checkSlaves();
  if ( ret == FSM::TRANNOTFOUND )  {
    return notifyMachine(SLAVE_TRANSITION);
  }
  return ret;
}

/// Callback, when transition was executed successfully
FSM::ErrCond Slave::transitionDone(const State* state)  {
  m_rule  = 0;
  m_state = state;
  m_meta  = SLAVE_ALIVE;
  ErrCond ret = m_machine->checkSlaves();
  if ( ret == FSM::TRANNOTFOUND )  {
    return notifyMachine(SLAVE_TRANSITION);
  }
  if ( ret == FSM::WAIT_ACTION )
    return FSM::SUCCESS;
  // Else: FSM::SUCCESS or FSM::FAIL
  return ret;
}

/// Callback, when the slave invoked a transition himself
FSM::ErrCond Slave::transitionSlave(const State* state)  {
  if ( state )  {
    m_rule  = 0;
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
  const Transition* tr = m_state->findTrans(rule->targetState());
  m_rule = 0;
  if ( tr )  {
    m_rule = rule;
    m_meta = SLAVE_EXECUTING;
    return isInternal() ? send(SLAVE_FINISHED,tr->to()) : sendRequest(tr);
  }
  return FSM::TRANNOTFOUND;
}

/// Start slave process
FSM::ErrCond Slave::startSlave()  {
  if ( isInternal() )  {
    send(SLAVE_ALIVE,0);
    return FSM::WAIT_ACTION;
  }
  return start();
}

/// Start slave process
FSM::ErrCond Slave::killSlave()  {
  if ( isInternal() )  {
    send(SLAVE_LIMBO,0);
    return FSM::WAIT_ACTION;
  }
  return kill();
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
  throw runtime_error(name()+"> Slave::sendRequest -- invalid base class implementation called.");
}

/// IOC and network handler
void Slave::handle(const Event& event)  {
  switch(event.eventtype) {
  case NetEvent:
    break;
  case IocEvent:
    handleIoc(event);
    break;
  default:
    display(ERROR,"Slave> %s: Unforeseen eventtype:%d",c_name(),event.eventtype);
    break;
  }
}

/// IOC handler
void Slave::handleIoc(const Event& event)   {
  switch(event.type)  {
  case SLAVE_LIMBO:
    iamDead();
    break;
  case SLAVE_ALIVE:
    iamHere();
    break;
  case SLAVE_TRANSITION:
    transitionSlave(event.iocPtr<State>());
    break;
  case SLAVE_FINISHED:
    transitionDone(event.iocPtr<State>());
    break;
  case SLAVE_FAILED:
    transitionFailed();
    break;
  case SLAVE_STARTING:
    break;
  // IOC handling of external commands
  case SLAVE_TRANSITION_TIMEOUT:
    display(ERROR,"%s > Transition timeout:%d",c_name(),event.type);
    transitionFailed();
    break;
  default:
    display(ERROR,"%s > Unforeseen ioc:%d",c_name(),event.type);
    break;
  }
}
