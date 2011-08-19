#ifndef ILINESHAPE_HH
#define ILINESHAPE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

//#include "Mint/GaussFct.h"

#include <complex>
#include <string>
#include <iostream>

#include "Mint/IGenFct.h"

#include "Mint/counted_ptr.h"
#include "Mint/DalitzCoordinate.h"


class ILineshape{
 public:
  virtual std::complex<double> getVal()=0;
  virtual std::complex<double> getValAtResonance()=0;
  virtual std::complex<double> getSmootherLargerVal()=0;
  // Last one could return getVal if you don't want to implement it

  virtual DalitzCoordinate getDalitzCoordinate(double nSigma=3)const=0;
  virtual void print(std::ostream& out = std::cout) const=0;
  virtual std::string name() const=0;
  virtual MINT::counted_ptr<IGenFct> generatingFunction() const=0;

  //virtual std::complex<double> EvtGenValue()=0; // for debug only, will delete

  /*
  virtual DalitzCoordinate generate(TRandom* rnd) const=0;
  virtual void setGenerationLimits(double mi, double ma)=0;
  virtual double generatingPDFValue() const=0; // normalised!!!
  */
  //  virtual const GaussFct& gaussianApprox()=0;
  virtual ~ILineshape(){}
};

#endif
//
