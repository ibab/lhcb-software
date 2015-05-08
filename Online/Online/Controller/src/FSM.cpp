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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <FiniteStateMachine/FSM.h>

using namespace std;
using namespace FiniteStateMachine;

// Standard constructor
FSM::FSM()  {
  m_targetState       = 0;
  m_currentState      = 0;
  m_previousState     = 0;
  m_currentTransition = 0;
  m_transitionHead    = 0;
}

// Standard constructor
FSM::FSM(MicFSMState curr)  {
  m_targetState       = curr;
  m_currentState      = curr;
  m_previousState     = curr;
  m_currentTransition = 0;
  m_transitionHead    = 0;
}

// Standard destructor
FSM::~FSM()  {
  while(m_transitionHead)  {
    removeTransition(m_transitionHead->m_from,m_transitionHead->m_to);
  }
}

/// Register callback to be executed on a metastate failure
void FSM::setFailureCallback(const Callback& action) {
  m_failCall = action;
}

/// Register callback to be executed if the current transition is not availible
void FSM::setNoTransitionCallback(const Callback& action) {
  m_noTransCall = action;
}

//----------------------------------------------------------------------------
FSM::ErrCond FSM::addTransition(MicFSMState from, MicFSMState to, 
				const string& condition, const Callback& action)
{
  MicFSMTransition* tr = new MicFSMTransition;
  tr->m_next       = 0;                    // Initialize the forward pointer
  tr->m_from       = from;                 // Set the from state
  tr->m_to         = to;                   // Set the to state
  tr->m_condition  = condition;            // Copy the condit. string
  tr->m_action     = action;               // Set the action address
  if( m_transitionHead )    {            // Check if the list is not empty
    MicFSMTransition* t;                 // Find the last element
    for( t = m_transitionHead; t->m_next; t = t->m_next );
    t->m_next  = tr;                       // Add the new transition at the end
  }
  else       {                           // If the list was empty 
    m_transitionHead = tr;               // Add the transition in the head 
  }
  return FSM::SUCCESS;                   // return always true
}

//----------------------------------------------------------------------------
FSM::ErrCond FSM::removeTransition(MicFSMState from, MicFSMState to)    {
  MicFSMTransition* tr = m_transitionHead;
  // Check if the headlist is the desired element
  if( tr->m_from == from && tr->m_to == to)   {
    m_transitionHead = 0;               // Delete the head of the list
    delete tr;
    return FSM::SUCCESS;                // return true
  }
  else    {                             // If it is not the list head
    for( ;tr->m_next; tr= tr->m_next)   {   // scan list until last element
      // Check if it is the element we are looking for
      if( tr->m_next->m_from == from && tr->m_next->m_to == to )   {
        tr->m_next = tr->m_next->m_next;      // bypassed
        tr = tr->m_next;
        delete tr;
        return FSM::SUCCESS;            // return success
      }
    }    
  }
  return FSM::TRANNOTFOUND;             // return false (not found)
}

/// Check if the transition from the current state to the target state exists
bool FSM::hasTransitionTo(MicFSMState target) const {
  for(MicFSMTransition* tr = m_transitionHead; tr; tr = tr->m_next)  {
    if( tr->m_from == m_currentState && tr->m_to == target )  {
      return true;
    }
  }
  return false;
}


//#include "RTL/rtl.h"
//----------------------------------------------------------------------------
FSM::ErrCond FSM::invokeTransition(MicFSMState target, const void* user_param)   {
  for(MicFSMTransition* tr = m_transitionHead; tr; tr = tr->m_next)  {
    if( tr->m_from == m_currentState && tr->m_to == target )  {
      m_targetState       = target;
      m_currentTransition = tr;              // Set the current transition 
      if( tr->m_action )        {              // Check if action routine exists 
        ErrCond status = ErrCond(tr->m_action.execute(user_param));
        if( status != FSM::SUCCESS )   {     // Check for success 
	  m_failCall.execute(user_param);
          return status;                     // Return the same status 
        }
      }
      // Now do the transition (if action routine returns SUCCESS)
      m_previousState = tr->m_from;
      m_currentState  = tr->m_to;
      return FSM::SUCCESS;                   // return success
    }
  }
  m_noTransCall.execute(user_param);
  return FSM::TRANNOTFOUND;                  // return false (not found)
}

