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
#ifndef ONLINE_CONTROLLER_CONTROLLER_H
#define ONLINE_CONTROLLER_CONTROLLER_H

// Framework include files
#include "Controller/CommandTarget.h"

// C/C++ include files
#include <string>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine   {

  // Forward declarations
  class Type;

  /**@class Controller  Controller.h FiniteStateMachine/Controller.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class Controller : public CommandTarget   {
  public:
    typedef FSM::ErrCond ErrCond;
  protected:
    enum { EXIT_PROCESS=99998,
	   ERROR_PROCESS=99999
    };

    /// Static cache of the pointer to the error state
    const State* m_errorState;
    /// Reference to machine object
    Machine* m_machine;

  public:
    /// Constructor
    Controller(const std::string& nam, Machine* machine);
    /// Standard destructor
    virtual ~Controller();
    /// DimCommand overload: handle DIM commands
    void commandHandler();
    /// Interactor Interrupt handling routine
    virtual void handle(const Event& ev);

    /// Publish state information when transition failed. 
    ErrCond fail();
    /// Publish state information when transition is completed
    ErrCond publish();
    /// State enter action for READY: Reset all internal slaves to external ones
    ErrCond ready();
    /// Invoke single transition request on machine
    ErrCond invokeTransition(const std::string& tr);
    /// Set transition target state
    virtual void setTargetState(int target);
  };   //  End class Controller
}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_CONTROLLER_H

