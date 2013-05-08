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
#ifndef ONLINE_FINITESTATEMACHINE_FSMTYPES_H
#define ONLINE_FINITESTATEMACHINE_FSMTYPES_H

// Framework include files
#include "FiniteStateMachine/Transition.h"

// C/C++ include files
#include <string>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine   {

  // Forward declarations
  class Type;

  enum TransitionFlags {
    NO_CHECKS     = Transition::NO_CHECKS,
    CREATE        = Transition::CREATE,
    CHECK         = Transition::CHECK,
    KILL          = Transition::KILL
  };
  namespace DAQ {
    const char ST_NAME_UNKNOWN[]      = {"UNKNOWN"};
    const char ST_NAME_OFFLINE[]      = {"OFFLINE"};
    const char ST_NAME_NOT_READY[]    = {"NOT_READY"};
    const char ST_NAME_READY[]        = {"READY"};
    const char ST_NAME_RUNNING[]      = {"RUNNING"};
    const char ST_NAME_STOPPED[]      = {"READY"};
    const char ST_NAME_ERROR[]        = {"ERROR"};
    const char ST_NAME_PAUSED[]       = {"PAUSED"};

    enum StateChar  {
      UNKNOWN   = 'U',
      OFFLINE   = 'O',
      NOT_READY = 'N',
      READY     = 'r',
      RUNNING   = 'R',
      STOPPED   = 'S',
      PAUSED    = 'P',
      ERROR     = 'E',
    };
  }

  /// Inventory of known FSM types
  Type* fsm_type(const std::string& typ);

}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_FSMTYPES_H

