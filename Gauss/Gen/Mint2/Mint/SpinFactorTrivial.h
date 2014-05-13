#ifndef SPINFACTOR_TRIVIAL_HH
#define SPINFACTOR_TRIVIAL_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

#include "Mint/CLHEPSystemOfUnits.h"
#include "Mint/IDalitzEvent.h"

#include "Mint/ISpinFactor.h"

// 2-body and trivial spin factor:
class SpinFactorTrivial : virtual public ISpinFactor{
 public:
  virtual double getVal(){return 1.0;}
  virtual double getVal(IDalitzEvent& ){return getVal();}

  virtual double RealVal(){return getVal();}
  virtual double RealVal(IDalitzEvent& ){return RealVal();}
  
  virtual std::complex<double> ComplexVal(){return std::complex<double>(getVal(),0);}
  virtual std::complex<double> ComplexVal(IDalitzEvent& ){return ComplexVal();}

  virtual std::string name() const{
    return "TrivialSpinFactor=1";
  }
  
};

#endif
//
