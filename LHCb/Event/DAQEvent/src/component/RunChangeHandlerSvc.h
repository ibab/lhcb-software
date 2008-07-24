// $Id: RunChangeHandlerSvc.h,v 1.1 2008-07-24 19:22:09 marcocle Exp $
#ifndef _RUNCHANGEHANDLERSVC_H_
#define _RUNCHANGEHANDLERSVC_H_

#include "GaudiKernel/Service.h"
#include "GaudiKernel/Map.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/SmartDataPtr.h"

#include <list>

/** @class RunChangeHandlerSvc RunChangeHandlerSvc.h
 *  
 *  Service intercepting "RunChange" incidents to replace the path of the XML
 *  files according to the new run number.
 *
 *  @author Marco Clemencic
 *  @date   2008-07-24
 */
class RunChangeHandlerSvc:
  public Service,
  public virtual IIncidentListener {

public:
  
  /// Standard constructor
  RunChangeHandlerSvc(const std::string& name, ISvcLocator* svcloc); 

  virtual ~RunChangeHandlerSvc(); ///< Destructor

  /** Query interfaces (\see{IInterface})
      @param riid       ID of Interface to be retrieved
      @param ppvUnknown Pointer to Location for interface pointer
  */
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

  /// Initialize Service
  virtual StatusCode initialize();
  
  /// Finalize Service
  virtual StatusCode finalize();
  
  // ---- Implement IIncidentListener interface ----
  /// Handle RunChange incident.
  virtual void handle(const Incident &inc);
  
private:

  /// Helper function to retrieve a service and cache the pointer to it.
  template <class I>
  inline I * getService(const std::string &name, I *&ptr) const {
    if (0 == ptr) {
      StatusCode sc = service(name, ptr, true);
      if( sc.isFailure() ) {
        throw GaudiException("Service ["+name+"] not found", this->name(), sc);
      }
    }
    return ptr;
  }
  
  /// Helper function to release acquired interfaces.
  template <class I>
  inline void release(I *&ptr) const {
    if (ptr) {
      ptr->release();
      ptr = 0;
    }
  }
  
  /// Get pointer to the event data service.
  inline IDataProviderSvc *eventSvc() const {
    return getService("EventDataSvc",m_evtSvc);
  }
  
  /// Get pointer to the detector data service.
  inline IDataProviderSvc *detectorSvc() const {
    return getService("DetectorDataSvc",m_detSvc);
  }
  
  /// Get pointer to the incident service.
  inline IIncidentSvc *incidentSvc() const {
    return getService("IncidentSvc",m_incSvc);
  }
  
  /// Get pointer to the detector data service.
  inline IUpdateManagerSvc *updMgrSvc() const {
    return getService("UpdateManagerSvc",m_ums);
  }
  
  /// Get pointer to the detector data service.
  inline IEventProcessor *evtProc() const {
    return getService("ApplicationMgr",m_evtProc);
  }

  /// Class to simplify handling of the objects to modify.
  struct CondData {
    CondData(IDataProviderSvc* pService,
        const std::string& fullPath,
        const std::string &pathTempl):
          object(pService, fullPath),
          pathTemplate(pathTempl) {}
    SmartDataPtr<ValidDataObject> object;
    std::string pathTemplate;
  };
  
  /// Modify the object opaque address.
  void update(CondData &cond);
  
  typedef GaudiUtils::Map<std::string,std::string> CondDescMap;
  CondDescMap m_condDesc;
  
  typedef std::list<CondData> Conditions;
  /// List of objects to modify 
  Conditions m_conditions;
  
  /// Current run number.
  unsigned int m_currentRun;
  
  /// EventDataSvc, for ODIN
  mutable IDataProviderSvc *m_evtSvc;
  
  /// SetectorDataSvc, for the objects to invalidate
  mutable IDataProviderSvc *m_detSvc;
  
  /// Incident service, to register as listener
  mutable IIncidentSvc *m_incSvc;
  
  /// UpdateMangerSvc, to invalidate objects
  mutable IUpdateManagerSvc *m_ums;
  
  /// Pointer to the event processor in order to be able to stop the run if
  /// something goes wrong during the incident handling.
  mutable IEventProcessor  *m_evtProc;

};

#endif
