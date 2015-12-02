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
#ifndef ONLINE_CONTROLLER_COMMANDTARGET_H
#define ONLINE_CONTROLLER_COMMANDTARGET_H

// Framework include files
#include "FiniteStateMachine/Type.h"
#include "FiniteStateMachine/FSM.h"
#include "Controller/DimSlave.h"
#include "CPP/Interactor.h"
#include "dis.hxx"

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine {

  // Forward declarations
  class Transition;
  class Machine;
  class State;

  /**@class CommandTarget  CommandTarget.h Controller/CommandTarget.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class CommandTarget : public DimCommand, public CPP::Interactor, public TypedObject  {
  public:
    typedef FSM::ErrCond ErrCond;

  protected:
    DimSlave::FSMMonitoring m_monitor;
    /// Variable to contain the state name
    std::string   m_stateName;
    /// Variable to contain the previous state name
    std::string   m_prevStateName;
    /// Pointer to the dim service publishing the state
    int           m_service;
    /// Pointer to the dim service publishing the sub-state
    int           m_fsmService;

  public:
    enum SubState  {
      SUCCESS_ACTION = 'S',
      EXEC_ACTION    = 'E',
      FAILED_ACTION  = 'F',
      UNKNOWN_ACTION = 'U'
    };

    /// Constructor
    CommandTarget(const std::string& nam);

    /// Standard destructor
    virtual ~CommandTarget();

    /// Currently declared state
    const std::string& state() const {  return m_stateName; }

    /// Declare process state to DIM service
    ErrCond declareState(const std::string& new_state, const std::string& opt="");

    /// Declare FSM sub-state
    ErrCond declareSubState(SubState new_state);

    /// Publish state information when transition is completed
    virtual ErrCond publish();

    /// ITaskFSM overload: Set transition target state
    virtual void setTargetState(int target) = 0;

    /// DimCommand overload: handle DIM commands
    virtual void commandHandler() = 0;

    /// Interrupt handling routine
    virtual void handle(const CPP::Event& ev) = 0;

    /// Run the object
    CommandTarget& run();

  };   //  End class State


  /**@class DAQCommandTarget  CommandTarget.h Controller/CommandTarget.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class DAQCommandTarget : public CommandTarget  {
    /// Flag to indicate that next transition will timeout
    bool m_timeout;
  public:
    /// Constructor
    DAQCommandTarget(const std::string& nam);

    /// Standard destructor
    virtual ~DAQCommandTarget();

    /// Set transition target state
    virtual void setTargetState(int target);

    /// DimCommand overload: handle DIM commands
    virtual void commandHandler();

    /// Interrupt handling routine
    virtual void handle(const CPP::Event& ev);
  };   //  End class State

}      //  End namespace 
#endif //  ONLINE_CONTROLLERxo_COMMANDTARGET_H

