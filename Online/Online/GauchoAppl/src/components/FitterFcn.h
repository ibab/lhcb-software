/*
 * FitterFcn.h
 *
 *  Created on: Aug 12, 2014
 *      Author: beat
 */

#ifndef FITTERFCN_H_
#define FITTERFCN_H_
#include "GaudiKernel/AlgTool.h"
#include "IFitterFcn.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "IDumAligWork.h"
namespace LHCb
{
  class FitterFcn : public AlgTool, virtual public LHCb::IFitterFcn
  {
    public:
      std::string m_DataFileName;
      std::string m_ParamFileName;
      std::string m_SvcName;
      std::vector<double> m_dat_x;
      std::vector<double> m_dat_y;
      std::vector<double> m_dat_dy;
      std::vector<double> m_params;
      std::string m_PartitionName;
      IGauchoMonitorSvc *m_MonSvc;
      LHCb::IDumAligWork *m_Parent;
      double m_result;
      void init();
      FitterFcn(const std::string &  type, const std::string &  name, const IInterface *  parent  );
      ~FitterFcn();
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
      double analyze();
      void ReadParams();
      void PubResult(long reference);
      void i_run();
      void setParent(void *p){m_Parent = (LHCb::IDumAligWork*)p;return;};
  };
}
#endif /* FITTERFCN_H_ */
