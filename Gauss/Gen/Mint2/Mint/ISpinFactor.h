#ifndef ISPINFACTOR_HH
#define ISPINFACTOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT

#include "Mint/IReturnRealForEvent.h"
#include "Mint/IReturnComplexForEvent.h"
#include "Mint/IDalitzEvent.h"

#include <string>

class ISpinFactor : virtual public MINT::IReturnRealForEvent<IDalitzEvent>,virtual public MINT::IReturnComplexForEvent<IDalitzEvent> {
 protected:
 ISpinFactor() : MINT::IReturnRealForEvent<IDalitzEvent>(),MINT::IReturnComplexForEvent<IDalitzEvent>()  {}
 public:
  virtual double getVal(IDalitzEvent& evt)=0;
  virtual double RealVal(IDalitzEvent& evt)=0;
  virtual std::complex<double> ComplexVal(IDalitzEvent& evt)=0;

  virtual std::string name() const=0;
  virtual ~ISpinFactor(){}
};

#endif
