#ifndef SPINFACTOR_TRIVIAL_HH
#define SPINFACTOR_TRIVIAL_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

#include "Mint/CLHEPSystemOfUnits.h"

#include "Mint/ISpinFactor.h"

// 2-body and trivial spin factor:
class SpinFactorTrivial : virtual public ISpinFactor{
 public:
  virtual double getVal(){return 1.0;}

  virtual double RealVal(){return getVal();}
  
  virtual std::string name() const{
    return "TrivialSpinFactor=1";
  }
  
};

#endif
//
