#ifndef GAUDIKERNEL_AdderSVC_H
#define GAUDIKERNEL_AdderSVC_H

#include "GaudiKernel/Service.h"
//#include "GaudiKernel/IMonitorSvc.h"
//#include "Gaucho/IGauchoMonitorSvc.h"
//#include "GaudiKernel/StatEntity.h"
#include "Gaucho/HistAdder.h"
#include "Gaucho/CounterAdder.h"
#include "Gaucho/SaveTimer.h"
#include <string>
#include <map>
#include <set>

// Forward declarations
class ISvcLocator;
//class MonObject;
//class MonSys;
//class MonSubSys;
//class CntrMgr;

//namespace LHCb
//{


class AdderSvc : public Service
{
public:
  AdderSvc(const std::string& name, ISvcLocator* sl);
  virtual ~AdderSvc();
  //IInterface pure virtual member functions
  StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  // Service pure virtual member functions
  StatusCode configure(){return StatusCode::SUCCESS;};
  StatusCode initialize();
  StatusCode start();
  StatusCode stop();
  StatusCode finalize();

private:

  std::string m_utgid;
  HistAdder *m_adder;
  HistAdder *m_EoRadder;
  std::string m_TaskName;
  std::string m_PartitionName;
  std::string m_SaverTaskName;
  std::string m_MyName;
  std::string m_ServiceName;
  std::string m_InputDNS;
  std::string m_OutputDNS;
  std::string m_TaskPattern;
  std::string m_ServicePattern;
  std::string m_AdderType;
  bool m_TopLevel;
  bool m_ExpandRate;
  bool m_isSaver;
  bool m_isEOR;
  SaveTimer *m_SaveTimer;
  SaveTimer *m_EoRSaver;
  std::string m_SaveRootDir;
  int m_SaveInterval; //in seconds


  // MonObjetc to convert conters in rates
//  MonRate  *m_monRate;


public:

};
//}
#endif // GAUDIKERNEL_AdderSVC_H
