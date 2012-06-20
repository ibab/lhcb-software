#ifndef ONLINE_GAUCHO_PUBSVC_H
#define ONLINE_GAUCHO_PUBSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GauchoAppl/MonAdder.h"

// Forward declarations
class MonAdder;
class AdderSys;
class SaveTimer;
class IMonitorSvc;
class MyErrh;
class IIncidentSvc;
class IGauchoMonitorSvc;
class IHistogramSvc;
class ISimpleTrendWriter;
class PubSvc : public Service, virtual public IIncidentListener
{
public:
  PubSvc(const std::string& name, ISvcLocator* sl);
  virtual ~PubSvc();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  virtual StatusCode initialize();
  virtual StatusCode start();
  virtual StatusCode stop();
  virtual StatusCode finalize();
  virtual void handle(const Incident&);
  IMonitorSvc        *m_pMonitorSvc;
//  IHistogramSvc      *m_phistsvc;
  AIDA::IHistogram1D *m_arrhist;
  AdderSys  *m_AdderSys;
  MonAdder  *m_adder;
  MonAdder  *m_EoRadder;
  SaveTimer *m_SaveTimer;
  SaveTimer *m_EoRSaver;
  int m_recvtmo;
  MyErrh *m_errh;
  /// Reference to the IncidentSvc instance
  IIncidentSvc             *m_incidentSvc;

  std::string m_InputDNS;
  std::string m_PartitionName;
  std::string m_MyName;
  std::string m_TaskPattern;
  std::string m_ServicePattern;
  std::string m_AdderClass;
  std::string m_myservicename;
  bool m_started;
  std::string m_prefix;
  unsigned long long m_prevupdate;
  ISimpleTrendWriter* m_trender;

};
#endif // ONLINE_GAUCHO_PUBSVC_H
