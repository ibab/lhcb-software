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
#ifndef ONLINE_FINITESTATEMACHINE_FSM_H
#define ONLINE_FINITESTATEMACHINE_FSM_H

// C/C++ include files
#include <string>
#include "CPP/Callback.h"

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine   {

  // Forward declarations
  struct MicFSMTransition;


  /**@class FSM FSM.h FiniteStateMachine/FSM.h
   *
   * @version 0.1
   */
  class FSM {
  public:
    typedef unsigned int MicFSMState;            // State type is defined as unsigned int
    enum _ErrCond { 
      SUCCESS,
      FAIL,
      TRANNOTFOUND,
      BADACTION,
      WAIT_ACTION
    };
    typedef long ErrCond;
  protected:
    MicFSMState        m_currentState;           // Current state
    MicFSMState        m_previousState;          // Previous state
    MicFSMTransition*  m_currentTransition;      // Current transition
    MicFSMTransition*  m_transitionHead;         // Transition head list
  public:
    /// Standard constructor
    FSM();
    FSM(MicFSMState curr);
    /// Standard destructor
    virtual ~FSM();
    void setCurrentState(MicFSMState s)  { m_currentState = s;     }
    MicFSMState currentState()   const   { return m_currentState;  }
    MicFSMState previousState()  const   { return m_previousState; }
    ErrCond addTransition   (MicFSMState from, MicFSMState to,const std::string& condition,const Callback& act);
    ErrCond removeTransition(MicFSMState from, MicFSMState to);
    ErrCond invokeTransition(MicFSMState state, const void* param=0);
  };
}      //  End namespace 
#endif  // ONLINE_FINITESTATEMACHINE_FSM_H
