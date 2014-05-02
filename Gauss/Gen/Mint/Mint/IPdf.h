#ifndef IPDF_HH
#define IPDF_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include "Mint/IReturnRealForEvent.h"

namespace MINT{

template<typename EVENT>
class IPdf : virtual public IReturnRealForEvent<EVENT>{
 public:
  virtual double getVal(EVENT & EVT)=0;
  virtual double RealVal(EVENT & EVT)=0;

  virtual void beginFit()=0;
  virtual void parametersChanged()=0;
  virtual void endFit()=0;

  //  virtual IPdf<EVENT>* Clone() const=0;

};
}// namespace MINT
#endif
//
