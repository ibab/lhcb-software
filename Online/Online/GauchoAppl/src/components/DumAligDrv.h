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

// Forward declarations
//class DimService;
//class ISimpleTrendWriter;
class TMinuit;
namespace LHCb
{
  class DumAligDrv;
  class Fitter
  {
    public:
      Fitter(DumAligDrv *parent,std::string pFilename);
      TMinuit *m_Minuit;
      DumAligDrv *m_parent;
      StatusCode init();
      StatusCode start();
      StatusCode run();
      StatusCode stop();
      StatusCode de_init();
      void write_params(int,std::vector<double> &params);
      void write_params(int npar, double *params);
      void read_params(int&,std::vector<double> &params);
      double getIterationResult();
      lib_rtl_thread_t m_thread;
      CounterTask *m_cntTask;
      std::string m_ParamFileName;
      BRTLLock *m_Lock;
  };
  class DumAligDrv: public OnlineService, virtual public IRunable
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
//      double analyze();
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
      TMinuit *m_Minuit;
      IGauchoMonitorSvc *m_MonSvc;
      Fitter *m_fitter;
      IIncidentSvc *incidentSvc () {return OnlineService::incidentSvc();};
  };
}
#endif // ONLINE_GAUCHO_DUMALIGDRV_H
