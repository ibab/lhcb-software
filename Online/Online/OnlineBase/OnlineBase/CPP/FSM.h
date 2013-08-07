/*
**++
**  FACILITY:  DEXPERT  (Fsm.h)
**
**  MODULE DESCRIPTION:
**
**      Basic class to implement Finite State Machine objects
**
**  AUTHORS:
**
**      P. Mato
**
**  CREATION DATE:  12-Dec-1990
**
**  DESIGN ISSUES:
**
**      {@tbs@}
**
**
**  MODIFICATION HISTORY:
**
**      {@tbs@}...
**--
*/
#ifndef CPP_FSM_H
#define CPP_FSM_H 1

class FSM {
public:
  enum { IDLE_STATE=0 };
  enum ErrCond { 
    FSM_K_SUCCESS,
    FSM_K_FAIL,
    FSM_K_TRANNOTFOUND,
    FSM_K_BADPREACTION,
    FSM_K_BADPOSTACTION
  };
  typedef ErrCond (FSM::*ActionFunc)();

  typedef unsigned int State;	     // State type is defined as unsigned int
  class Transition {	             // Transition type is an structure
  private:
    Transition(const Transition&) : next(0), condition(0) {  }
    Transition& operator=(const Transition&) { return *this; }
  public:
    Transition(State from, State to, const char* cond, ActionFunc act);
    ~Transition();
    Transition* next;                // Pointer to the next transition
    State       from;                // Transition starting state 
    State       to;                  // Transition ending state
    char*       condition;           // Text string condition
    ActionFunc  action;              // Function porinter to be call 
  };

protected:
  State        m_currentState;       // Current state of the object
  State        m_previousState;      // Previous state
  Transition*  m_currentTransition;  // Current transition
  Transition*  m_head;               // Transition head list
  ErrCond i_addTransition( State, State, const char*, ActionFunc);
public:
  /// Default constructor
  FSM();
  /// Initializing constructor
  explicit FSM(Transition**);
  /// Standard destructor
  virtual ~FSM() {}
  /// Head of transition set
  Transition* transitionHead()  const {  return m_head; }
  /// Pointer to current transition
  Transition* currentTransition() const  { return m_currentTransition; }
  /// Access current state of the FSM machine
  State currentState() { return m_currentState; }
  /// Access previous state (during executing transition)
  State previousState(){ return m_previousState; }
  /// Clean up Micro FSM: kill all transition objects
  void removeTransitions();
  /// Add new transition to FSM machine
  ErrCond addTransition( Transition* );
  /// Add empty transition between 2 states
  ErrCond addTransition(State from, State to, const char* nam) {
    return i_addTransition(from, to, nam, (ActionFunc)0);
  }
  /// Add tranition with explicit member funtion to be called
  template <class T> ErrCond addTransition(State from, State to, const char* nam, ErrCond (T::*pmf)()) {
    return i_addTransition(from, to, nam, (ActionFunc)pmf);
  }
  /// Remove transition between 2 states
  ErrCond removeTransition( State, State);
  /// Invoke transition 
  ErrCond invokeTransition( State );
  /// Virtual overload: post action
  virtual ErrCond postAction() { return FSM_K_SUCCESS;}
  /// Virtual overload: pre action
  virtual ErrCond preAction()  { return FSM_K_SUCCESS;}
};
#endif // CPP_FSM_H
