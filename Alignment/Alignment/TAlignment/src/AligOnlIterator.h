/*
 * AligOnlIterator.h
 *
 *  Created on: Oct 6, 2014
 *      Author: beat
 */

#ifndef ALIGONLITERATOR_H_
#define ALIGONLITERATOR_H_

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "RTL/rtl.h"
#include "GaudiKernel/AlgTool.h"
#include "IAlignUser.h"
#include "IAlignSys.h"


  class AligOnlIterator : public AlgTool, virtual public LHCb::IAlignIterator
  {
    public:
  		AligOnlIterator(const std::string &  type, const std::string &  name, const IInterface *  parent  );
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
//      std::string m_CntTaskName;
      std::string m_ParamFileName;
      std::vector<std::string> m_CounterNames;
      std::string m_PartitionName;
      StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
  };
}


#endif /* ALIGONLITERATOR_H_ */
