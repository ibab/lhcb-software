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
#include "FiniteStateMachine/State.h"
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
  const State* paused    = daq->addState(ST_NAME_PAUSED);

  daq->setInitialState(offline);

  not_ready->when(anyChildInState(error),      moveTo(error));
  not_ready->when(anyChildInState(offline),    moveTo(offline));
  not_ready->when(allChildrenInState(ready,running,paused), moveTo(ready));

  ready->when    (  anyChildInState(error),      moveTo(error));
  ready->when    (  anyChildInState(offline),    moveTo(offline));
  ready->when    (  anyChildInState(not_ready),  moveTo(not_ready));
  ready->when    (  allChildrenInState(running), moveTo(running));

  running->when  (  anyChildInState(error),      moveTo(error));
  running->when  (  anyChildInState(offline),    moveTo(offline));
  running->when  (  anyChildInState(not_ready),  moveTo(not_ready));
  running->when  (  anyChildInState(paused),     moveTo(paused));
  running->when  (  allChildrenInState(ready),   moveTo(ready));

  paused->when   (  anyChildInState(error),      moveTo(error));
  paused->when   (  anyChildInState(offline),    moveTo(offline));
  paused->when   (  anyChildInState(not_ready),  moveTo(not_ready));
  paused->when   (  allChildrenInState(running), moveTo(running));
  paused->when   (  allChildrenInState(ready),   moveTo(ready));

  error->when    (  allChildrenInState(offline), moveTo(offline));

  
  Tr*  unload0   = daq->addTransition("unload",    running,     offline, NO_CHECKS);
  //Tr*  unload1   = daq->addTransition("unload",    ready,       offline, NO_CHECKS);
  Tr*  unload2   = daq->addTransition("unload",    not_ready,   offline, KILL);
  Tr*  unload3   = daq->addTransition("unload",    offline,     offline, NO_CHECKS);

  Tr*  reset0    = daq->addTransition("reset",     not_ready,   not_ready);
  Tr*  reset1    = daq->addTransition("reset",     ready,       not_ready, NO_CHECKS);
  Tr*  reset2    = daq->addTransition("reset",     error,       not_ready, NO_CHECKS);

  Tr*  recover0  = daq->addTransition("recover",   error,       offline,   NO_CHECKS);
  Tr*  recover1  = daq->addTransition("recover",   running,     offline,   NO_CHECKS);
  Tr*  recover2  = daq->addTransition("recover",   ready,       offline,   NO_CHECKS);
  Tr*  recover3  = daq->addTransition("recover",   not_ready,   offline,   NO_CHECKS);
  Tr*  recover4  = daq->addTransition("recover",   offline,     offline);

  Tr*  daq_err0  = daq->addTransition("daq_err",   not_ready,   error,     NO_CHECKS);
  Tr*  daq_err1  = daq->addTransition("daq_err",   ready,       error,     NO_CHECKS);
  Tr*  daq_err2  = daq->addTransition("daq_err",   running,     error,     NO_CHECKS);
  Tr*  daq_err3  = daq->addTransition("daq_err",   paused,      error,     NO_CHECKS);

  Tr*  RESET0    = daq->addTransition("RESET",     error,       offline,   NO_CHECKS);
  Tr*  RESET1    = daq->addTransition("RESET",     running,     offline,   NO_CHECKS);
  Tr*  RESET2    = daq->addTransition("RESET",     ready,       offline,   NO_CHECKS);
  Tr*  RESET3    = daq->addTransition("RESET",     not_ready,   offline,   NO_CHECKS);
  Tr*  RESET4    = daq->addTransition("RESET",     offline,     offline,   NO_CHECKS);
  Tr*  RESET5    = daq->addTransition("RESET",     paused,      offline,   NO_CHECKS);

  daq->addRule      (daq_err0,   daq, ST_NAME_ANY,       ST_NAME_ERROR);
  daq->addRule      (daq_err1,   daq, ST_NAME_ANY,       ST_NAME_ERROR);
  daq->addRule      (daq_err2,   daq, ST_NAME_ANY,       ST_NAME_ERROR);
  daq->addRule      (daq_err3,   daq, ST_NAME_ANY,       ST_NAME_ERROR);

  //daq->addRule      (recover0,   daq, ST_NAME_ERROR,     ST_NAME_OFFLINE);
  // Otherwise: FULL KILL on recover
  daq->addRule      (recover0,   daq, ST_NAME_ERROR,     ST_NAME_OFFLINE);
  daq->addRule      (recover1,   daq, ST_NAME_ERROR,     ST_NAME_OFFLINE);
  daq->addRule      (recover2,   daq, ST_NAME_ERROR,     ST_NAME_OFFLINE);
  daq->addRule      (recover3,   daq, ST_NAME_ERROR,     ST_NAME_OFFLINE);
  daq->addRule      (recover4,   daq, ST_NAME_ERROR,     ST_NAME_OFFLINE);

  daq->addRule      (RESET0,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  daq->addRule      (RESET1,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  daq->addRule      (RESET2,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  daq->addRule      (RESET3,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  daq->addRule      (RESET4,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  daq->addRule      (RESET5,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);

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

  /// Offline -> Not Ready
  Tr*  load      = daq->addTransition("load",      offline,     not_ready, CHECK|CREATE);
  daq->addPredicate (load,       daq, ST_NAME_OFFLINE,   ST_NAME_NOT_READY, ST_NAME_READY, ST_NAME_RUNNING);
  daq->addRule      (load,       daq, ST_NAME_OFFLINE,   ST_NAME_NOT_READY);

  /// Not Ready -> Ready
  Tr*  configure = daq->addTransition("configure", not_ready,   ready);
  daq->addPredicate (configure,  daq, ST_NAME_NOT_READY, ST_NAME_READY, ST_NAME_RUNNING);
  daq->addRule      (configure,  daq, ST_NAME_NOT_READY, ST_NAME_READY);//->excludeStates(ST_NAME_RUNNING);

  /// Ready -> Running
  Tr*  start0    = daq->addTransition("start",     ready,       running);
  daq->addPredicate (start0,     daq, ST_NAME_READY,     ST_NAME_PAUSED,  ST_NAME_RUNNING);
  daq->addRule      (start0,     daq, ST_NAME_READY,     ST_NAME_RUNNING);
  daq->addRule      (start0,     daq, ST_NAME_PAUSED,    ST_NAME_RUNNING);

  /// Running -> Paused
  Tr*  pause     = daq->addTransition("pause",     running,     paused);
  daq->addRule      (pause,      daq, ST_NAME_RUNNING,   ST_NAME_PAUSED,  Rule::MASTER2SLAVE);

  /// Paused -> Running
  Tr*  start1    = daq->addTransition("start",     paused,      running);
  daq->addPredicate (start1,     daq, ST_NAME_READY,     ST_NAME_PAUSED,     ST_NAME_RUNNING);
  daq->addRule      (start1,     daq, ST_NAME_READY,     ST_NAME_RUNNING);
  daq->addRule      (start1,     daq, ST_NAME_PAUSED,    ST_NAME_RUNNING);

  /// Paused -> Ready
  Tr*  stop1     = daq->addTransition("stop",      paused,      ready);
  daq->addRule      (stop1,      daq, ST_NAME_PAUSED,    ST_NAME_READY);
  daq->addRule      (stop1,      daq, ST_NAME_RUNNING,   ST_NAME_READY);

  /// Running -> Ready
  Tr*  stop0     = daq->addTransition("stop",      running,     ready);
  daq->addRule      (stop0,      daq, ST_NAME_RUNNING,   ST_NAME_READY);

  /// Ready -> Ready
  Tr*  stop2     = daq->addTransition("stop",      ready,       ready);
  daq->addRule      (stop2,      daq, ST_NAME_RUNNING,   ST_NAME_READY);

  daq->addRule      (unload0,    daq, ST_NAME_RUNNING,   ST_NAME_OFFLINE, Rule::MASTER2SLAVE);

  daq->addRule      (unload2,    daq, ST_NAME_RUNNING,   ST_NAME_OFFLINE, Rule::MASTER2SLAVE);
  daq->addRule      (unload2,    daq, ST_NAME_READY,     ST_NAME_OFFLINE, Rule::MASTER2SLAVE);
  daq->addRule      (unload2,    daq, ST_NAME_NOT_READY, ST_NAME_OFFLINE);

  daq->addRule      (unload3,    daq, ST_NAME_RUNNING,   ST_NAME_OFFLINE, Rule::MASTER2SLAVE);
  daq->addRule      (unload3,    daq, ST_NAME_READY,     ST_NAME_OFFLINE, Rule::MASTER2SLAVE);
  daq->addRule      (unload3,    daq, ST_NAME_NOT_READY, ST_NAME_OFFLINE);

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
  const State* paused    = typ->addState(ST_NAME_PAUSED);
  typ->setInitialState(unknown);

  const State* daq_offline   = daq->state(ST_NAME_OFFLINE);
  const State* daq_not_ready = daq->state(ST_NAME_NOT_READY);
  const State* daq_ready     = daq->state(ST_NAME_READY);
  const State* daq_running   = daq->state(ST_NAME_RUNNING);
  const State* daq_error     = daq->state(ST_NAME_ERROR);
  const State* daq_paused    = daq->state(ST_NAME_PAUSED);
 
  not_ready->when(anyChildInState(error,daq_error),          moveTo(error));
  not_ready->when(anyChildInState(offline,daq_offline),      moveTo(offline));
  not_ready->when(allChildrenInState(daq_ready,daq_running,daq_paused), moveTo(ready));

  ready->when    (  anyChildInState(error,daq_error),        moveTo(error));
  ready->when    (  anyChildInState(offline,daq_offline),    moveTo(offline));
  ready->when    (  allChildrenInState(daq_running), moveTo(running));
  ready->when    (  allChildrenInState(daq_not_ready),moveTo(not_ready));

  running->when  (  anyChildInState(error,daq_error),        moveTo(error));
  running->when  (  anyChildInState(offline,daq_offline),    moveTo(offline));
  running->when  (  anyChildInState(paused,daq_paused),      moveTo(paused));
  running->when  (  allChildrenInState(ready,daq_ready),     moveTo(ready));

  paused->when   (  anyChildInState(error,daq_error),        moveTo(error));
  paused->when   (  anyChildInState(offline,daq_offline),    moveTo(offline));
  paused->when   (  allChildrenInState(daq_running), moveTo(running));
  paused->when   (  allChildrenInState(daq_ready),   moveTo(ready));

  error->when    (  allChildrenInState(offline,daq_offline), moveTo(offline));

  /* Tr* crea = */ typ->addTransition("create",    unknown,     offline,   CHECK|CREATE);
  Tr*  load      = typ->addTransition("load",      offline,     not_ready, CHECK|CREATE);
  Tr*  configure = typ->addTransition("configure", not_ready,   ready);
  Tr*  start0    = typ->addTransition("start",     ready,       running);
  Tr*  start1    = typ->addTransition("start",     paused,      running);
  Tr*  stop0     = typ->addTransition("stop",      running,     ready);
  Tr*  stop1     = typ->addTransition("stop",      paused,      ready);
  Tr*  pause     = typ->addTransition("pause",     running,     paused);

  Tr*  unload1   = typ->addTransition("unload",    not_ready,   offline);
  Tr*  unload2   = typ->addTransition("unload",    offline,     offline);
  Tr*  destroy   = typ->addTransition("destroy",   offline,     unknown,   KILL);

  Tr*  reset1    = typ->addTransition("reset",     ready,       not_ready);
  Tr*  recover   = typ->addTransition("recover",   error,       offline);

  Tr*  daq_err0  = typ->addTransition("error",     not_ready,   error,     NO_CHECKS);
  Tr*  daq_err1  = typ->addTransition("error",     ready,       error,     NO_CHECKS);
  Tr*  daq_err2  = typ->addTransition("error",     paused,      error,     NO_CHECKS);
  Tr*  daq_err3  = typ->addTransition("error",     running,     error,     NO_CHECKS);

  Tr*  RESET0    = typ->addTransition("RESET",     error,       offline,   NO_CHECKS);
  Tr*  RESET1    = typ->addTransition("RESET",     running,     offline,   NO_CHECKS);
  Tr*  RESET2    = typ->addTransition("RESET",     ready,       offline,   NO_CHECKS);
  Tr*  RESET3    = typ->addTransition("RESET",     not_ready,   offline,   NO_CHECKS);
  Tr*  RESET4    = typ->addTransition("RESET",     offline,     offline,   NO_CHECKS);
  Tr*  RESET5    = typ->addTransition("RESET",     paused,      offline,   NO_CHECKS);

  typ->addRule      (daq_err0,   daq, ST_NAME_ANY,       ST_NAME_ERROR);
  typ->addRule      (daq_err1,   daq, ST_NAME_ANY,       ST_NAME_ERROR);
  typ->addRule      (daq_err2,   daq, ST_NAME_ANY,       ST_NAME_ERROR);
  typ->addRule      (daq_err3,   daq, ST_NAME_ANY,       ST_NAME_ERROR);
  typ->addRule      (recover,    daq, ST_NAME_ERROR,     ST_NAME_OFFLINE);

  typ->addRule      (reset1,     daq, ST_NAME_READY,     ST_NAME_NOT_READY);
  typ->addRule      (load,       daq, ST_NAME_OFFLINE,   ST_NAME_NOT_READY);
  typ->addPredicate (configure,  daq, ST_NAME_NOT_READY, ST_NAME_READY, ST_NAME_RUNNING);
  typ->addRule      (configure,  daq, ST_NAME_NOT_READY, ST_NAME_READY);

  typ->addPredicate (start0,     daq, ST_NAME_READY,     ST_NAME_PAUSED, ST_NAME_RUNNING);
  typ->addRule      (start0,     daq, ST_NAME_READY,     ST_NAME_RUNNING);
  typ->addRule      (start0,     daq, ST_NAME_PAUSED,    ST_NAME_RUNNING);

  typ->addPredicate (start1,     daq, ST_NAME_READY,     ST_NAME_PAUSED, ST_NAME_RUNNING);
  typ->addRule      (start1,     daq, ST_NAME_READY,     ST_NAME_RUNNING);
  typ->addRule      (start1,     daq, ST_NAME_PAUSED,    ST_NAME_RUNNING);

  typ->addRule      (pause,      daq, ST_NAME_RUNNING,   ST_NAME_PAUSED, Rule::MASTER2SLAVE);
  typ->addRule      (stop0,      daq, ST_NAME_RUNNING,   ST_NAME_READY);
  typ->addRule      (stop1,      daq, ST_NAME_PAUSED,    ST_NAME_READY);

  typ->addRule      (unload1,    daq, ST_NAME_NOT_READY, ST_NAME_OFFLINE);
  typ->addRule      (unload2,    daq, ST_NAME_NOT_READY, ST_NAME_OFFLINE);
  typ->addRule      (unload2,    daq, ST_NAME_OFFLINE,   ST_NAME_OFFLINE);
  typ->addRule      (destroy,    daq, ST_NAME_ANY,       ST_NAME_OFFLINE);

  typ->addRule      (RESET0,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  typ->addRule      (RESET1,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  typ->addRule      (RESET2,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  typ->addRule      (RESET3,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  typ->addRule      (RESET4,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
  typ->addRule      (RESET5,     daq, ST_NAME_ANY,       ST_NAME_OFFLINE);
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
