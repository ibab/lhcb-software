#ifndef ISPINFACTOR_HH
#define ISPINFACTOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT

#include "Mint/IReturnRealForEvent.h"
#include "Mint/IDalitzEvent.h"

#include <string>

class ISpinFactor : virtual public MINT::IReturnRealForEvent<IDalitzEvent>{
 protected:
 ISpinFactor() : MINT::IReturnRealForEvent<IDalitzEvent>() {}
 public:
  virtual double getVal(IDalitzEvent& evt)=0;
  virtual double RealVal(IDalitzEvent& evt)=0;
  // this is it, so far - but keep
  // option open to demand more
  // functionality than just returning
  // real.
  virtual std::string name() const=0;
  virtual ~ISpinFactor(){}
};

#endif
