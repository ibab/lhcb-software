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

  not_ready->when(anyChildInState(error),          moveTo(error));
  not_ready->when(anyChildInState(offline),        moveTo(offline));
  not_ready->when(allChildrenInState(ready,running,paused), moveTo(ready));

  ready->when    (  anyChildInState(error),        moveTo(error));
  ready->when    (  anyChildInState(offline),      moveTo(offline));
  ready->when    (  anyChildInState(not_ready),    moveTo(not_ready));
  ready->when    (  allChildrenInState(running),   moveTo(running));

  running->when  (  anyChildInState(error),        moveTo(error));
  running->when  (  anyChildInState(offline),      moveTo(offline));
  running->when  (  anyChildInState(not_ready),    moveTo(not_ready));
  running->when  (  anyChildInState(paused),       moveTo(paused));
  running->when  (  allChildrenInState(ready),     moveTo(ready));

  paused->when   (  anyChildInState(error),        moveTo(error));
  paused->when   (  anyChildInState(offline),      moveTo(offline));
  paused->when   (  anyChildInState(not_ready),    moveTo(not_ready));
  paused->when   (  allChildrenInState(running),   moveTo(running));
  paused->when   (  allChildrenInState(ready),     moveTo(ready));

  offline->when  (  anyChildInState(error),        moveTo(error));
  offline->when  (  allChildrenInState(running),   moveTo(running));
  offline->when  (  allChildrenInState(ready,paused,running),     moveTo(ready));
  offline->when  (  allChildrenInState(not_ready,ready,paused,running), moveTo(not_ready));

  error->when    (  anyChildInState(error),        moveTo(error));
  error->when    (  allChildrenInState(offline),   moveTo(offline));
  error->when    (  allChildrenInState(running),   moveTo(running));
  error->when    (  allChildrenInState(ready,paused,running),     moveTo(ready));
  error->when    (  allChildrenInState(not_ready,ready,paused,running), moveTo(not_ready));

  Tr*  reset0    = daq->addTransition("reset",     not_ready,   not_ready);
  Tr*  reset1    = daq->addTransition("reset",     ready,       not_ready, NO_CHECKS);
  Tr*  reset2    = daq->addTransition("reset",     error,       not_ready, NO_CHECKS);
  Tr*  reset3    = daq->addTransition("reset",     offline,     offline,   NO_CHECKS);

  Tr*  recover0  = daq->addTransition("recover",   error,       offline,   NO_CHECKS);
  Tr*  recover1  = daq->addTransition("recover",   running,     offline,   NO_CHECKS);
  Tr*  recover2  = daq->addTransition("recover",   ready,       offline,   NO_CHECKS);
  Tr*  recover3  = daq->addTransition("recover",   not_ready,   offline,   NO_CHECKS);
  Tr*  recover4  = daq->addTransition("recover",   offline,     offline,   NO_CHECKS);

  Tr*  daq_err0  = daq->addTransition("daq_err",   not_ready,   error,     NO_CHECKS);
  Tr*  daq_err1  = daq->addTransition("daq_err",   ready,       error,     NO_CHECKS);
  Tr*  daq_err2  = daq->addTransition("daq_err",   running,     error,     NO_CHECKS);
  Tr*  daq_err3  = daq->addTransition("daq_err",   paused,      error,     NO_CHECKS);
  Tr*  daq_err4  = daq->addTransition("daq_err",   offline,     error,     NO_CHECKS);

  Tr*  RESET0    = daq->addTransition("RESET",     error,       offline,   NO_CHECKS);
  Tr*  RESET1    = daq->addTransition("RESET",     running,     offline,   NO_CHECKS);
  Tr*  RESET2    = daq->addTransition("RESET",     ready,       offline,   NO_CHECKS);
  Tr*  RESET3    = daq->addTransition("RESET",     not_ready,   offline,   NO_CHECKS);
  Tr*  RESET4    = daq->addTransition("RESET",     offline,     offline,   NO_CHECKS);
  Tr*  RESET5    = daq->addTransition("RESET",     paused,      offline,   NO_CHECKS);

  daq->addTransition("When-Rule",     error,       not_ready, CHECK);
  daq->addTransition("When-Rule",     error,       ready,     CHECK);
  daq->addTransition("When-Rule",     error,       running,   CHECK);
  daq->addTransition("When-Rule",     error,       paused,    CHECK);

  daq->addTransition("When-Rule",   offline,       ready,     CHECK);
  daq->addTransition("When-Rule",   offline,       running,   CHECK);
  daq->addTransition("When-Rule",   offline,       paused,    CHECK);

  daq_err0->adoptRule(AllChildrenOfType(daq).moveTo(error));
  daq_err1->adoptRule(AllChildrenOfType(daq).moveTo(error));
  daq_err2->adoptRule(AllChildrenOfType(daq).moveTo(error));
  daq_err3->adoptRule(AllChildrenOfType(daq).moveTo(error));
  //daq_err4->adoptRule(AnyChildOfType(daq).moveTo(error));

  //daq->adoptRule      (recover0,   daq, error,     offline);
  // Otherwise: FULL KILL on recover
  recover0->adoptRule(AllChildrenOfType(daq).move(error,     offline));
  recover1->adoptRule(AllChildrenOfType(daq).move(error,     offline));
  recover2->adoptRule(AllChildrenOfType(daq).move(error,     offline));
  recover3->adoptRule(AllChildrenOfType(daq).move(error,     offline));
  recover4->adoptRule(AllChildrenOfType(daq).move(error,     offline));

  RESET0->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET1->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET2->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET3->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET4->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET5->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));

  reset0->adoptRule(  AllChildrenOfType(daq).move(not_ready, not_ready));
  reset0->adoptRule(  AllChildrenOfType(daq).move(ready,     not_ready));
  reset0->adoptRule(  AllChildrenOfType(daq).move(error,     not_ready));

  reset1->adoptRule(  AllChildrenOfType(daq).move(not_ready, not_ready));
  reset1->adoptRule(  AllChildrenOfType(daq).move(ready,     not_ready));
  reset1->adoptRule(  AllChildrenOfType(daq).move(error,     not_ready));

  reset2->adoptRule(  AllChildrenOfType(daq).move(not_ready, not_ready));
  reset2->adoptRule(  AllChildrenOfType(daq).move(ready,     not_ready));
  //reset2->adoptRule(  AllChildrenOfType(daq).move(running,   ready));
  reset2->adoptRule(  AllChildrenOfType(daq).move(error,     not_ready));

  reset3->adoptRule(  AllChildrenOfType(daq).move(not_ready, not_ready));
  reset3->adoptRule(  AllChildrenOfType(daq).move(ready,     not_ready));
  reset3->adoptRule(  AllChildrenOfType(daq).move(paused,    ready));
  //reset3->adoptRule(  AllChildrenOfType(daq).move(running,   ready));
  reset3->adoptRule(  AllChildrenOfType(daq).move(error,     not_ready));

  /// Offline -> Not Ready
  Tr*  load      = daq->addTransition("load",      offline,     not_ready, CHECK|CREATE);
  load->addPredicate( AllChildrenOfType(daq).inState(offline, not_ready, ready, paused, running));
  load->adoptRule(    AllChildrenOfType(daq).move(offline,   not_ready));
  load->adoptRule(    AllChildrenOfType(daq).move(paused,    ready));

  /// Not Ready -> Ready
  Tr*  configure = daq->addTransition("configure", not_ready,   ready);
  configure->addPredicate(AllChildrenOfType(daq).inState(not_ready, ready, paused, running));
  configure->adoptRule(   AllChildrenOfType(daq).move(not_ready, ready));
  configure->adoptRule(   AllChildrenOfType(daq).move(paused,    ready));

  /// Ready -> Running
  Tr*  start0    = daq->addTransition("start",     ready,       running);
  start0->addPredicate(AllChildrenOfType(daq).inState(ready,     paused,  running));
  start0->adoptRule(   AllChildrenOfType(daq).move(ready,     running));
  start0->adoptRule(   AllChildrenOfType(daq).move(paused,    running));

  /// Running -> Paused
  Tr*  pause     = daq->addTransition("pause",     running,     paused);
  pause->adoptRule(    AllChildrenOfType(daq).move(running,   paused,  Rule::MASTER2SLAVE));

  /// Paused -> Running
  Tr*  start1    = daq->addTransition("start",     paused,      running);
  start1->addPredicate(AllChildrenOfType(daq).inState(ready, paused, running));
  start1->adoptRule(   AllChildrenOfType(daq).move(ready,     running));
  start1->adoptRule(   AllChildrenOfType(daq).move(paused,    running));

  /// Paused -> Ready
  Tr*  stop1     = daq->addTransition("stop",      paused,      ready);
  stop1->adoptRule(    AllChildrenOfType(daq).move(paused,    ready));
  stop1->adoptRule(    AllChildrenOfType(daq).move(running,   ready));

  /// Running -> Ready
  Tr*  stop0     = daq->addTransition("stop",      running,     ready);
  stop0->adoptRule(    AllChildrenOfType(daq).move(running,   ready));

  /// Ready -> Ready
  Tr*  stop2     = daq->addTransition("stop",      ready,       ready);
  stop2->adoptRule(    AllChildrenOfType(daq).move(running,   ready));

  /// In case Children died, the controller is OFFLINE; still must accept commands
  Tr*  stop3     = daq->addTransition("stop",      offline,     offline,        NO_CHECKS);
  stop3->adoptRule(    AllChildrenOfType(daq).move(paused,    ready));
  stop3->adoptRule(    AllChildrenOfType(daq).move(running,   ready));

  // Unload while RUNNING: let's avoid it if possible!
  Tr*  unload0   = daq->addTransition("unload",    running,     offline,   NO_CHECKS);
  unload0->adoptRule(  AllChildrenOfType(daq).move(running,   offline, Rule::MASTER2SLAVE));

  // Unload while READY:   let's avoid it if possible!
  //Tr*  unload1   = daq->addTransition("unload",    ready,       offline, NO_CHECKS);

  // Normal case: all slaves are NOT_READY
  Tr*  unload2   = daq->addTransition("unload",    not_ready,   offline,   KILL);
  unload2->adoptRule(  AllChildrenOfType(daq).move(running,   offline, Rule::MASTER2SLAVE));
  unload2->adoptRule(  AllChildrenOfType(daq).move(ready,     offline, Rule::MASTER2SLAVE));
  unload2->adoptRule(  AllChildrenOfType(daq).move(not_ready, offline));

  // If we have dead children, the master is OFFLINE. Still propagate the commands
  Tr*  unload3   = daq->addTransition("unload",    offline,     offline,   NO_CHECKS);
  unload3->adoptRule(  AllChildrenOfType(daq).move(running,   offline, Rule::MASTER2SLAVE));
  unload3->adoptRule(  AllChildrenOfType(daq).move(ready,     offline, Rule::MASTER2SLAVE));
  unload3->adoptRule(  AllChildrenOfType(daq).move(not_ready, offline));

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
 
  not_ready->when(anyChildInState(error,daq_error),              moveTo(error));
  not_ready->when(anyChildInState(offline,daq_offline),          moveTo(offline));
  not_ready->when(allChildrenInState(daq_ready,daq_running,daq_paused),     moveTo(ready));

  ready->when    (  anyChildInState(error,daq_error),            moveTo(error));
  ready->when    (  anyChildInState(offline,daq_offline),        moveTo(offline));
  ready->when    (  allChildrenInState(running,daq_running),     moveTo(running));
  ready->when    (  allChildrenInState(not_ready,daq_not_ready), moveTo(not_ready));

  running->when  (  anyChildInState(error,daq_error),            moveTo(error));
  running->when  (  anyChildInState(offline,daq_offline),        moveTo(offline));
  running->when  (  anyChildInState(paused,daq_paused),          moveTo(paused));
  running->when  (  allChildrenInState(ready,daq_ready,paused,daq_paused),  moveTo(ready));

  paused->when   (  anyChildInState(error,daq_error),            moveTo(error));
  paused->when   (  anyChildInState(offline,daq_offline),        moveTo(offline));
  paused->when   (  allChildrenInState(daq_running),             moveTo(running));
  paused->when   (  allChildrenInState(daq_ready),               moveTo(ready));

  offline->when  (  allChildrenInState(running,daq_running),     moveTo(running));
  offline->when  (  allChildrenInState(ready,daq_ready,daq_paused,daq_running),         moveTo(ready));
  offline->when  (  allChildrenInState(not_ready,daq_not_ready,daq_ready,daq_paused,daq_running), moveTo(not_ready));

  error->when    (  anyChildInState(error,daq_error),            moveTo(error));
  error->when    (  allChildrenInState(offline,daq_offline),     moveTo(offline));
  error->when    (  allChildrenInState(running,daq_running),     moveTo(running));
  error->when    (  allChildrenInState(ready,daq_ready,daq_paused,daq_running),         moveTo(ready));
  error->when    (  allChildrenInState(not_ready,daq_not_ready,daq_ready,daq_paused,daq_running), moveTo(not_ready));

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
  Tr*  recover0  = typ->addTransition("recover",   error,       offline);
  Tr*  recover1  = typ->addTransition("recover",   not_ready,   offline);

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

  typ->addTransition("When-Rule",     error,       not_ready, CHECK);
  typ->addTransition("When-Rule",     error,       ready,     CHECK);
  typ->addTransition("When-Rule",     error,       running,   CHECK);
  typ->addTransition("When-Rule",     error,       paused,    CHECK);

  typ->addTransition("When-Rule",   offline,       ready,     CHECK);
  typ->addTransition("When-Rule",   offline,       running,   CHECK);
  typ->addTransition("When-Rule",   offline,       paused,    CHECK);

  daq_err0->adoptRule(    AllChildrenOfType(daq).moveTo(ST_NAME_ERROR));
  daq_err1->adoptRule(    AllChildrenOfType(daq).moveTo(ST_NAME_ERROR));
  daq_err2->adoptRule(    AllChildrenOfType(daq).moveTo(ST_NAME_ERROR));
  daq_err3->adoptRule(    AllChildrenOfType(daq).moveTo(ST_NAME_ERROR));
  recover0->adoptRule(    AllChildrenOfType(daq).move(ST_NAME_ERROR,     ST_NAME_OFFLINE));

  reset1->adoptRule(      AllChildrenOfType(daq).move(ST_NAME_READY,     ST_NAME_NOT_READY));
  load->adoptRule(        AllChildrenOfType(daq).move(ST_NAME_OFFLINE,   ST_NAME_NOT_READY));

  configure->addPredicate(AllChildrenOfType(daq).inState(ST_NAME_NOT_READY, ST_NAME_READY, ST_NAME_PAUSED, ST_NAME_RUNNING));
  configure->adoptRule(   AllChildrenOfType(daq).move(ST_NAME_NOT_READY, ST_NAME_READY));

  start0->addPredicate(   AllChildrenOfType(daq).inState(ST_NAME_READY, ST_NAME_PAUSED, ST_NAME_RUNNING));
  start0->adoptRule(      AllChildrenOfType(daq).move(ST_NAME_READY,     ST_NAME_RUNNING));
  start0->adoptRule(      AllChildrenOfType(daq).move(ST_NAME_PAUSED,    ST_NAME_RUNNING));

  start1->addPredicate(   AllChildrenOfType(daq).inState(ST_NAME_READY, ST_NAME_PAUSED, ST_NAME_RUNNING));
  start1->adoptRule(      AllChildrenOfType(daq).move(ST_NAME_READY,     ST_NAME_RUNNING));
  start1->adoptRule(      AllChildrenOfType(daq).move(ST_NAME_PAUSED,    ST_NAME_RUNNING));

  pause->adoptRule(       AllChildrenOfType(daq).move(ST_NAME_RUNNING,   ST_NAME_PAUSED, Rule::MASTER2SLAVE));
  stop0->adoptRule(       AllChildrenOfType(daq).move(ST_NAME_RUNNING,   ST_NAME_READY));
  stop1->adoptRule(       AllChildrenOfType(daq).move(ST_NAME_PAUSED,    ST_NAME_READY));

  unload1->adoptRule(     AllChildrenOfType(daq).move(ST_NAME_NOT_READY, ST_NAME_OFFLINE));
  unload2->adoptRule(     AllChildrenOfType(daq).move(ST_NAME_NOT_READY, ST_NAME_OFFLINE));
  unload2->adoptRule(     AllChildrenOfType(daq).move(ST_NAME_OFFLINE,   ST_NAME_OFFLINE));
  destroy->adoptRule(     AllChildrenOfType(daq).move(ST_NAME_ANY,       ST_NAME_OFFLINE));

  RESET0->adoptRule(      AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET1->adoptRule(      AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET2->adoptRule(      AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET3->adoptRule(      AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET4->adoptRule(      AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET5->adoptRule(      AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
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
