/*
**      Small class to implement a Finite State Machine
**
**  AUTHORS:
**
**      P. Mato  &  J.A. Perlas
**
**  CREATION DATE:  18-SEP-1991
**
**============================================================
** #  Date     Changes made                                 By
** -- -------- ------------------------------------------- ---
** 01 18/09/91 First version based on DEXPERT code         JAP
** 02 19/03/92 Changed to pass a param in action routines  JAP
** 03 20/10/06 Major rewrite and switch to C++             MSF
*/
#ifndef ONLINEKERNEL_MICFSM_H
#define ONLINEKERNEL_MICFSM_H

struct InternalTransition;
class FSM {
public:
  typedef unsigned int State;            // State type is defined as unsigned int
  static const State IDLE = 0;
  typedef enum { 
    SUCCESS,
    FAIL,
    TRANNOTFOUND,
    BADPREACTION,
    BADPOSTACTION
  } ErrCond;

  class Action   {                       // Transition type is an structure
  public:
    virtual FSM::ErrCond execute(void* param) = 0;
    virtual unsigned int release() = 0;
  };

protected:
  State                m_currentState;           // Current state
  State                m_previousState;          // Previous state
  InternalTransition*  m_currentTransition;      // Current transition
  InternalTransition*  m_transitionHead;         // Transition head list
public:
  /// Standard constructor
  FSM();
  /// Standard destructor
  virtual ~FSM();
  State currentState()    { return m_currentState;  }
  State previousState()   { return m_previousState; }
  ErrCond addTransition   (State from, State to,const char* condition,Action* pre=0,Action* act=0,Action* post=0);
  ErrCond removeTransition(State from, State to);
  ErrCond invokeTransition(State,void* param);
};

#endif  // ONLINEKERNEL_MICFSM_H
