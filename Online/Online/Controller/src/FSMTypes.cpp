/*============================================================
**
**      FSM process management on a HLT worker node
**
**  AUTHORS:
**
**      M.Frank
**
**==========================================================*/

// Framework include files
#include "FiniteStateMachine/Type.h"
#include "FiniteStateMachine/Rule.h"
#include "FiniteStateMachine/Transition.h"
#include "FiniteStateMachine/FSMTypes.h"
#include <stdexcept>

using namespace FiniteStateMachine;
using namespace FiniteStateMachine::DAQ;
using namespace std;

/// Create DAQ machine type
static Type* defineDAQType()    {
  typedef Transition Tr;
  Type *daq = new Type("DAQ");
  const State* offline   = daq->addState(ST_NAME_OFFLINE);
  const State* not_ready = daq->addState(ST_NAME_NOT_READY);
  const State* ready     = daq->addState(ST_NAME_READY);
  const State* running   = daq->addState(ST_NAME_RUNNING);
  const State* error     = daq->addState(ST_NAME_ERROR);

  Tr*  load      = daq->addTransition("load",      offline,     not_ready, CHECK|CREATE);
  Tr*  configure = daq->addTransition("configure", not_ready,   ready);
  Tr*  start     = daq->addTransition("start",     ready,       running);
  Tr*  stop0     = daq->addTransition("stop",      running,     ready);
  Tr*  stop1     = daq->addTransition("stop",      ready,       ready);

  Tr*  unload0   = daq->addTransition("unload",    running,     offline, KILL);
  Tr*  unload1   = daq->addTransition("unload",    ready,       offline, KILL);
  Tr*  unload2   = daq->addTransition("unload",    not_ready,   offline, KILL);
  Tr*  unload3   = daq->addTransition("unload",    offline,     offline, KILL);

  Tr*  reset0    = daq->addTransition("reset",     not_ready,   not_ready);
  Tr*  reset1    = daq->addTransition("reset",     ready,       not_ready);
  Tr*  reset2    = daq->addTransition("reset",     error,       not_ready);

  Tr*  recover0  = daq->addTransition("recover",   error,       offline);
  Tr*  recover1  = daq->addTransition("recover",   running,     offline);
  Tr*  recover2  = daq->addTransition("recover",   ready,       offline);
  Tr*  recover3  = daq->addTransition("recover",   not_ready,   offline);
  Tr*  recover4  = daq->addTransition("recover",   offline,     offline);

  Tr*  daq_err0  = daq->addTransition(not_ready,   error,       NO_CHECKS);
  Tr*  daq_err1  = daq->addTransition(ready,       error,       NO_CHECKS);
  Tr*  daq_err2  = daq->addTransition(running,     error,       NO_CHECKS);

  Tr*  RESET0    = daq->addTransition("RESET",     error,       offline, NO_CHECKS);
  Tr*  RESET1    = daq->addTransition("RESET",     running,     offline, NO_CHECKS);
  Tr*  RESET2    = daq->addTransition("RESET",     ready,       offline, NO_CHECKS);
  Tr*  RESET3    = daq->addTransition("RESET",     not_ready,   offline, NO_CHECKS);
  Tr*  RESET4    = daq->addTransition("RESET",     offline,     offline, NO_CHECKS);

  daq->setInitialState(offline);

  daq->addRule      (daq_err0,   daq, ST_NAME_ANY,       ST_NAME_ERROR);
  daq->addRule      (daq_err1,   daq, ST_NAME_ANY,       ST_NAME_ERROR);
  daq->addRule      (daq_err2,   daq, ST_NAME_ANY,       ST_NAME_ERROR);

  //daq->addRule      (recover0,   daq, ST_NAME_ERROR,     ST_NAME_OFFLINE);
  // Otherwise: FULL KILL on recover
  daq->addRule      (recover0,   daq, ST_NAME_ERROR,     ST_NAME_OFFLINE);
  daq->addRule      (recover1,   daq, ST_NAME_ERROR,     ST_NAME_OFFLINE);
  daq->addRule      (recover2,   daq, ST_NAME_ERROR,     ST_NAME_OFFLINE);
  daq->addRule      (recover3,   daq, ST_NAME_ERROR,     ST_NAME_OFFLINE);
  daq->addRule      (recover4,   daq, ST_NAME_ERROR,     ST_NAME_OFFLINE);

  daq->addRule      (RESET0,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  daq->addRule      (RESET1,     daq, ST_NAME_RUNNING,   ST_NAME_OFFLINE);
  daq->addRule      (RESET2,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  daq->addRule      (RESET3,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  daq->addRule      (RESET4,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);

  daq->addRule      (reset0,     daq, ST_NAME_NOT_READY, ST_NAME_NOT_READY);
  daq->addRule      (reset0,     daq, ST_NAME_READY,     ST_NAME_NOT_READY);
  daq->addRule      (reset0,     daq, ST_NAME_ERROR,     ST_NAME_NOT_READY);
  daq->addRule      (reset1,     daq, ST_NAME_NOT_READY, ST_NAME_NOT_READY);
  daq->addRule      (reset1,     daq, ST_NAME_READY,     ST_NAME_NOT_READY);
  daq->addRule      (reset1,     daq, ST_NAME_ERROR,     ST_NAME_NOT_READY);

  daq->addRule      (reset2,     daq, ST_NAME_NOT_READY, ST_NAME_NOT_READY);
  daq->addRule      (reset2,     daq, ST_NAME_READY,     ST_NAME_NOT_READY);
  daq->addRule      (reset2,     daq, ST_NAME_RUNNING,   ST_NAME_READY);
  daq->addRule      (reset2,     daq, ST_NAME_ERROR,     ST_NAME_NOT_READY);

  daq->addPredicate (load,       daq, ST_NAME_OFFLINE,   ST_NAME_NOT_READY, ST_NAME_READY, ST_NAME_RUNNING);
  daq->addRule      (load,       daq, ST_NAME_OFFLINE,   ST_NAME_NOT_READY);

  daq->addPredicate (configure,  daq, ST_NAME_NOT_READY, ST_NAME_READY, ST_NAME_RUNNING);
  daq->addRule      (configure,  daq, ST_NAME_NOT_READY, ST_NAME_READY);//->excludeStates(ST_NAME_RUNNING);

  daq->addPredicate (start,      daq, ST_NAME_READY,     ST_NAME_RUNNING);
  daq->addRule      (start,      daq, ST_NAME_READY,     ST_NAME_RUNNING);

  daq->addRule      (stop0,      daq, ST_NAME_RUNNING,   ST_NAME_READY);
  daq->addRule      (stop1,      daq, ST_NAME_RUNNING,   ST_NAME_READY);

  daq->addRule      (unload0,    daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  daq->addRule      (unload1,    daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  daq->addRule      (unload2,    daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  daq->addRule      (unload3,    daq, ST_NAME_ANY,       ST_NAME_OFFLINE);


  return daq;
}

/// Create DAQ machine type
static Type* defineDAQSteerType() {
  typedef Transition Tr;
  Type* daq = fsm_type("DAQ");
  Type *typ = new Type("DAQSteer");
  const State* unknown   = typ->addState(ST_NAME_UNKNOWN);
  const State* offline   = typ->addState(ST_NAME_OFFLINE);
  const State* not_ready = typ->addState(ST_NAME_NOT_READY);
  const State* ready     = typ->addState(ST_NAME_READY);
  const State* running   = typ->addState(ST_NAME_RUNNING);
  const State* error     = typ->addState(ST_NAME_ERROR);

  Tr*  create    = typ->addTransition("create",    unknown,   offline,   CHECK|CREATE);
  Tr*  load      = typ->addTransition("load",      offline,   not_ready, CHECK|CREATE);
  Tr*  configure = typ->addTransition("configure", not_ready, ready);
  Tr*  start     = typ->addTransition("start",     ready,     running);
  Tr*  stop      = typ->addTransition("stop",      running,   ready);
  Tr*  unload1   = typ->addTransition("unload",    not_ready, offline, KILL);
  Tr*  unload2   = typ->addTransition("unload",    offline,   offline, KILL);
  Tr*  destroy   = typ->addTransition("destroy",   offline,   unknown, KILL);

  Tr*  reset1    = typ->addTransition("reset",     ready,     not_ready);
  Tr*  recover   = typ->addTransition("recover",   error,     offline);

  Tr*  daq_err0  = typ->addTransition(not_ready,   error,     NO_CHECKS);
  Tr*  daq_err1  = typ->addTransition(ready,       error,     NO_CHECKS);
  Tr*  daq_err2  = typ->addTransition(running,     error,     NO_CHECKS);

  Tr*  RESET0    = typ->addTransition("RESET",     error,     offline,   NO_CHECKS);
  Tr*  RESET1    = typ->addTransition("RESET",     running,   offline,   NO_CHECKS);
  Tr*  RESET2    = typ->addTransition("RESET",     ready,     offline,   NO_CHECKS);
  Tr*  RESET3    = typ->addTransition("RESET",     not_ready, offline,   NO_CHECKS);
  Tr*  RESET4    = typ->addTransition("RESET",     offline,   offline,   NO_CHECKS);

  typ->setInitialState(unknown);
  typ->addRule      (daq_err0,   daq, ST_NAME_ANY,       ST_NAME_ERROR);
  typ->addRule      (daq_err1,   daq, ST_NAME_ANY,       ST_NAME_ERROR);
  typ->addRule      (daq_err2,   daq, ST_NAME_ANY,       ST_NAME_ERROR);
  typ->addRule      (recover,    daq, ST_NAME_ERROR,     ST_NAME_OFFLINE);

  typ->addRule      (reset1,     daq, ST_NAME_READY,     ST_NAME_NOT_READY);
  typ->addRule      (load,       daq, ST_NAME_OFFLINE,   ST_NAME_NOT_READY);
  typ->addPredicate (configure,  daq, ST_NAME_NOT_READY, ST_NAME_READY, ST_NAME_RUNNING);
  typ->addRule      (configure,  daq, ST_NAME_NOT_READY, ST_NAME_READY);

  typ->addPredicate (start,      daq, ST_NAME_READY,     ST_NAME_RUNNING);
  typ->addRule      (start,      daq, ST_NAME_READY,     ST_NAME_RUNNING);

  typ->addRule      (stop,       daq, ST_NAME_RUNNING,   ST_NAME_READY);
  daq->addRule      (unload1,    daq, ST_NAME_NOT_READY, ST_NAME_OFFLINE);
  daq->addRule      (unload2,    daq, ST_NAME_NOT_READY, ST_NAME_OFFLINE);
  daq->addRule      (unload2,    daq, ST_NAME_OFFLINE,   ST_NAME_OFFLINE);
  typ->addRule      (destroy,    daq, ST_NAME_ANY,       ST_NAME_OFFLINE);

  typ->addRule      (RESET0,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  typ->addRule      (RESET1,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  typ->addRule      (RESET2,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  typ->addRule      (RESET3,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  typ->addRule      (RESET4,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  return typ;
}

/// Inventory of known FSM types
Type* FiniteStateMachine::fsm_type(const std::string& typ)  {
  typedef map<string,Type*> _M;
  static _M m;
  _M::iterator i = m.find(typ);
  if ( i == m.end() )  {
    if ( typ == "DAQ" ) return m[typ]=defineDAQType();
    else if ( typ == "DAQSteer" ) return m[typ]=defineDAQSteerType();
    throw runtime_error("Request for undefined FSM type:"+typ);
  }
  return (*i).second;
}
