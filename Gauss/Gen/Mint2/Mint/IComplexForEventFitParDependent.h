#ifndef ICOMPLEXFOREVENT_FITPARDEPENDENT_HH
#define ICOMPLEXFOREVENT_FITPARDEPENDENT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include <complex>
#include "Mint/IReturnComplexForEvent.h"
#include "Mint/IFitParDependent.h"

namespace MINT{
  template<typename EVENT_TYPE>
  class IComplexForEventFitParDependent
    : virtual public IReturnComplexForEvent<EVENT_TYPE>
    , virtual public IFitParDependent{
  };
  
}//namespace MINT
#endif
