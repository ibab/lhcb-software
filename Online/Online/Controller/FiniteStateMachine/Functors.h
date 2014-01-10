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
#ifndef ONLINE_FINITESTATEMACHINE_FUNCTORS_H
#define ONLINE_FINITESTATEMACHINE_FUNCTORS_H

// Framework include files
#include "FiniteStateMachine/Rule.h"
#include "FiniteStateMachine/Slave.h"

// C/C++ include files
#include <string>
#include <vector>
#include <algorithm>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine {

  // Forward declarations
  typedef FsmHandle<Machine> MachineHandle;

  template <typename T> class Delete  {
  public:
    /// Operator invoked for each object to be deleted
    void operator()(T* t) const  { delete t; }
  };

  template<typename T> struct Functor  {
    typedef Functor<T> Base;
    /// Functor Object
    T object;
    /// Standard constructor
    Functor(T o) : object(o) {}
  };

  template<typename T> struct ActionCounter : Functor<const T*> {
    /// Base class type
    typedef ActionCounter<T> Base;
    size_t count;
    size_t fail;
    size_t dead;
    /// Standard constructor
    ActionCounter(const T* o) : Functor<const T*>(o), count(0), fail(0), dead(0) {}
  };

  template <class T> struct FsmCheck : Functor<T> {
    /// Base class type
    typedef Functor<T> Base;
    /// Internal object status
    int status;
    /// Standard constructor
    FsmCheck(T t) : Functor<T>(t), status(FSM::SUCCESS) {}
    /// Check if predicates were fullfilled
    bool ok() const    {   return status == FSM::SUCCESS;     }
    /// Check if predicates were fullfilled
    bool wait() const  {   return status == FSM::WAIT_ACTION; }
  };

  struct ByName : Functor<const TypedObject*> {
    /// Standard constructor
    ByName(const TypedObject* o) : Base(o) {    }
    /// Operator invoked for each predicate to check if it is fulfilled
    bool operator()(const TypedObject* p) const;
    /// Operator invoked for each predicate to check if it is fulfilled
    bool operator()(const std::pair<std::string,TypedObject*>& p) const;
  };
  
  struct FindTransitionByState : Functor<const State*> {
    /// Standard constructor
    FindTransitionByState(const State* s) : Base(s) {}
    /// Operator invoked for each predicate to check if it is fulfilled
    bool operator()(const Transition* t) const;
  };

  struct FindTransitionByName : Functor<const std::string&> {
    /// Standard constructor
    FindTransitionByName(const std::string& s) : Base(s) {}
    /// Operator invoked for each predicate to check if it is fulfilled
    bool operator()(const Transition* t) const;
  };

  struct FindTransitionByStateName : Functor<const std::string&>  {
    /// Standard constructor
    FindTransitionByStateName(const std::string& s) : Base(s) {}
    /// Operator invoked for each predicate to check if it is fulfilled
    bool operator()(const Transition* t) const;
  };

  struct SlaveReset  {
    /// Operator invoked for each task to reset its state to idle
    void operator()(Slave* t) const;
  };

  struct SlaveStarter : public ActionCounter<Transition>  {
    /// Standard constructor
    SlaveStarter (const Transition* t) : ActionCounter<Transition>(t) {}
    /// Operator invoked for each slave to be started
    void operator()(Slave* s);
  };

  struct SlaveKiller : public ActionCounter<Transition>  {
    /// Standard constructor
    SlaveKiller (const Transition* t) : ActionCounter<Transition>(t) {}
    /// Operator invoked for each slave to be started
    void operator()(Slave* s);
  };

  struct SlaveLimboCount : public ActionCounter<void>  {
    /// Standard constructor
    SlaveLimboCount() : ActionCounter<void>(0) {}
    /// Operator invoked for each slave to be checked
    void operator()(const Slave* s);
  };

  struct CheckStateSlave : public ActionCounter<Transition>  {
    /// Standard constructor
    CheckStateSlave(const Transition* t) : ActionCounter<Transition>(t) {}
    /// Operator invoked for each slave being analyzed
    void operator()(const Slave* s);
  };

  struct SetSlaveState  : public ActionCounter<State> {
    /// Slave mask
    int mask;
    /// Standard constructor. By default all slaves are acted on
  SetSlaveState(const State* new_state, int match_mask=(int)Slave::SLAVE_NONE) 
      : ActionCounter<State>(new_state), mask(match_mask) {}
    /// Operator invoked for each predicate to check if it is fulfilled
    void operator()(Slave* s);
  };

  struct PredicateSlave : public FsmCheck<const Transition*> {
    /// Standard constructor
    PredicateSlave(const Transition* t) : FsmCheck<const Transition*>(t) {}
    /// Operator invoked for each predicate to check if it is fulfilled
    void operator()(const Slave* s);
  };

  struct InvokeSlave : public FsmCheck<const Transition*>  {
    /// Flag to indicate if the transition is MASTER2SLAVE or SLAVE2MASTER (see Rule.h)
    Rule::Direction direction;
    /// Standard constructor
    InvokeSlave(const Transition* t, Rule::Direction dir) : FsmCheck<const Transition*>(t), direction(dir) {}
    /// Operator invoked for each predicate to check if it is fulfilled
    void operator()(Slave* s);
  };
  
   struct PrintObject  {
    /// Printout prefix
    std::string prefix;
    /// Standard constructor
    PrintObject(const std::string& pref="") : prefix(pref) {}
    /// Map operator
    void operator()(const std::pair<const std::string,Transition*>& p) const{  this->operator()(p.second); }
    /// Map operator
    void operator()(const std::pair<const std::string,Machine*>& p) const   {  this->operator()(p.second); }
    /// Map operator
    void operator()(const std::pair<const std::string,Slave*>& p) const     {  this->operator()(p.second); }
    /// Map operator
    void operator()(const std::pair<const std::string,State*>& p) const     {  this->operator()(p.second); }
    /// Map operator
    void operator()(const std::pair<const std::string,Predicate*>& p) const {  this->operator()(p.second); }
    /// Map operator
    void operator()(const std::pair<const std::string,Rule*>& p) const      {  this->operator()(p.second); }
    /// Operator invoked for each Transition object printing.
    void operator()(const Transition* t) const;
    /// Operator invoked for each machine during printing.
    void operator()(const Machine* m) const;
    /// Operator invoked for each slave during printing.
    void operator()(const Slave* s) const;
    /// Operator invoked for each State object printout.
    void operator()(const State* s) const;
    /// Operator invoked for each Rule object printout.
    void operator()(const Predicate* p) const;
    /// Operator invoked for each Rule object printout.
    void operator()(const Rule* r) const;

    /// Operator invoked for each State object printout.
    void operator()(const StateHandle& s) const;
    /// Operator invoked for each Machine object printout.
    void operator()(const MachineHandle& s) const;
  };

  
}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_FUNCTORS_H

