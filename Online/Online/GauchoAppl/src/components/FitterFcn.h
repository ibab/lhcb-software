/*
 * FitterFcn.h
 *
 *  Created on: Aug 12, 2014
 *      Author: beat
 */

#ifndef FITTERFCN_H_
#define FITTERFCN_H_
#include "IFitterFcn.h"
#include "Gaucho/IGauchoMonitorSvc.h"
#include "IDumAligWork.h"
namespace LHCb
{
  class FitterFcn : virtual public IFitterFcn
  {
    public:
      std::string m_DataFileName;
      std::string m_ParamFileName;
      std::string m_SvcName;
      std::vector<double> m_dat_x;
      std::vector<double> m_dat_y;
      std::vector<double> m_dat_dy;
      std::string m_RefFileName;
      std::vector<double> m_params;
      std::string m_PartitionName;
      IGauchoMonitorSvc *m_MonSvc;
      IDumAligWork *m_Parent;
      double m_result;
      void init();
      FitterFcn(IDumAligWork*);
      ~FitterFcn();
      double analyze();
      void ReadParams();
      void PubResult(long reference);
      void run();
  };
}
#endif /* FITTERFCN_H_ */
