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
#ifndef ONLINE_FINITESTATEMACHINE_TASKMANAGER_H
#define ONLINE_FINITESTATEMACHINE_TASKMANAGER_H

// Framework include files

// C/C++ include files
#include <string>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine {

  /**@class TaskManager  TaskManager.h Ctrl/TaskManager.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class TaskManager   {
  protected:
    /// Node name
    std::string m_node;
    /// Name of the DIM command to start a process
    std::string m_start;
    /// Name of the DIM command to stop a process
    std::string m_stop;
    /// Name of the DIM command to kill a process
    std::string m_kill;
    
    /// Standard constructor
    TaskManager(const std::string& node);
    /// Standard destructor
    virtual ~TaskManager();
    /// Copy constructor
    TaskManager(const TaskManager& c);
    /// Assignment operator
    TaskManager& operator=(const TaskManager& c);
  public:
    /// Instance accessor
    static TaskManager& instance(const std::string& node);
    /// Start a process
    int start(const std::string& utgid, const std::string& fmc_args, const std::string& cmd, const std::string& args) const;
    /// Kill a process
    int stop(const std::string& utgid, int sig_num, int wait_before_kill)  const;
    /// Kill a process
    int kill(const std::string& utgid, int sig_num) const;
  };   //  End class State
}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_TASKMANAGER
