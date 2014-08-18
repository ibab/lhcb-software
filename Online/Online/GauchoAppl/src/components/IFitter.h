#ifndef ONLINE_GAUCHO_IFITTER_H
#define ONLINE_GAUCHO_IFITTER_H
#include "GaudiKernel/IAlgTool.h"


namespace LHCb
{
  class IFitter : virtual public IAlgTool
  {
    public:
      DeclareInterfaceID(LHCb::IFitter,1,0);
      virtual void write_params(int,std::vector<double> &)=0;
      virtual void write_params(int npar, double *)=0;
      virtual void read_params(int&,std::vector<double> &)=0;
      virtual double getIterationResult()=0;
  };
}
#endif // ONLINE_GAUCHO_IFITTER_H
