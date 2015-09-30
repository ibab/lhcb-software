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
#ifndef ONLINE_CONTROLLER_FMCSLAVE_H
#define ONLINE_CONTROLLER_FMCSLAVE_H

// Framework include files
#include "Controller/DimSlave.h"

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine   {

  /**@class Fmcslave  Fmcslave.h Controller/Fmcslave.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class FmcSlave : public DimSlave   {
  public:
    /// Pointer array with process arguments (to be set by the sub-class)
    std::vector<std::string>  m_fmcArgs;
  public:
    /** Class Constructor
     *
     * @arg typ      [pointer,read-only] FSM type
     * @arg nam      [string, read-only] Slave name
     * @arg machine  [string, read-only] FSM machine reference
     * @arg internal [bool,   read-only] Flag indicating (initial) internal transition actions
     */
    FmcSlave(const Type* typ, const std::string& nam, Machine* machine, bool internal);
    /// Standatrd destructor
    virtual ~FmcSlave();
    /// Set the FMC arguments from single string
    DimSlave& setFmcArgs(const std::string& args);
    /// Add the FMC arguments from single string
    DimSlave& addFmcArgs(const std::string& args);
    /// Start slave process
    virtual ErrCond start();
    /// Kill slave process
    virtual ErrCond kill();
    /// Force slave killing
    virtual ErrCond forceKill();
    /// Handle timeout on unload transition according to timer ID
    virtual void handleUnloadTimeout();
  };   //  End class DimSlave
}      //  End namespace 
#endif //  ONLINE_CONTROLLER_FMCSLAVE_H
