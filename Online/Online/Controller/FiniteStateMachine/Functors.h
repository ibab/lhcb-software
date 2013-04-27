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

  class ByName  {
  protected:
    /// Object name
    const TypedObject* object;
  public:
    /// Standard constructor
    ByName(const TypedObject* o) : object(o) {    }
    /// Operator invoked for each predicate to check if it is fulfilled
    bool operator()(const TypedObject* p) const;
    /// Operator invoked for each predicate to check if it is fulfilled
    bool operator()(const std::pair<std::string,TypedObject*>& p) const;
  };
  
  class FindTransitionByState  {
  protected:
    /// Current transition pointer
    const State* object;
  public:
    /// Standard constructor
    FindTransitionByState(const State* s) : object(s) {}
    /// Operator invoked for each predicate to check if it is fulfilled
    bool operator()(const Transition* t) const;
  };

  class FindTransitionByName  {
  protected:
    /// Current transition pointer
    const std::string& object;
  public:
    /// Standard constructor
    FindTransitionByName(const std::string& s) : object(s) {}
    /// Operator invoked for each predicate to check if it is fulfilled
    bool operator()(const Transition* t) const;
  };

  class FindTransitionByStateName  {
  protected:
    /// Current transition pointer
    const std::string& object;
  public:
    /// Standard constructor
    FindTransitionByStateName(const std::string& s) : object(s) {}
    /// Operator invoked for each predicate to check if it is fulfilled
    bool operator()(const Transition* t) const;
  };

  template<typename T> struct ActionCounter  {
    const T* object;
    size_t count;
    size_t fail;
    size_t dead;
    /// Standard constructor
    ActionCounter(const T* o) : object(o), count(0), fail(0), dead(0) {}
  };

  struct SlaveStarter : public ActionCounter<Transition>  {
    /// Standard constructor
    SlaveStarter (const Transition* t) : ActionCounter(t) {}
    /// Operator invoked for each slave to be started
    void operator()(Slave* s);
  };
  struct SlaveKiller : public ActionCounter<Transition>  {
    /// Standard constructor
    SlaveKiller (const Transition* t) : ActionCounter(t) {}
    /// Operator invoked for each slave to be started
    void operator()(Slave* s);
  };
  struct SlaveLimboCount : public ActionCounter<void>  {
    /// Standard constructor
    SlaveLimboCount() : ActionCounter(0) {}
    /// Operator invoked for each slave to be checked
    void operator()(const Slave* s);
  };
  struct CheckStateSlave : public ActionCounter<Transition>  {
    /// Standard constructor
    CheckStateSlave(const Transition* t) : ActionCounter(t) {}
    /// Operator invoked for each slave being analyzed
    void operator()(const Slave* s);
  };

  template <class T> struct FsmCheckFunctor {
    /// Current reference object pointer
    T object;
    /// Internal object status
    int status;
    /// Standard constructor
    FsmCheckFunctor(T t) : object(t), status(FSM::SUCCESS) {}
    /// Check if predicates were fullfilled
    bool ok() const    {   return status == FSM::SUCCESS;     }
    /// Check if predicates were fullfilled
    bool wait() const  {   return status == FSM::WAIT_ACTION; }
  };
  struct SetSlaveState  : public FsmCheckFunctor<Slave::SlaveState> {
    /// Standard constructor
    SetSlaveState(Slave::SlaveState s) : FsmCheckFunctor<Slave::SlaveState>(s) {}
    /// Operator invoked for each predicate to check if it is fulfilled
    void operator()(Slave* s);
  };
  struct PredicateSlave : public FsmCheckFunctor<const Transition*> {
    /// Standard constructor
    PredicateSlave(const Transition* t) : FsmCheckFunctor<const Transition*>(t) {}
    /// Operator invoked for each predicate to check if it is fulfilled
    void operator()(const Slave* s);
  };
  struct InvokeSlave : public FsmCheckFunctor<const Transition*>  {
    /// Flag to indicate if the transition is MASTER2SLAVE or SLAVE2MASTER (see Rule.h)
    Rule::Direction direction;
    /// Standard constructor
    InvokeSlave(const Transition* t, Rule::Direction dir) : FsmCheckFunctor<const Transition*>(t), direction(dir) {}
    /// Operator invoked for each predicate to check if it is fulfilled
    void operator()(Slave* s);
  };
  
  struct InvokeSlave2 : public FsmCheckFunctor<const Transition*>  {
    /// Flag to indicate if the transition is MASTER2SLAVE or SLAVE2MASTER (see Rule.h)
    Rule::Direction direction;
    const std::vector<Slave*> slaves;
    /// Standard constructor
    InvokeSlave2(const std::vector<Slave*>& s, const Transition* t,Rule::Direction dir);
    /// Operator invoked for each predicate to check if it is fulfilled
    void operator()(const Rule* rule);
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

