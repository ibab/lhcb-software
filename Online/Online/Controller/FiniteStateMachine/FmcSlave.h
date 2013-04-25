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
#include "FiniteStateMachine/DimSlave.h"

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine   {

  /**@class Fmcslave  Fmcslave.h FiniteStateMachine/Fmcslave.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class FmcSlave : public DimSlave   {
  public:
    std::string  fmc_args;
    std::string  cmd_args;
    bool         m_internal;
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
    /// Access flag indicating (initial) internal transition actions
    bool isInternal() const { return m_internal; }

    /// Start slave process
    virtual ErrCond start();
    /// Kill slave process
    virtual ErrCond kill();
    /// Send transition request to the slave
    virtual ErrCond sendRequest(const Transition* tr);
    /// Start slave process using FMC
    virtual ErrCond fmcStart();
  };   //  End class DimSlave
}      //  End namespace 
#endif //  ONLINE_CONTROLLER_FMCSLAVE_H
