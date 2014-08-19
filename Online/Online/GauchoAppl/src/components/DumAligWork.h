#ifndef ONLINE_GAUCHO_DUMALIGWORK_H
#define ONLINE_GAUCHO_DUMALIGWORK_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "GaudiOnline/OnlineService.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "RTL/rtl.h"
#include "IFitterFcn.h"
#include "IDumAligWork.h"

//#include "Gaucho/BRTL_Lock.h"

// Forward declarations
//class DimService;
//class ISimpleTrendWriter;
namespace LHCb
{
  class DumAligWork: public extends2<OnlineService, IDumAligWork,IRunable>
  {
    public:
      DumAligWork(const std::string& name, ISvcLocator* sl);
      virtual ~DumAligWork();
      //IInterface pure virtual member functions
//      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
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
      IFitterFcn * m_fitterFcn;
      void readReference();
      virtual void handle(const Incident& inc);
//  unsigned long long m_prevupdate;

      std::string m_PartitionName;
      std::string m_RefFileName;
      unsigned long m_Reference;
      lib_rtl_lock_t m_Lock;
      lib_rtl_thread_t m_thread;
      bool m_runonce;
      void waitRunOnce();
      void setRunOnce();
      void *getThis(){return this;};
      IToolSvc *m_ToolSvc;
  };
}
#endif // ONLINE_GAUCHO_DUMALIGWORK_H
