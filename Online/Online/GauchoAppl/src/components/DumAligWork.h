#ifndef ONLINE_GAUCHO_DUMALIGWORK_H
#define ONLINE_GAUCHO_DUMALIGWORK_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiOnline/OnlineService.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "dim/dim.hxx"
#include "RTL/rtl.h"

#include "Gaucho/BRTL_Lock.h"

// Forward declarations
//class DimService;
//class ISimpleTrendWriter;
namespace LHCb
{
  class MyTimer;
  class DumAligWork: public OnlineService, virtual public IRunable
  {
    public:
      DumAligWork(const std::string& name, ISvcLocator* sl);
      virtual ~DumAligWork();
      //IInterface pure virtual member functions
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
      virtual StatusCode start();
      virtual StatusCode initialize();
      virtual StatusCode finalize();
      virtual StatusCode run();
      virtual StatusCode i_run();
      virtual StatusCode stop();
      virtual StatusCode i_continue();
      void ReadParams();
      double analyze();
      virtual void handle(const Incident& inc);
//  unsigned long long m_prevupdate;

      std::string m_PartitionName;
      std::string m_DataFileName;
      std::string m_ParamFileName;
      std::string m_SvcName;
      std::vector<double> m_dat_x;
      std::vector<double> m_dat_y;
      std::vector<double> m_dat_dy;

      std::vector<double> m_params;
      double m_result;
      IGauchoMonitorSvc *m_MonSvc;
      MyTimer *m_Timer;
      lib_rtl_lock_t m_Lock;
      lib_rtl_thread_t m_thread;
      bool m_runonce;
      void waitRunOnce();
      void setRunOnce();

  };
  class MyTimer : public DimTimer
  {
    public:
      DumAligWork *m_Worker;
      MyTimer(DumAligWork *);
      void timerHandler();
  };
}
#endif // ONLINE_GAUCHO_DUMALIGWORK_H
