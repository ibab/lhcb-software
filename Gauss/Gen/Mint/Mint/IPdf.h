#ifndef IPDF_HH
#define IPDF_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include "Mint/IEventAccess.h"
#include "Mint/IReturnReal.h"
#include "Mint/IGetRealEvent.h"

namespace MINT{

template<typename EVENT>
class IPdf : virtual public IGetRealEvent<EVENT>{
 public:
  virtual double getVal()=0;
  virtual double RealVal()=0;

  virtual void beginFit()=0;
  virtual void parametersChanged()=0;
  virtual void endFit()=0;

  //  virtual IPdf<EVENT>* Clone() const=0;

};
}// namespace MINT
#endif
//
