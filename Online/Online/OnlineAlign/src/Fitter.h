#ifndef ONLINE_GAUCHO_FITTER_H
#define ONLINE_GAUCHO_FITTER_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "RTL/rtl.h"
#include "GaudiKernel/AlgTool.h"
#include "OnlineAlign/IAlignUser.h"
#include "OnlineAlign/IAlignSys.h"

// Forward declarations
//class DimService;
//class ISimpleTrendWriter;
class TMinuit;
class CounterTask;
namespace LHCb
{
//  class AlignDrv;
  class Fitter : public AlgTool, virtual public LHCb::IAlignIterator
  {
    public:
      Fitter(const std::string &  type, const std::string &  name, const IInterface *  parent  );
      TMinuit *m_Minuit;
      IAlignDrv *m_parent;
//      StatusCode init(std::string, std::string);
      StatusCode i_start();
      StatusCode i_run();
      StatusCode initialize();
      StatusCode finalize();
      StatusCode stop();
//      StatusCode de_init();
      void write_params(int,std::vector<double> &params);
      void write_params(int npar, double *params);
      void read_params(int&,std::vector<double> &params);
      double getIterationResult();
      lib_rtl_thread_t m_thread;
      CounterTask *m_cntTask;
//      std::string m_CntTaskName;
      std::string m_ParamFileName;
      std::vector<std::string> m_CounterNames;
      std::string m_CntDNS;
      std::string m_PartitionName;
      std::string m_CntTask;
      std::vector<double> m_params;
      StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  };
}
#endif // ONLINE_GAUCHO_FITTER_H
