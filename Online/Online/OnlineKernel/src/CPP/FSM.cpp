/*
**++
**  FACILITY:  DEXPERT
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


/*
**
**  INCLUDE FILES
**
*/

#include "CPP/FSM.h"
#include <cstring>

//-----------------------------------------------------------------------------
FSM::Transition::Transition(State f, State t, const char* cond, ActionFunc act)  {
  next = 0;         // Initialize the forward pointer
  from = f;         // Set the from state
  to = t;           // Set the to state
  action = act;     // Set the action address
  condition = cond ? strcpy(new char[strlen(cond)+1],cond) : 0; // Copy the condition string
}
//-----------------------------------------------------------------------------
FSM::Transition::~Transition()  {
  if ( condition ) delete [] condition;
}
//-----------------------------------------------------------------------------
FSM::FSM()    {
  m_currentState      = IDLE_STATE;
  m_previousState     = IDLE_STATE;
  m_currentTransition = 0;
  m_head    = 0;
}

//-----------------------------------------------------------------------------
FSM::FSM( Transition** tarray)  {
  m_currentState      = IDLE_STATE;
  m_previousState     = IDLE_STATE;
  m_currentTransition = 0;
  m_head              = 0;
  for (Transition* tr = *tarray; tr; tr = *(++tarray))  {
    addTransition( tr );
  }  
}  
//-----------------------------------------------------------------------------
void FSM::removeTransitions() {
  Transition *tn, *tr = transitionHead();        // o Clean up Micro FSM
  while ( tr ) {                                 //
    tn = tr->next;                               //
    delete tr;                                   //
    tr = tn;                                     //
  }                                              //
}
//-----------------------------------------------------------------------------
FSM::ErrCond FSM::addTransition( Transition* tr ) {
  Transition* t;
  if (m_head )      {                     // Check is the list is not empty
    for( t=m_head; t->next; t=t->next){}  // Find the last element
    t->next  = tr;                        // add the new transition at the end
    tr->next = 0;                         // Initialize the next pointer to zero
  }                                       //
  else            {                       // If the list was empty
    m_head = tr;                          // add the transition in the head
  }                                       //
  return FSM_K_SUCCESS;                   // Return always success
}

//-----------------------------------------------------------------------------
FSM::ErrCond FSM::i_addTransition( State from,State to, const char* condition, ActionFunc action) {
  // Create a Transition structure
  Transition* tr   = new Transition(from,to,condition,action);
  addTransition( tr );                    // add the transition into the list
  return FSM_K_SUCCESS;                   // return always true
}
//-----------------------------------------------------------------------------
FSM::ErrCond FSM::removeTransition(State from, State to) {
  Transition* tr = m_head;
  if( tr->from == from && tr->to == to)  {// Check if the headlist is the desired element
    m_head = tr->next;                    // Delete the head of the list
    return FSM_K_SUCCESS;                 // return true
  }
  else  {                                 // If it is not the list head
    for( ;tr->next; tr= tr->next) {       // scan list until last element
      if( tr->next->from == from && tr->next->to == to ) { // Check if it is the right element
        tr->next = tr->next->next;        // bypassed
        return FSM_K_SUCCESS;             // return success
      }
    }    
  }
  return FSM_K_TRANNOTFOUND;              // return false (not found)
}

//-----------------------------------------------------------------------------
FSM::ErrCond FSM::invokeTransition( State target) {
  for(Transition* tr = m_head; tr; tr = tr->next)   { // Scan the whole transition list
    if( tr->from == currentState() && tr->to == target )  {
      m_currentTransition = tr;           // Set the current transition
      ErrCond status = preAction();       // Execute Preaction
      if( status != FSM_K_SUCCESS)      { // Compare if status is successful
        return FSM_K_BADPREACTION;        // Return with the bad preaction code
      }
      if( tr->action )      {             // Check is action routine exists
        status = tr->action ? (this->*tr->action)() : FSM_K_SUCCESS;
        if( status != FSM_K_SUCCESS )  {  // Check for success
          return status;                  // Return the same bad status
        }
      }
      m_previousState = currentState();   // Do the transition
      m_currentState  = target;           // idem
      status = postAction();              // Execute Preaction
      if( status != FSM_K_SUCCESS)  {     // Compare if status is successful
        return FSM_K_BADPOSTACTION;       // Return with the bad preaction code
      }
      return FSM_K_SUCCESS;               // return success
    }
  }  
  return FSM_K_TRANNOTFOUND;              // return false (not found)
}
