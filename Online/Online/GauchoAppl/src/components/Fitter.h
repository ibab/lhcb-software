#ifndef ONLINE_GAUCHO_FITTER_H
#define ONLINE_GAUCHO_FITTER_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiOnline/OnlineService.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "RTL/rtl.h"
#include "Gaucho/CounterTask.h"
#include "Gaucho/BRTL_Lock.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/AlgTool.h"
#include "IFitter.h"
#include "IDumAligDrv.h"

// Forward declarations
//class DimService;
//class ISimpleTrendWriter;
class TMinuit;
namespace LHCb
{
  class DumAligDrv;
  class Fitter : public AlgTool, virtual public LHCb::IFitter
  {
    public:
      Fitter(const std::string &  type, const std::string &  name, const IInterface *  parent  );
      TMinuit *m_Minuit;
      IDumAligDrv *m_parent;
      StatusCode init(std::string, std::string);
      StatusCode start();
      StatusCode run();
      StatusCode initialize();
      StatusCode stop();
      StatusCode de_init();
      void write_params(int,std::vector<double> &params);
      void write_params(int npar, double *params);
      void read_params(int&,std::vector<double> &params);
      double getIterationResult();
      lib_rtl_thread_t m_thread;
      CounterTask *m_cntTask;
      std::string m_CntTaskName;
      std::string m_ParamFileName;
      std::vector<std::string> m_CounterNames;
      std::string m_CntDNS;
      std::string m_RefFileName;
      std::string m_PartitionName;
      std::string m_CntTask;
      std::vector<double> m_params;
      StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  };
}
#endif // ONLINE_GAUCHO_FITTER_H
