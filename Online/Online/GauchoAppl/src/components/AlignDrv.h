#ifndef ONLINE_GAUCHO_DUMAlignDrv_H
#define ONLINE_GAUCHO_DUMAlignDrv_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiOnline/OnlineService.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "RTL/rtl.h"
#include "Gaucho/CounterTask.h"
#include "Gaucho/BRTL_Lock.h"
#include "IAlignUser.h"
#include "IAlignSys.h"
// Forward declarations
//class DimService;
//class ISimpleTrendWriter;
namespace LHCb
{
  class AlignDrv: public extends2< OnlineService, IRunable, IAlignDrv>
  {
    public:
      AlignDrv(const std::string& name, ISvcLocator* sl);
      virtual ~AlignDrv();
      //IInterface pure virtual member functions
//      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
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
      IAlignIterator *m_fitter;
      void writeReference();
      void doContinue();
      void doStop();
      IIncidentSvc *incidentSvc () {return OnlineService::incidentSvc();};
      IToolSvc *m_ToolSvc;
      IGauchoMonitorSvc *getMonSvc();
  };
}
#endif // ONLINE_GAUCHO_DUMAlignDrv_H
