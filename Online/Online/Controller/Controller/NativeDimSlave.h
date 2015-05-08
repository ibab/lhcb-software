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
#ifndef ONLINE_CONTROLLER_NATIVEDIMSLAVE_H
#define ONLINE_CONTROLLER_NATIVEDIMSLAVE_H

// Framework include files
#include "Controller/DimSlave.h"

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine   {

  /**@class NativeDimSlave  NativeDimSlave.h Controller/NativeDimSlave.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class NativeDimSlave : public DimSlave   {
  protected:
    /// Process identifier of the created (alive) slave process
    int                       m_pid;

  public:
    /** Class Constructor
     *
     * @arg nam     [string, read-only] Slave name
     * @arg typ     [string, read-only] FSM type
     * @arg machine [string, read-only] FSM machine reference
     */
    NativeDimSlave(const Type* typ, const std::string& nam, Machine* machine, bool internal);
    /// Standatrd destructor
    virtual ~NativeDimSlave();
    /// Start slave process
    virtual ErrCond start();
    /// Force slave killing
    virtual ErrCond forceKill();
    /// Handle timeout on unload transition according to timer ID
    virtual void handleUnloadTimeout();
    /// Handle state updates for a particular slave
    virtual void handleState(const std::string& msg);
  };   //  End class NativeDimSlave
}      //  End namespace 
#endif //  ONLINE_CONTROLLER_NATIVEDIMSLAVE_H
