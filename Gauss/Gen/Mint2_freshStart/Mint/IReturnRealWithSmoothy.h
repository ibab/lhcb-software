#ifndef IRETURN_REALVALUE_WITHSMOOTHY_HH
#define IRETURN_REALVALUE_WITHSMOOTHY_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include "Mint/IReturnReal.h"
#include <complex>

namespace MINT{

class IReturnRealWithSmoothy : virtual public IReturnReal{
  protected:
    IReturnRealWithSmoothy(){};
  public:
    //virtual double RealVal()=0;
    virtual double SmootherLargerRealVal()=0;
    virtual ~IReturnRealWithSmoothy(){};
  };
  
}//namespace MINT
#endif
