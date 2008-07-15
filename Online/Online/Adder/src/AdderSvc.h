#ifndef AdderSvc_AdderSvc_H
#define AdderSvc_AdderSvc_H 1

// Include files
#include "CPP/Interactor.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"

class IIncidentSvc;
class ProcessManager;
namespace LHCb {

  class AdderSvc : virtual public IIncidentListener, public Service, public Interactor  {
  public:
    /// Constructor of this form must be provided
    AdderSvc(const std::string& name, ISvcLocator* pSvcLocator); 
    /// Default destructor
    virtual ~AdderSvc();
    /// Iservice overload: Initialize service
    StatusCode initialize();
    /// Iservice overload: Finalize service
    StatusCode finalize();
    /// Call to save the histograms
    StatusCode save();
    /// Incident handler callback
    virtual void handle(const Incident& inc);
    /// Interactor handler
    virtual void handle(const Event& ev);

private:
  /// Reference to incident service
  IIncidentSvc* m_incidentSvc;
    
  ProcessMgr *m_processMgr;

  std::string m_utgid;
  std::string m_nodeName;
  
  std::vector<std::string> m_taskName;
  std::vector<std::string> m_algorithmName;
  std::vector<std::string> m_objectName;
  
  std::string m_dimClientDns;
  int m_refreshTime;
    
  };
}  //end namespace LHCb  

#endif    // AdderSvc_AdderSvc_H
