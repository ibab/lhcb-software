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
#include "GaudiKernel/SmartIF.h"
#include "XmlTools/IXmlParserSvc.h"

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
  public extends1<Service, IIncidentListener> {

public:

  /// Standard constructor
  RunChangeHandlerSvc(const std::string& name, ISvcLocator* svcloc);

  virtual ~RunChangeHandlerSvc(); ///< Destructor

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
  inline SmartIF<I>& getService(const std::string &name, SmartIF<I> &ptr) const {
    if (UNLIKELY( !ptr.isValid() )) {
      ptr = serviceLocator()->service(name, true);
      if(UNLIKELY( !ptr.isValid() )) {
        throw GaudiException("Service ["+name+"] not found", this->name(),
            StatusCode::FAILURE);
      }
    }
    return ptr;
  }

  /// Get pointer to the event data service.
  inline SmartIF<IDataProviderSvc>& eventSvc() const {
    return getService("EventDataSvc", m_evtSvc);
  }

  /// Get pointer to the detector data service.
  inline SmartIF<IDataProviderSvc>& detectorSvc() const {
    return getService("DetectorDataSvc", m_detSvc);
  }

  /// Get pointer to the incident service.
  inline SmartIF<IIncidentSvc>& incidentSvc() const {
    return getService("IncidentSvc", m_incSvc);
  }

  /// Get pointer to the detector data service.
  inline SmartIF<IUpdateManagerSvc>& updMgrSvc() const {
    return getService("UpdateManagerSvc", m_ums);
  }

  /// Get pointer to the detector data service.
  inline SmartIF<IEventProcessor>& evtProc() const {
    return getService("ApplicationMgr", m_evtProc);
  }

  /// Get pointer to the detector data service.
  inline SmartIF<IXmlParserSvc>& xmlParser() const {
    return getService("XmlParserSvc", m_xmlParser);
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

  /// Modify the object opaque address (flag for update).
  void update(CondData &cond);

  /// Flag for update all the registered objects.
  void update();

  typedef GaudiUtils::Map<std::string,std::string> CondDescMap;
  CondDescMap m_condDesc;

  typedef std::list<CondData> Conditions;
  /// List of objects to modify
  Conditions m_conditions;

  /// Current run number.
  unsigned long m_currentRun;

  /// EventDataSvc, for ODIN
  mutable SmartIF<IDataProviderSvc> m_evtSvc;

  /// SetectorDataSvc, for the objects to invalidate
  mutable SmartIF<IDataProviderSvc> m_detSvc;

  /// Incident service, to register as listener
  mutable SmartIF<IIncidentSvc> m_incSvc;

  /// UpdateMangerSvc, to invalidate objects
  mutable SmartIF<IUpdateManagerSvc> m_ums;

  /// Pointer to the event processor in order to be able to stop the run if
  /// something goes wrong during the incident handling.
  mutable SmartIF<IEventProcessor> m_evtProc;

  /// Pointer to the XML parser service, used to clear the cache of parsed XML
  /// files when we get a RunChangeIncident without actual run change.
  mutable SmartIF<IXmlParserSvc> m_xmlParser;

};

#endif
