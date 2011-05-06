#ifndef IRETURN_COMPLEXVALUE_HH
#define IRETURN_COMPLEXVALUE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include <complex>

namespace MINT{
  class IReturnComplex{
  protected:
    IReturnComplex(){}
  public:
    virtual std::complex<double> ComplexVal()=0;
    virtual ~IReturnComplex(){};
  };
  
}//namespace MINT
#endif
