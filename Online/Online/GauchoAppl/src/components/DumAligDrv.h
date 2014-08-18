#ifndef ONLINE_GAUCHO_DUMALIGDRV_H
#define ONLINE_GAUCHO_DUMALIGDRV_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiOnline/OnlineService.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "RTL/rtl.h"
#include "Gaucho/CounterTask.h"
#include "Gaucho/BRTL_Lock.h"
#include "IFitter.h"
#include "IDumAligDrv.h"
// Forward declarations
//class DimService;
//class ISimpleTrendWriter;
namespace LHCb
{
  class Fitter;
  class DumAligDrv: public OnlineService, virtual public IDumAligDrv
  {
    public:
      DumAligDrv(const std::string& name, ISvcLocator* sl);
      virtual ~DumAligDrv();
      //IInterface pure virtual member functions
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
      virtual StatusCode start();
      virtual StatusCode initialize();
      virtual StatusCode finalize();
      virtual StatusCode run();
      virtual StatusCode stop();
      virtual StatusCode pause();
      virtual void setRunOnce();
      virtual void waitRunOnce();
//      double analyze();
      virtual void handle(const Incident& inc);
//  unsigned long long m_prevupdate;

      std::string m_PartitionName;
      std::string m_RefFileName;
//      std::vector<std::string> m_CounterNames;
//      std::string m_CntDNS;
//      std::string m_CntTask;
      bool m_runonce;
      IGauchoMonitorSvc *m_MonSvc;
      IFitter *m_fitter;
      void writeReference();
      IIncidentSvc *incidentSvc () {return OnlineService::incidentSvc();};
      IToolSvc *m_ToolSvc;
      IGauchoMonitorSvc *getMonSvc();
  };
}
#endif // ONLINE_GAUCHO_DUMALIGDRV_H
