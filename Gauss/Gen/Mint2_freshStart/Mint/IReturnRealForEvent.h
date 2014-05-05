#ifndef IRETURN_REALVALUE_GIVEN_EVENT_HH
#define IRETURN_REALVALUE_GIVEN_EVENT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

//#include "Mint/IReturnComplex.h"
#include <complex>

namespace MINT{

template<typename EVENT_TYPE>
class IReturnRealForEvent{
 protected:
  IReturnRealForEvent(){};
 public:
  virtual double RealVal(EVENT_TYPE & evt)=0;
  virtual ~IReturnRealForEvent(){};
};

}//namespace MINT
#endif
