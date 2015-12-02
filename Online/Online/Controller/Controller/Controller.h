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
    Machine*     m_machine;
    /// Service name to retrieve number of instances
    std::string  m_tagSvcName;
    /// Variable to publish the task information
    std::string  m_taskInfo;
    /// Variable to publish the instance information
    std::string  m_tagInfo;
    /// Pointer to the dim service publishing the task information
    int          m_fsmTasks;
    /// Pointer to the dim service publishing the instance information
    int          m_fsmTags;
    /// Pointer to dim client with maximum instance tag
    int          m_numTasks;
    /// Maximum instance tag
    int          m_maxInstanceTag;

  public:
    /// Constructor
    Controller(const std::string& nam, const std::string& tag_svc, Machine* machine);
    /// Standard destructor
    virtual ~Controller();
    /// DimCommand overload: handle DIM commands
    void commandHandler();
    /// Interactor Interrupt handling routine
    virtual void handle(const CPP::Event& ev);

    /// Publish state information when transition failed. 
    virtual ErrCond fail();
    /// Publish state information when transition is completed
    virtual ErrCond publish();
    /// Publish state information of the slaves
    virtual FSM::ErrCond publishSlaves();
    /// Transition pre-action for configure
    virtual ErrCond config();
    /// Transition pre-action for start: Reset all internal slaves to external ones
    virtual ErrCond start();
    /// Invoke single transition request on machine
    virtual ErrCond invokeTransition(const std::string& tr);
    /// Set transition target state
    virtual void setTargetState(int target);
    /// Adjust the number of slaves to be controlled (ignore overcounted slaves)
    void publishInstances(int max_tag);
    void controlInstances();
  };   //  End class Controller
}      //  End namespace 

#endif //  ONLINE_FINITESTATEMACHINE_CONTROLLER_H
