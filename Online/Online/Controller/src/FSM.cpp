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
#include <iostream>
#include <FiniteStateMachine/FSM.h>
#include "FiniteStateMachine/TypedObject.h"

using namespace std;
using namespace FiniteStateMachine;

namespace FiniteStateMachine  {
  struct MicFSMTransition {                // Transition type is an structure
    MicFSMTransition*  next;               // Pointer to the next
    FSM::MicFSMState   from;               // Transition starting state
    FSM::MicFSMState   to;                 // Transition ending state
    string             condition;          // Text string condition
    Callback           action;             // Function pointer to be called
  };
}

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
    removeTransition(m_transitionHead->from,m_transitionHead->to);
  }
}

//----------------------------------------------------------------------------
FSM::ErrCond FSM::addTransition(MicFSMState from, MicFSMState to, 
				const string& condition, const Callback& action)
{
  MicFSMTransition* tr = new MicFSMTransition;
  tr->next       = 0;                    // Initialize the forward pointer
  tr->from       = from;                 // Set the from state
  tr->to         = to;                   // Set the to state
  tr->condition  = condition;            // Copy the condit. string
  tr->action     = action;               // Set the action address
  if( m_transitionHead )    {            // Check if the list is not empty
    MicFSMTransition* t;                 // Find the last element
    for( t = m_transitionHead; t->next; t = t->next );
    t->next  = tr;                       // Add the new transition at the end 
    tr->next = 0;                        // Initialize the next pointer to zero 
  }
  else       {                           // If the list was empty 
    m_transitionHead = tr;               // Add the transition in the head 
  }
  return FSM::SUCCESS;                   // return always true
}
#include "RTL/rtl.h"

//----------------------------------------------------------------------------
FSM::ErrCond FSM::removeTransition(MicFSMState from, MicFSMState to)    {
  MicFSMTransition* tr = m_transitionHead;
  // Check if the headlist is the desired element
  if( tr->from == from && tr->to == to)   {
    m_transitionHead = 0;               // Delete the head of the list
    delete tr;
    return FSM::SUCCESS;                // return true
  }
  else    {                             // If it is not the list head
    for( ;tr->next; tr= tr->next)   {   // scan list until last element
      // Check if it is the element we are looking for
      if( tr->next->from == from && tr->next->to == to )   {
        tr->next = tr->next->next;      // bypassed
        tr = tr->next;
        delete tr;
        return FSM::SUCCESS;            // return success
      }
    }    
  }
  return FSM::TRANNOTFOUND;             // return false (not found)
}

//----------------------------------------------------------------------------
FSM::ErrCond FSM::invokeTransition(MicFSMState target, const void* user_param)   {
#if 0
  TypedObject::display(TypedObject::ALWAYS,"FSM[%s]> Invoke transition from state:%d to state %d",
		       RTL::processName().c_str(),m_currentState,int(target));
#endif
  for(MicFSMTransition* tr = m_transitionHead; tr; tr = tr->next)  {
    if( tr->from == m_currentState && tr->to == target )  {
      m_targetState = target;
      m_currentTransition = tr;              // Set the current transition 
      //std::cout << "Transition from state:" << m_currentState << " to state: " << int(target) << std::endl;
      if( tr->action )        {              // Check if action routine exists 
        ErrCond status = ErrCond(tr->action.execute(user_param));
        if( status != FSM::SUCCESS )   {     // Check for success 
	  std::cout << "Failed     action: " << int(tr->from) << " --> " << int(tr->to) << std::endl;
          return status;                     // Return the same status 
        }
      }
      // Now do the transition (if action routine returns SUCCESS)
      m_previousState = m_currentState;
      m_currentState  = target;
      return FSM::SUCCESS;                   // return success
    }
  }
  TypedObject::display(TypedObject::ALWAYS,"FSM[%s]> Unknown transition from state:%d to state %d",
		       RTL::processName().c_str(),m_currentState,int(target));
  return FSM::TRANNOTFOUND;                  // return false (not found)
}
