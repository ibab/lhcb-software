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
    GaudiDimFSM(IAppMgrUI* svc);
    virtual ~GaudiDimFSM();
    StatusCode cancel();
    StatusCode configure();
    StatusCode initialize();
    StatusCode nextEvent(int num_event);
    StatusCode finalize();
    StatusCode terminate();
  };
}
#endif // ONLINEKERNEL_GAUDIDIMFSM_H
