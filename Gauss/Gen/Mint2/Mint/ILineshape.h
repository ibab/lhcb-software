#ifndef ILINESHAPE_HH
#define ILINESHAPE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

//#include "Mint/GaussFct.h"

#include <complex>
#include <string>
#include <iostream>

#include "Mint/IDalitzEvent.h"
#include "Mint/IGenFct.h"
#include "Mint/IFitParDependent.h"

#include "Mint/counted_ptr.h"
#include "Mint/DalitzCoordinate.h"


class ILineshape : virtual public MINT::IFitParDependent{
 public:
  virtual std::complex<double> getVal(IDalitzEvent& evt)=0;

  virtual DalitzCoordinate getDalitzCoordinate(double nSigma=3)const=0;
  virtual void print(std::ostream& out = std::cout) const=0;
  virtual std::string name() const=0;
  virtual MINT::counted_ptr<IGenFct> generatingFunction() const=0;

  //  virtual bool changedSinceLastCall() const=0;// for IFitParDependent
  virtual ~ILineshape(){}
};

#endif
//
