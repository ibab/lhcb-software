#ifndef IGETREALEVENT_WITHSMOOTHY_HH
#define IGETREALEVENT_WITHSMOOTHY_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include "Mint/IReturnRealWithSmoothy.h"
#include "Mint/IEventAccess.h"

namespace MINT{

template<typename EVENT>
class IGetRealEventWithSmoothy
  : virtual public IEventAccess<EVENT>
    , virtual public IGetRealEvent<EVENT>
    , virtual public IReturnRealWithSmoothy
  {
  public:
    //  virtual double getVal()=0;
    //  virtual double RealVal()=0;
    // virtual double SmootherLargerRealVal()=0;
    virtual ~IGetRealEventWithSmoothy(){}
  };
  
}//namespace MINT

#endif
//
