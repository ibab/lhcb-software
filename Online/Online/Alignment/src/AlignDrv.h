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
#include "Alignment/IAlignUser.h"
#include "Alignment/IAlignSys.h"
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
      virtual StatusCode start();
      virtual StatusCode initialize();
      virtual StatusCode finalize();
      virtual StatusCode run();
      virtual StatusCode stop();
      virtual StatusCode pause();
      virtual void setRunOnce();
      virtual void waitRunOnce();
      virtual void handle(const Incident& inc);

      std::string m_PartitionName;
      std::string m_RefFileName;
      std::string m_FitterClass;
      std::string m_FitterName;
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
