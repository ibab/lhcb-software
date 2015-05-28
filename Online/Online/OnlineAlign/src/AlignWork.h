#ifndef ONLINE_GAUCHO_ALIGNWORK_H
#define ONLINE_GAUCHO_ALIGNWORK_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "GaudiOnline/OnlineService.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "RTL/rtl.h"
#include "OnlineAlign/IAlignUser.h"
#include "OnlineAlign/IAlignSys.h"

namespace LHCb
{
  class AlignWork: public extends2<OnlineService, IAlignWork,IRunable>
  {
    public:
      AlignWork(const std::string& name, ISvcLocator* sl);
      virtual ~AlignWork();
      virtual StatusCode start();
      virtual StatusCode initialize();
      virtual StatusCode finalize();
      virtual StatusCode run();
      virtual StatusCode i_run();
      virtual StatusCode stop();
      virtual StatusCode i_continue();
      IGauchoMonitorSvc *m_MonSvc;
      IGauchoMonitorSvc *getMonSvc(){return m_MonSvc;};
      std::string getPartitionName(){return m_PartitionName;};
      IAlignFcn * m_fitterFcn;
      void readReference();
      unsigned long getReference(){return m_Reference;}
      virtual void handle(const Incident& inc);
//  unsigned long long m_prevupdate;

      std::string m_PartitionName;
      std::string m_RefFileName;
      std::string m_FitFcnName;
      std::string m_FitFcnClass;
      unsigned long m_Reference;
      lib_rtl_thread_t m_thread;
      bool m_runonce;
      void waitRunOnce();
      void setRunOnce();
      void *getThis(){return this;};
      IToolSvc *m_ToolSvc;
  };
}
#endif // ONLINE_GAUCHO_ALIGNWORK_H
