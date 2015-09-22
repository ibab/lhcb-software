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

enum DaqType  {
  DAQ_NORMAL=0,
  DAQ_PAUSE_ALL=1<<0
};

/// Create DAQ machine type
static Type* defineDAQActiveType(DaqType daqType = DAQ_NORMAL)    {
  typedef Transition Tr;
  Type *daq = new Type("DAQ");
  const State* offline   = daq->addState(ST_NAME_OFFLINE, State::STARTUP);
  const State* not_ready = daq->addState(ST_NAME_NOT_READY);
  const State* ready     = daq->addState(ST_NAME_READY);
  const State* active    = daq->addState(ST_NAME_ACTIVE);
  const State* running   = daq->addState(ST_NAME_RUNNING);
  const State* error     = daq->addState(ST_NAME_ERROR, State::FAIL);
  const State* paused    = daq->addState(ST_NAME_PAUSED);

  daq->setInitialState(offline);

  not_ready->when(anyChildInState(error),          moveTo(error));
  not_ready->when(anyChildInState(offline),        moveTo(offline));
  not_ready->when(allChildrenInState(ready,active,running,paused), moveTo(ready));

  ready->when    (  anyChildInState(error),        moveTo(error));
  ready->when    (  anyChildInState(offline),      moveTo(offline));
  ready->when    (  anyChildInState(not_ready),    moveTo(not_ready));
  ready->when    (  allChildrenInState(active),   moveTo(active));
  ready->when    (  allChildrenInState(running),   moveTo(running));
  ready->when    (  allChildrenInState(running,paused),   moveTo(paused));

  running->when  (  anyChildInState(error),        moveTo(error));
  running->when  (  anyChildInState(offline),      moveTo(offline));
  running->when  (  anyChildInState(not_ready),    moveTo(not_ready));
  if ( daqType&DAQ_PAUSE_ALL )
    running->when(  allChildrenInState(paused),       moveTo(paused));
  else
    running->when(  anyChildInState(paused),       moveTo(paused));
  running->when  (  allChildrenInState(ready),     moveTo(ready));
  running->when  (  allChildrenInState(active),     moveTo(active));
  //running->when  (  allChildrenInState(running),   moveTo(running));

  paused->when   (  anyChildInState(error),        moveTo(error));
  paused->when   (  anyChildInState(offline),      moveTo(offline));
  paused->when   (  anyChildInState(not_ready),    moveTo(not_ready));
  paused->when   (  allChildrenInState(running),   moveTo(running));
  paused->when   (  allChildrenInState(ready),     moveTo(ready));
  paused->when   (  allChildrenInState(active),     moveTo(active));

  offline->when  (  anyChildInState(error),        moveTo(error));
  offline->when  (  allChildrenInState(running),   moveTo(running));
  offline->when  (  allChildrenInState(active,running), moveTo(active));
  offline->when  (  allChildrenInState(ready,active,running), moveTo(ready));
  offline->when  (  allChildrenInState(ready,active,paused,running), moveTo(paused));
  offline->when  (  allChildrenInState(not_ready,ready,active,paused,running), moveTo(not_ready));

  error->when    (  anyChildInState(error),        moveTo(error));
  error->when    (  allChildrenInState(running),   moveTo(running));
  error->when    (  allChildrenInState(active,running), moveTo(active));
  error->when    (  allChildrenInState(ready,active,running), moveTo(ready));
  error->when    (  allChildrenInState(ready,active,paused,running), moveTo(paused));
  error->when    (  allChildrenInState(not_ready,ready,active,paused,running), moveTo(not_ready));
  error->when    (  allChildrenInState(offline,not_ready,ready,active,paused,running), moveTo(offline));

  Tr*  reset0    = daq->addTransition("reset",     not_ready,   not_ready, NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  reset1    = daq->addTransition("reset",     ready,       not_ready, NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  reset2    = daq->addTransition("reset",     active,      not_ready, NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  reset3    = daq->addTransition("reset",     offline,     offline,   NO_CHECKS|ON_TIMEOUT_KILL);

  Tr*  daq_err0  = daq->addTransition("daq_err",   not_ready,   error,     NO_CHECKS);
  Tr*  daq_err1  = daq->addTransition("daq_err",   ready,       error,     NO_CHECKS);
  Tr*  daq_err2  = daq->addTransition("daq_err",   active,      error,     NO_CHECKS);
  Tr*  daq_err3  = daq->addTransition("daq_err",   running,     error,     NO_CHECKS);
  Tr*  daq_err4  = daq->addTransition("daq_err",   paused,      error,     NO_CHECKS);
  /*Tr*  daq_err4  =*/ daq->addTransition("daq_err",   offline,     error,     NO_CHECKS);

  Tr*  RESET0    = daq->addTransition("RESET",     error,       offline,   NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  RESET1    = daq->addTransition("RESET",     running,     offline,   NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  RESET2    = daq->addTransition("RESET",     ready,       offline,   NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  RESET3    = daq->addTransition("RESET",     active,      offline,   NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  RESET4    = daq->addTransition("RESET",     not_ready,   offline,   NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  RESET5    = daq->addTransition("RESET",     offline,     offline,   NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  RESET6    = daq->addTransition("RESET",     paused,      offline,   NO_CHECKS|ON_TIMEOUT_KILL);

  daq->addTransition("When-Rule",     error,       not_ready, CHECK);
  daq->addTransition("When-Rule",     error,       ready,     CHECK);
  daq->addTransition("When-Rule",     error,       active,    CHECK);
  daq->addTransition("When-Rule",     error,       running,   CHECK);
  daq->addTransition("When-Rule",     error,       paused,    CHECK);

  daq->addTransition("When-Rule",   offline,       ready,     CHECK);
  daq->addTransition("When-Rule",   offline,       active,    CHECK);
  daq->addTransition("When-Rule",   offline,       running,   CHECK);
  daq->addTransition("When-Rule",   offline,       paused,    CHECK);

  daq_err0->adoptRule(AllChildrenOfType(daq).moveTo(error));
  daq_err1->adoptRule(AllChildrenOfType(daq).moveTo(error));
  daq_err2->adoptRule(AllChildrenOfType(daq).moveTo(error));
  daq_err3->adoptRule(AllChildrenOfType(daq).moveTo(error));
  daq_err4->adoptRule(AllChildrenOfType(daq).moveTo(error));

  // Otherwise: FULL KILL on recover
  int flags = KILL|NO_CHECKS;//NO_CHECKS|ON_TIMEOUT_KILL
  Tr*  recover0  = daq->addTransition("recover",   error,     offline,   flags);
  Tr*  recover1  = daq->addTransition("recover",   running,   offline,   flags);
  Tr*  recover2  = daq->addTransition("recover",   ready,     offline,   flags);
  Tr*  recover3  = daq->addTransition("recover",   active,    offline,   flags);
  /* Tr*  recover4 = */ daq->addTransition("recover",   not_ready, offline,   flags);
  Tr*  recover5  = daq->addTransition("recover",   paused,    offline,   flags);

  recover0->adoptRule( AllChildrenOfType(daq).execTransition(daq->transitionsByName("recover")));
  recover1->adoptRule( AllChildrenOfType(daq).execTransition(daq->transitionsByName("recover")));
  recover2->adoptRule( AllChildrenOfType(daq).execTransition(daq->transitionsByName("recover")));
  recover3->adoptRule( AllChildrenOfType(daq).execTransition(daq->transitionsByName("recover")));
  recover5->adoptRule( AllChildrenOfType(daq).execTransition(daq->transitionsByName("recover")));

#if 0
#endif
  RESET0->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET1->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET2->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET3->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET4->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET5->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET6->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));

  reset0->adoptRule(reset0).adoptRule(reset1).adoptRule(reset2).adoptRule(reset3);
  reset1->adoptRule(reset0).adoptRule(reset1).adoptRule(reset2).adoptRule(reset3);
  reset2->adoptRule(reset0).adoptRule(reset1).adoptRule(reset2).adoptRule(reset3);
  reset3->adoptRule(reset0).adoptRule(reset1).adoptRule(reset2).adoptRule(reset3);

  /// Offline -> Not Ready
  Tr*  load      = daq->addTransition("load",    offline,     not_ready, CHECK|CREATE);
  load->addPredicate( AllChildrenOfType(daq).inState(offline, not_ready, ready, active, paused, running));
  load->adoptRule(    AllChildrenOfType(daq).move(offline,    not_ready));
  load->adoptRule(    AllChildrenOfType(daq).move(paused,     ready));
  load->adoptRule(    AllChildrenOfType(daq).move(offline,    ready)); // --> Checkpointing

  /// Not Ready -> Ready
  Tr*  configure = daq->addTransition("configure", not_ready,    ready);
  configure->addPredicate(AllChildrenOfType(daq).inState(not_ready, ready, active, paused, running));
  configure->adoptRule(   AllChildrenOfType(daq).move(not_ready, ready));
  configure->adoptRule(   AllChildrenOfType(daq).move(paused,    ready));

  /// Ready     -> Active
  Tr*  start0    = daq->addTransition("activate",  ready,       active);
  start0->addPredicate(AllChildrenOfType(daq).inState(ready,    active,  paused,  running));
  start0->adoptRule(start0);

  /// Active     -> Running
  Tr*  go1    = daq->addTransition("go",        active,       running);
  go1->addPredicate(AllChildrenOfType(daq).inState(ready,    active,  paused,  running));
  go1->adoptRule(go1);

  /// Running   -> Paused
  Tr*  pause     = daq->addTransition("pause",     running,     paused);
  pause->adoptRule(ParentOfType(daq).move(running, paused));
  pause->adoptRule(pause);

  /// Ready     -> Paused
  pause          = daq->addTransition("pause",     ready,     paused);
  pause->adoptRule(ParentOfType(daq).move(ready, paused));
  pause->adoptRule(pause);

  /// Paused    -> Running for children in state paused
  Tr*  cont      = daq->addTransition("continue",  paused,      running);
  cont->addPredicate(AllChildrenOfType(daq).inState(paused, active, running));
  cont->adoptRule(   AllChildrenOfType(daq).move(active, running));
  cont->adoptRule(   AllChildrenOfType(daq).move(paused, running));

  /// Running   -> Ready
  Tr*  stop0     = daq->addTransition("stop",      running,     ready);
  /// Active    -> Ready
  Tr*  stop1     = daq->addTransition("stop",      active,      ready);
  /// Paused    -> Ready
  Tr*  stop2     = daq->addTransition("stop",      paused,      ready);
  /// Ready     -> Ready
  Tr*  stop3     = daq->addTransition("stop",      ready,       ready);
  /// In case Children died, the controller is OFFLINE; still must accept commands
  Tr*  stop4     = daq->addTransition("stop",      offline,     offline,   NO_CHECKS);

  stop0->adoptRule(stop0).adoptRule(stop1).adoptRule(stop2);  // RUNNING: on stop move all {PAUSED,RUNNING,ACTIVE} to READY
  stop1->adoptRule(stop0).adoptRule(stop1).adoptRule(stop2);  // PAUSED:  on stop move all {PAUSED,RUNNING,ACTIVE} to READY
  stop2->adoptRule(stop0).adoptRule(stop1).adoptRule(stop2);  // ACTIVE:  on stop move all {PAUSED,RUNNING,ACTIVE} to READY
  stop3->adoptRule(stop0).adoptRule(stop1).adoptRule(stop2);  // READY:   on stop move all {PAUSED,RUNNING,ACTIVE} to READY
  stop4->adoptRule(stop0).adoptRule(stop1).adoptRule(stop2);  // OFFLINE: on stop move all {PAUSED,RUNNING,ACTIVE} to READY

  // Normal case: all slaves are NOT_READY
  Tr*  unload2   = daq->addTransition("unload",    not_ready,   offline,   KILL);
  // If we have dead children, the master is OFFLINE. Still propagate the commands
  Tr*  unload3   = daq->addTransition("unload",    offline,     offline,   NO_CHECKS);
  unload2->adoptRule(unload2).adoptRule(unload3);
  unload3->adoptRule(unload2).adoptRule(unload3);

  return daq;
}

/// Create DAQ machine type
static Type* defineDAQType(DaqType daqType = DAQ_NORMAL)    {
  typedef Transition Tr;
  Type *daq = new Type(daqType&DAQ_PAUSE_ALL ? "DAQPause" : "DAQ");
  const State* offline   = daq->addState(ST_NAME_OFFLINE, State::STARTUP);
  const State* not_ready = daq->addState(ST_NAME_NOT_READY);
  const State* ready     = daq->addState(ST_NAME_READY);
  const State* running   = daq->addState(ST_NAME_RUNNING);
  const State* error     = daq->addState(ST_NAME_ERROR, State::FAIL);
  const State* paused    = daq->addState(ST_NAME_PAUSED);

  daq->setInitialState(offline);

  not_ready->when(anyChildInState(error),          moveTo(error));
  not_ready->when(anyChildInState(offline),        moveTo(offline));
  not_ready->when(allChildrenInState(ready,running,paused), moveTo(ready));

  ready->when    (  anyChildInState(error),        moveTo(error));
  ready->when    (  anyChildInState(offline),      moveTo(offline));
  ready->when    (  anyChildInState(not_ready),    moveTo(not_ready));
  ready->when    (  allChildrenInState(running),   moveTo(running));
  ready->when    (  allChildrenInState(running,paused),   moveTo(paused));

  running->when  (  anyChildInState(error),        moveTo(error));
  running->when  (  anyChildInState(offline),      moveTo(offline));
  running->when  (  anyChildInState(not_ready),    moveTo(not_ready));
  if ( daqType&DAQ_PAUSE_ALL )
    running->when(  allChildrenInState(paused),    moveTo(paused));
  else
    running->when(  anyChildInState(paused),       moveTo(paused));
  running->when  (  allChildrenInState(ready),     moveTo(ready));
  //running->when  (  allChildrenInState(running),   moveTo(running));

  paused->when   (  anyChildInState(error),        moveTo(error));
  paused->when   (  anyChildInState(offline),      moveTo(offline));
  paused->when   (  anyChildInState(not_ready),    moveTo(not_ready));
  paused->when   (  allChildrenInState(running),   moveTo(running));
  paused->when   (  allChildrenInState(ready),     moveTo(ready));

  offline->when  (  anyChildInState(error),        moveTo(error));
  offline->when  (  allChildrenInState(running),   moveTo(running));
  offline->when  (  allChildrenInState(ready,running), moveTo(ready));
  offline->when  (  allChildrenInState(ready,paused,running), moveTo(paused));
  offline->when  (  allChildrenInState(not_ready,ready,paused,running), moveTo(not_ready));

  error->when    (  anyChildInState(error),        moveTo(error));
  error->when    (  allChildrenInState(running),   moveTo(running));
  error->when    (  allChildrenInState(ready,running), moveTo(ready));
  error->when    (  allChildrenInState(ready,paused,running), moveTo(paused));
  error->when    (  allChildrenInState(not_ready,ready,paused,running), moveTo(not_ready));
  error->when    (  allChildrenInState(offline,not_ready,ready,paused,running), moveTo(offline));

  Tr*  reset0    = daq->addTransition("reset",     not_ready,   not_ready, NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  reset1    = daq->addTransition("reset",     ready,       not_ready, NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  reset3    = daq->addTransition("reset",     offline,     offline,   NO_CHECKS|ON_TIMEOUT_KILL);

  Tr*  daq_err0  = daq->addTransition("daq_err",   not_ready,   error,     NO_CHECKS);
  Tr*  daq_err1  = daq->addTransition("daq_err",   ready,       error,     NO_CHECKS);
  Tr*  daq_err2  = daq->addTransition("daq_err",   running,     error,     NO_CHECKS);
  Tr*  daq_err3  = daq->addTransition("daq_err",   paused,      error,     NO_CHECKS);
  /*Tr*  daq_err4  =*/ daq->addTransition("daq_err",   offline,     error,     NO_CHECKS);

  Tr*  RESET0    = daq->addTransition("RESET",     error,       offline,   NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  RESET1    = daq->addTransition("RESET",     running,     offline,   NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  RESET2    = daq->addTransition("RESET",     ready,       offline,   NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  RESET3    = daq->addTransition("RESET",     not_ready,   offline,   NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  RESET4    = daq->addTransition("RESET",     offline,     offline,   NO_CHECKS|ON_TIMEOUT_KILL);
  Tr*  RESET5    = daq->addTransition("RESET",     paused,      offline,   NO_CHECKS|ON_TIMEOUT_KILL);

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

  // Otherwise: FULL KILL on recover
  int flags = KILL|NO_CHECKS;//NO_CHECKS|ON_TIMEOUT_KILL
  Tr*  recover0  = daq->addTransition("recover",   error,     offline,   flags);
  Tr*  recover1  = daq->addTransition("recover",   running,   offline,   flags);
  Tr*  recover2  = daq->addTransition("recover",   ready,     offline,   flags);
  Tr*  recover3  = daq->addTransition("recover",   not_ready, offline,   flags);
  Tr*  recover5  = daq->addTransition("recover",   paused,    offline,   flags);

  recover0->adoptRule( AllChildrenOfType(daq).execTransition(daq->transitionsByName("recover")));
  recover1->adoptRule( AllChildrenOfType(daq).execTransition(daq->transitionsByName("recover")));
  recover2->adoptRule( AllChildrenOfType(daq).execTransition(daq->transitionsByName("recover")));
  recover3->adoptRule( AllChildrenOfType(daq).execTransition(daq->transitionsByName("recover")));
  recover5->adoptRule( AllChildrenOfType(daq).execTransition(daq->transitionsByName("recover")));
#if 0
#endif
  RESET0->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET1->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET2->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET3->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET4->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));
  RESET5->adoptRule(  AllChildrenOfType(daq).execTransition(daq->transitionsByName("RESET")));

  reset0->adoptRule(reset0).adoptRule(reset1).adoptRule(reset3);
  reset1->adoptRule(reset0).adoptRule(reset1).adoptRule(reset3);
  reset3->adoptRule(reset0).adoptRule(reset1).adoptRule(reset3);

  /// Offline -> Not Ready
  Tr*  load      = daq->addTransition("load",    offline,     not_ready, CHECK|CREATE);
  load->addPredicate( AllChildrenOfType(daq).inState(offline, not_ready, ready, paused, running));
  load->adoptRule(    AllChildrenOfType(daq).move(offline,    not_ready));
  load->adoptRule(    AllChildrenOfType(daq).move(paused,     ready));
  load->adoptRule(    AllChildrenOfType(daq).move(offline,    ready)); // --> Checkpointing

  /// Not Ready -> Ready
  Tr*  configure = daq->addTransition("configure", not_ready,    ready);
  configure->addPredicate(AllChildrenOfType(daq).inState(not_ready, ready, paused, running));
  configure->adoptRule(   AllChildrenOfType(daq).move(not_ready, ready));
  configure->adoptRule(   AllChildrenOfType(daq).move(paused,    ready));

  /// Ready     -> Running
  Tr*  start0    = daq->addTransition("start",     ready,       running);
  start0->addPredicate(AllChildrenOfType(daq).inState(ready,    paused,  running));
#if 0  
  /// Paused    -> Running
  Tr*  start1    = daq->addTransition("start",     paused,      running);
  start1->addPredicate(AllChildrenOfType(daq).inState(ready,    running));
  //start1->addPredicate(AllChildrenOfType(daq).inState(ready,    paused,  running));

  start0->adoptRule(start0).adoptRule(start1);
  start1->adoptRule(start0).adoptRule(start1);
#endif
  start0->adoptRule(start0);

  /// Running   -> Paused
  Tr*  pause     = daq->addTransition("pause",     running,     paused);
  pause->adoptRule(ParentOfType(daq).move(running, paused));
  pause->adoptRule(pause);

  /// Ready     -> Paused
  pause          = daq->addTransition("pause",     ready,     paused);
  pause->adoptRule(ParentOfType(daq).move(ready, paused));
  pause->adoptRule(pause);

  /// Paused    -> Running for children in state paused
  Tr*  cont      = daq->addTransition("continue",  paused,      running);
  cont->addPredicate(AllChildrenOfType(daq).inState(paused, ready, running));
  cont->adoptRule(   AllChildrenOfType(daq).move(ready,  running));
  cont->adoptRule(   AllChildrenOfType(daq).move(paused, running));

  /// Running   -> Ready
  Tr*  stop0     = daq->addTransition("stop",      running,     ready);
  /// Paused    -> Ready
  Tr*  stop1     = daq->addTransition("stop",      paused,      ready);
  /// Ready     -> Ready
  Tr*  stop2     = daq->addTransition("stop",      ready,       ready);
  /// In case Children died, the controller is OFFLINE; still must accept commands
  Tr*  stop3     = daq->addTransition("stop",      offline,     offline,   NO_CHECKS);

  stop0->adoptRule(stop0).adoptRule(stop1);  // RUNNING: on stop move all {PAUSED,RUNNING} to READY
  stop1->adoptRule(stop0).adoptRule(stop1);  // PAUSED:  on stop move all {PAUSED,RUNNING} to READY
  stop2->adoptRule(stop0).adoptRule(stop1);  // READY:   on stop move all {PAUSED,RUNNING} to READY
  stop3->adoptRule(stop0).adoptRule(stop1);  // OFFLINE: on stop move all {PAUSED,RUNNING} to READY

  // Normal case: all slaves are NOT_READY
  Tr*  unload2   = daq->addTransition("unload",    not_ready,   offline,   KILL);
  // If we have dead children, the master is OFFLINE. Still propagate the commands
  Tr*  unload3   = daq->addTransition("unload",    offline,     offline,   NO_CHECKS);
  unload2->adoptRule(unload2).adoptRule(unload3);
  unload3->adoptRule(unload2).adoptRule(unload3);

  return daq;
}

/// Create DAQ machine type
static Type* defineDAQSteerType() {
  typedef Transition Tr;
  Type* daq = fsm_type("DAQ");
  Type *typ = new Type("DAQSteer");
  const State* unknown   = typ->addState(ST_NAME_UNKNOWN, State::STARTUP);
  const State* offline   = typ->addState(ST_NAME_OFFLINE);
  const State* not_ready = typ->addState(ST_NAME_NOT_READY);
  const State* ready     = typ->addState(ST_NAME_READY);
  const State* running   = typ->addState(ST_NAME_RUNNING);
  const State* error     = typ->addState(ST_NAME_ERROR, State::FAIL);
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
  offline->when  (  allChildrenInState(ready,daq_ready,daq_paused,daq_running), moveTo(ready));
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
  /*Tr*  recover1  =*/ typ->addTransition("recover",   not_ready,   offline);

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

  pause->adoptRule(       AllChildrenOfType(daq).move(ST_NAME_RUNNING,   ST_NAME_PAUSED));
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
    if      ( typ == "DAQ"       ) return m[typ]=defineDAQType(DAQ_NORMAL);
    else if ( typ == "DAQPause"  ) return m[typ]=defineDAQType(DAQ_PAUSE_ALL);
    else if ( typ == "DAQActive" ) return m[typ]=defineDAQActiveType(DAQ_NORMAL);
    else if ( typ == "DAQSteer"  ) return m[typ]=defineDAQSteerType();
    throw runtime_error("Request for undefined FSM type:"+typ);
  }
  return (*i).second;
}
