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

  typedef unsigned int State;		  // State type is defined as unsigned int
  typedef struct _transition {		// Transition type is an structure
    _transition(State from, State to, const char* cond, ActionFunc act);
    ~_transition();
    struct _transition* next;		  // Pointer to the next
    State       from;			        // Transition starting state 
    State       to;			          // Transition ending state
    char*       condition;		    // Text string condition
    ActionFunc  action;			      // Function porinter to be call 
  } Transition;

protected:
  State        m_currentState;		  // Current state of the object
  State        m_previousState;		  // Previous state
  Transition*  m_currentTransition;	// Current transition
  Transition*  m_head;		          // Transition head list
  ErrCond i_addTransition( State, State, const char*, ActionFunc);
public:
  FSM();
  FSM(Transition**);
  /// Standard destructor
  virtual ~FSM() {}
  /// Head of transition set
  Transition* transitionHead()  const {  return m_head; }
  /// Pointer to current transition
  Transition* currentTransition() const  { return m_currentTransition; }
  State currentState() { return m_currentState; }
  State previousState(){ return m_previousState; }
  /// Clean up Micro FSM: kill all transition objects
  void removeTransitions();
  ErrCond addTransition( Transition* );
  ErrCond addTransition(State from, State to, const char* nam) {
    return i_addTransition(from, to, nam, (ActionFunc)0);
  }
  template <class T> ErrCond addTransition(State from, State to, const char* nam, ErrCond (T::*pmf)()) {
    return i_addTransition(from, to, nam, (ActionFunc)pmf);
  }
  ErrCond removeTransition( State, State);
  ErrCond invokeTransition( State );
  virtual ErrCond postAction() { return FSM_K_SUCCESS;}
  virtual ErrCond preAction()  { return FSM_K_SUCCESS;}
};
#endif // CPP_FSM_H
