#ifndef IRETURN_COMPLEXVALUE_GIVEN_EVENT_HH
#define IRETURN_COMPLEXVALUE_GIVEN_EVENT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include <complex>

namespace MINT{
template<typename EVENT_TYPE>
  class IReturnComplexForEvent{
  protected:
    IReturnComplexForEvent(){}
  public:
    virtual std::complex<double> ComplexVal(EVENT_TYPE & evt)=0;
    virtual ~IReturnComplexForEvent(){};
  };
  
}//namespace MINT
#endif
