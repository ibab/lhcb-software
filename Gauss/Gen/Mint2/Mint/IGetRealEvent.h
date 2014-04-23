#ifndef IGETREALEVENT_HH
#define IGETREALEVENT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include "Mint/IReturnReal.h"
#include "Mint/IEventAccess.h"

namespace MINT{

template<typename EVENT>
class IGetRealEvent 
: virtual public IEventAccess<EVENT>
, virtual public IReturnReal
{
 public:
  //  virtual double getVal()=0;
  //  virtual double RealVal()=0;
  virtual ~IGetRealEvent(){}
};

}//namespace MINT

#endif
//
