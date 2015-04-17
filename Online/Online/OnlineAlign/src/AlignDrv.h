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
#include "OnlineAlign/IAlignUser.h"
#include "OnlineAlign/IAlignSys.h"
#include <cstdio>
// Forward declarations
//class DimService;
//class ISimpleTrendWriter;
namespace LHCb
{
  class AlignDrv: public extends2< OnlineService, IRunable, IAlignDrv>
  {
    private:
      long rdRef(FILE *);
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
      long *m_RefBase;
      bool m_firstRef;
      bool m_runonce;
      IGauchoMonitorSvc *m_MonSvc;
      IAlignIterator *m_fitter;
      void writeReference();
      void setReferenceBase(long);
      void doContinue();
      void doStop();
      IIncidentSvc *incidentSvc () {return OnlineService::incidentSvc();};
      IToolSvc *m_ToolSvc;
      IGauchoMonitorSvc *getMonSvc();
  };
}
#endif // ONLINE_GAUCHO_DUMAlignDrv_H
