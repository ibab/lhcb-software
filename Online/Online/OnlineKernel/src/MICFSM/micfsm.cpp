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
#include <MICFSM/micfsm.h>

struct InternalTransition {            // Transition type is an structure
  InternalTransition* next;            // Pointer to the next
  FSM::State       from;               // Transition starting state
  FSM::State       to;                 // Transition ending state
  char*            condition;          // Text string condition
  FSM::Action*     preaction;          // Function pointer to be called
  FSM::Action*     action;             // Function pointer to be called
  FSM::Action*     postaction;         // Function pointer to be called
};

// Standard constructor
FSM::FSM()  {
  m_currentState      = IDLE;
  m_previousState     = IDLE;
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
FSM::ErrCond FSM::addTransition(State from, State to, 
                              const char* condition, 
                              Action* preaction,
                              Action* action,
                              Action* postaction)
{
  InternalTransition* tr   = new InternalTransition;
  char* cond = new char[strlen(condition)+1];
  tr->next       = 0;                    // Initialize the forward pointer
  tr->from       = from;                 // Set the from state
  tr->to         = to;                   // Set the to state
  tr->condition  = strcpy(cond, condition); // Copy the condit. string
  tr->preaction  = preaction;            // Set the action address
  tr->action     = action;               // Set the action address
  tr->postaction = postaction;           // Set the action address

  if( m_transitionHead )    {            // Check if the list is not empty
    InternalTransition* t;               // Find the last element
    for( t = m_transitionHead; t->next; t = t->next );
    t->next  = tr;                       // Add the new transition at the end 
    tr->next = 0;                        // Initialize the next pointer to zero 
  }
  else       {                           // If the list was empty 
    m_transitionHead = tr;               // Add the transition in the head 
  }
  return FSM::SUCCESS;                   // return always true
}

//----------------------------------------------------------------------------
FSM::ErrCond FSM::removeTransition(State from, State to)    {
  InternalTransition* tr = m_transitionHead;
  // Check if the headlist is the desired element
  if( tr->from == from && tr->to == to)   {
    m_transitionHead = 0;               // Delete the head of the list
    if ( tr->condition  ) delete [] tr->condition;
    if ( tr->preaction  ) tr->preaction->release();
    if ( tr->action     ) tr->action->release();
    if ( tr->postaction ) tr->postaction->release();
    delete tr;
    return FSM::SUCCESS;                // return true
  }
  else    {                             // If it is not the list head
    for( ;tr->next; tr= tr->next)   {   // scan list until last element
      // Check if it is the element we are looking for
      if( tr->next->from == from && tr->next->to == to )   {
        tr->next = tr->next->next;      // bypassed
        tr = tr->next;
        if ( tr->condition ) delete [] tr->condition;
        if ( tr->preaction  ) tr->preaction->release();
        if ( tr->action     ) tr->action->release();
        if ( tr->postaction ) tr->postaction->release();
        delete tr;
        return FSM::SUCCESS;            // return success
      }
    }    
  }
  return FSM::TRANNOTFOUND;             // return false (not found)
}

//----------------------------------------------------------------------------
FSM::ErrCond FSM::invokeTransition(State target, void* user_param)   {
  // scan the whole transition list 
  for(InternalTransition* tr = m_transitionHead; tr; tr = tr->next)  {
    // Check if a state matches the current and target states 
    if( tr->from == m_currentState && tr->to == target )  {
      m_currentTransition = tr;              // Set the current transition 
      if( tr->preaction )        {           // Check if pre-action routine exists 
        ErrCond status = tr->preaction->execute(user_param);
        if( status != FSM::SUCCESS )   {      // Check for success 
          return FSM::BADPREACTION;           // Return with the bad preaction code
        }
      }
      if( tr->action )        {              // Check if action routine exists 
        ErrCond status = tr->action->execute(user_param);
        if( status != FSM::SUCCESS )   {     // Check for success 
          return status;                     // Return the same status 
        }
      }
      // Now do the transition (if action routine returns SUCCESS)
      m_previousState = m_currentState;
      m_currentState  = target;

      if( tr->postaction )        {          // Check if pre-action routine exists 
        ErrCond status = tr->postaction->execute(user_param);
        if( status != FSM::SUCCESS )   {     // Check for success 
          return FSM::BADPOSTACTION;         // Return with the bad preaction code
        }
      }
      return FSM::SUCCESS;                   // return success
    }
  }
  return FSM::TRANNOTFOUND;                  // return false (not found)
}
