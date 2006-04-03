#ifndef ONLINEKERNEL_GAUDITASK_H
#define ONLINEKERNEL_GAUDITASK_H

// Framework includes
#include "GaudiOnline/GaudiDeamon.h"

/*
 * LHCb namespace declaration
 */
namespace LHCb  {
  /** @class GaudiTask GaudiTask.h GaudiOnline/GaudiTask.h
    *
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class GaudiTask : public GaudiDeamon  {
  protected:
    /// Sub application manager object
    IAppMgrUI*  m_subMgr;
    /// Property: optional options
    std::string m_optOptions;
  public:
    /// Standard constructor
    GaudiTask(IInterface* svc);
    /// Default destructor
    virtual ~GaudiTask();
    /// Set properties of application manager instance
    StatusCode setInstanceProperties(IAppMgrUI* inst);
    /// Callback on I/O cancel
    StatusCode cancel();
    /// Callback on configure transition
    StatusCode configure();
    /// Callback on start transition
    StatusCode initialize();
    /// Callback to process event
    StatusCode nextEvent(int num_event);
    /// Callback on stop transition
    StatusCode finalize();
    /// Callback on reset transition
    StatusCode terminate();
  };
}
#endif // ONLINEKERNEL_GAUDITASK_H
