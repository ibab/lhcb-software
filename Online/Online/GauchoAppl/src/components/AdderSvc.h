#ifndef ONLINE_GAUCHO_ADDERSVC_H
#define ONLINE_GAUCHO_ADDERSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"

// Forward declarations
class MonAdder;
class AdderSys;
class SaveTimer;
class IMonitorSvc;
class MyErrh;
class IIncidentSvc;
class IGauchoMonitorSvc;
class IHistogramSvc;
namespace AIDA { class IHistogram1D; }

class AdderSvc : public Service, virtual public IIncidentListener
{
public:
  AdderSvc(const std::string& name, ISvcLocator* sl);
  virtual ~AdderSvc();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  StatusCode initialize();
  StatusCode start();
  StatusCode stop();
  StatusCode finalize();
  void gotoPause();
  virtual void handle(const Incident&);
  SaveTimer *m_SaveTimer;

private:
  IMonitorSvc        *m_pMonitorSvc;
  IHistogramSvc      *m_phistsvc;
  AIDA::IHistogram1D *m_arrhist;
  AdderSys  *m_AdderSys;
  MonAdder  *m_adder;
  MonAdder  *m_EoRadder;
  SaveTimer *m_EoRSaver;
  int m_recvtmo;
  MyErrh *m_errh;
  /// Reference to the IncidentSvc instance
  IIncidentSvc             *m_incidentSvc;

  std::string m_InputDNS;
  std::string m_OutputDNS;
  std::string m_SaveRootDir;
  std::string m_PartitionName;
  std::string m_SaverTaskName;
  std::string m_MyName;
  std::string m_TaskPattern;
  std::string m_ServicePattern;
  std::string m_AdderClass;
  bool m_dohisto;
  bool m_expandRate;
  bool m_isSaver;
  bool m_isEOR;
  bool m_started;
  bool m_doPause;
  bool m_PauseOnEOR;
  int  m_SaveInterval; //in seconds
  bool m_SaveonUpdate;
  bool m_DebugOn;
  std::string m_MyServiceName;
};
#endif // ONLINE_GAUCHO_ADDERSVC_H
