#ifndef ONLINEKERNEL_GAUDIDIMFSM_H
#define ONLINEKERNEL_GAUDIDIMFSM_H

// Framework includes
#include "GaudiOnline/DimTaskFSM.h"
// Forward declarations
class IAppMgrUI;


/*
 * LHCb namespace declaration
 */
namespace LHCb  {
  /** @class GaudiDimFSM GaudiDimFSM.h GaudiOnline/GaudiDimFSM.h
    *
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class GaudiDimFSM : public DimTaskFSM  {
  protected:
    IAppMgrUI*  m_appMgr;
  public:
    /// Initializing constructor                       (OFFLINE     -> Inactive)
    GaudiDimFSM(IAppMgrUI* svc);
    /// Standard destructor
    virtual ~GaudiDimFSM();
    /// Cancel IO request
    StatusCode cancel();
    /// Configure application                           (Inactive   -> Configured)
    StatusCode configure();
    /// Initialize the application                      (Configured -> Ready)
    StatusCode initialize();
    /// Start the application
    virtual StatusCode start();
    /// Process single event                            (Running    -> Running)
    StatusCode nextEvent(int num_event);
    /// Stop the application
    virtual StatusCode stop();
    /// Finialize the application                       (Ready      -> Finalized)
    StatusCode finalize();
    /// Terminate application, but do not exit process  (Finalized  -> Inactive)
    StatusCode terminate();
  };
}
#endif // ONLINEKERNEL_GAUDIDIMFSM_H
