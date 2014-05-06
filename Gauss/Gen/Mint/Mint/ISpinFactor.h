#ifndef ISPINFACTOR_HH
#define ISPINFACTOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT

#include "Mint/IReturnReal.h"
#include <string>

class ISpinFactor : virtual public MINT::IReturnReal{
 protected:
 ISpinFactor() : MINT::IReturnReal(){}
 public:
  virtual double getVal()=0;
  virtual double RealVal()=0;
  // this is it, so far - but keep
  // option open to demand more
  // functionality than just returning
  // real.
  virtual std::string name() const=0;
  virtual ~ISpinFactor(){}
};

#endif
