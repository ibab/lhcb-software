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
#ifndef ONLINE_CONTROLLER_DIMSLAVE_H
#define ONLINE_CONTROLLER_DIMSLAVE_H

// Framework include files
#include "FiniteStateMachine/Slave.h"

// C/C++ include files
#include <vector>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine   {

  /**@class DimSlave  DimSlave.h FiniteStateMachine/DimSlave.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class DimSlave : public Slave   {
    typedef std::pair<void*,unsigned long> TimerID;
    typedef std::map<const void*,int> TimeoutTable; 
  protected:

    /// Timeout table for various transitions
    TimeoutTable              m_timeouts;
    /// ID of the DIM command to send commands to the slave
    std::pair<int,int>        m_dimState;
    /// General transition timeout, if no other is specified.
    int                       m_tmo;
    /// Process identifier of the created (alive) slave process
    int                       m_pid;
    /// ID of timeout timer
    TimerID                   m_timerID;
    /// Name of the DIM command to receive the slave's state information
    std::string               m_commandName;
    /// String with start command
    std::string               m_cmd;
    /// Pointer array with process arguments (to be set by the sub-class)
    std::vector<std::string>  m_argv;
    /// Pointer array with environment variables (to be set by the sub-class)
    std::vector<std::string>  m_envp;

  public:
    enum  Slave_Timeouts {
      SLAVE_UNLOAD_TIMEOUT    = 2001,
      SLAVE_TERMINATE_TIMEOUT = 2002,
      SLAVE_KILL_TIMEOUT      = 2003
    };
    struct FSMMonitoring  {
      unsigned long lastCmd, doneCmd;
      int pid;
      char targetState, state, metaState, pad;
      int partitionID;
    } m_monitor;
  public:
    /** Class Constructor
     *
     * @arg nam     [string, read-only] Slave name
     * @arg typ     [string, read-only] FSM type
     * @arg machine [string, read-only] FSM machine reference
     */
    DimSlave(const Type* typ, const std::string& nam, Machine* machine);
    /// Standatrd destructor
    virtual ~DimSlave();
    /// Set the command line value
    void setCommand(const std::string& cmd)  {  m_cmd = cmd;  }
    /// Access the command string 
    const std::string& command() const       {  return m_cmd; }
    /// Start slave process
    virtual ErrCond start();
    /// Kill slave process
    virtual ErrCond kill();
    /// Send transition request to the slave
    virtual ErrCond sendRequest(const Transition* tr);

    /// Handle timeout according to timer ID
    void handleTimeout();
    /// Handle state updates for a particular slave
    void handleState(const std::string& msg);

    /// Add entry in transition timeout table (timeout in seconds)
    DimSlave& addTimeout(const Transition* param, int tmp);
    /// Remove entry in transition timeout table
    DimSlave& removeTimeout(const Transition* param);
    /// Start the slave's transition timeout
    DimSlave& startTimer(int reason, const void* param=0);
    /// Stop the slave's transition timeout
    DimSlave& stopTimer();
    /// Clone the environment of the existing process
    DimSlave& cloneEnv();

    /// DTQ overload to process timeout(s)
    static void tmoHandler(void* tag);
    /// DimInfo overload to process messages
    static void infoHandler(void* tag, void* address, int* size);
    /// DimInfo overload to process messages
    static void stateHandler(void* tag, void* address, int* size);
  };   //  End class DimSlave
}      //  End namespace 
#endif //  ONLINE_CONTROLLER_DIMSLAVE_H

