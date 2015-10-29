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

  /**@class DimSlave  DimSlave.h Controller/DimSlave.h
   *
   * Basic implmentation of a slave process proxy, which uses DIM
   * for the inter process communication.
   *
   * Otherwise no OS dependent commands are used to fork and kill
   * the processes.
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class DimSlave : public Slave   {
  protected:
    /// ID of the DIM command to send commands to the slave
    std::pair<int,int>        m_dimState;
    /// Service IDs for auxiliary and debug services
    std::pair<int,int>        m_dbgState;
    /// Name of the DIM command to receive the slave's state information
    std::string               m_commandName;
    /// Name of the command to be sent as first action to shutdown slave
    std::string               m_killCmd;
    /// String with start command
    std::string               m_cmd;
    /// Pointer array with process arguments (to be set by the sub-class)
    std::vector<std::string>  m_argv;
    /// Pointer array with environment variables (to be set by the sub-class)
    std::vector<std::string>  m_envp;

    std::string               m_dbgData1, m_dbgData2;

  public:
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
    DimSlave(const Type* typ, const std::string& nam, Machine* machine, bool internal);
    /// Standatrd destructor
    virtual ~DimSlave();

    /// Set the command line value
    void setCommand(const std::string& cmd)  {  m_cmd = cmd;  }
    /// Access the command string 
    const std::string& command() const       {  return m_cmd; }

    /// Start slave process. Must be implemented by sub-class
    virtual ErrCond start() = 0;
    /// Inquire slave state. The reply may come later!
    virtual ErrCond inquireState();
    /// Kill slave process
    virtual ErrCond kill();
    /// Send transition request to the slave
    virtual ErrCond sendRequest(const Transition* tr);
    /// Publish debug information
    virtual void publishDebugInfo();
    /// Publish tag information
    virtual void publishTag(const std::string& tag);

    /// Start the slave's transition timeout
    DimSlave& startTimer(int reason, const void* param=0);
    /// Stop the slave's transition timeout
    DimSlave& stopTimer();

    /// Clone the environment of the existing process
    DimSlave& cloneEnv();
    /// Set the process arguments from single string
    DimSlave& setArgs(const std::string& args);
    /// Add entries to the process arguments from single string
    DimSlave& addArgs(const std::string& args);

    /// DTQ overload to process timeout(s)
    static void tmoHandler(void* tag);
    /// DimInfo overload to process messages
    static void infoHandler(void* tag, void* address, int* size);
    /// DimInfo overload to process messages
    static void stateHandler(void* tag, void* address, int* size);
    /// DimInfo overload to process auxiliary debug publishings
    static void dbgHandler1(void* tag, void** buff, int* size, int* first);
    /// DimInfo overload to process auxiliary debug publishings
    static void dbgHandler2(void* tag, void** buff, int* size, int* first);
  };   //  End class DimSlave
}      //  End namespace 
#endif //  ONLINE_CONTROLLER_DIMSLAVE_H

