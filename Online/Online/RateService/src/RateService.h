#ifndef RateService_RateService_H
#define RateService_RateService_H 1

// Include files
#include "CPP/Interactor.h"
#include "GaudiKernel/Service.h"
#include "Gaucho/IGauchoMonitorSvc.h"

class ProcessManager;
namespace LHCb {

  class RateService : virtual public Service, public Interactor  {
  public:
    /// Constructor of this form must be provided
    RateService(const std::string& name, ISvcLocator* pSvcLocator); 
    /// Default destructor
    virtual ~RateService();
    /// Iservice overload: Initialize service
    StatusCode initialize();
    /// Iservice overload: Finalize service
    StatusCode finalize();
    /// Interactor handler
    virtual void handle(const Event& ev);

private:
  /// Reference to monitoring service
  //IMonitorSvc*  m_monitorSvc;
  IGauchoMonitorSvc* m_pGauchoMonitorSvc; ///< Online Gaucho Monitoring Service
  
  ProcessMgr* m_processMgr;
  
  std::string m_utgid;
  std::string m_nodeName;
  std::string m_partName;
  std::string m_taskName;
  int m_nbCounterInMonRate;
  std::vector<std::string> m_objectName;
  
  std::string m_dimClientDns;
  int m_refreshTime;
  bool m_enablePostEvents;
    
  };
}  //end namespace LHCb  

#endif    // RateService_RateService_H
