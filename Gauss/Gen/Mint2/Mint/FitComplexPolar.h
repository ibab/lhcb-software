#ifndef FIT_COMPLEX_POLAR_HH
#define FIT_COMPLEX_POLAR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include <string>
#include <iostream>
#include <complex>
#include <string>

#include "Mint/NamedParameterBase.h"
#include "Mint/NamedParameter.h"
#include "Mint/FitParameter.h"
#include "Mint/Phase.h"

#include "Mint/FitComplex.h"
#include "Mint/FitParDependent.h"
#include "Mint/IFitParRegister.h"

namespace MINT{

  class FitComplexPolar : public FitComplex{
  //  FitComplexPolar(const FitComplexPolar& ){};
  // no copying for now.
  // dangerous because for each
  // FitParameter, a pointer
  // is held in MinuitParametSet.
  // Anyway, what would the copied parameter
  // mean?
 protected:
  FitParameter _amp;
  FitParameter _phase;

  void defaultInit();

  static double _degFac;
  static void calculateDegFac();
 public:
  inline const FitParameter& amp() const{return _amp;}
  inline const FitParameter& phase() const{return _phase;}

  FitParameter& amp(){return _amp;}
  FitParameter& phase(){return _phase;}
  static double degFac(){
    if(_degFac < 0) calculateDegFac();
    return _degFac;
  }

  virtual FitComplex::TYPE type()const{return FitComplex::POLAR;}

  virtual FitParameter& p1(){return _amp;}
  virtual FitParameter& p2(){return _phase;}
  virtual const FitParameter& p1() const{return _amp;}
  virtual const FitParameter& p2() const{return _phase;}

  static std::string makeAmpName(const std::string& varName);
  static std::string makePhaseName(const std::string& varName);

  virtual void set(std::complex<double> z);

  FitComplexPolar(const std::string& varName
		  , const char* fname=0
		  , MinuitParameterSet* pset=0
		  , MINT::IFitParRegister* daddy=0
		  , FitParameter::FIX_OR_WHAT fow=FitParameter::FIX
		  , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		  );
  FitComplexPolar(const std::string& varName
		  , MinuitParameterSet* pset
		  , MINT::IFitParRegister* daddy=0
		  , FitParameter::FIX_OR_WHAT fow=FitParameter::FIX
		  , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		  );
  virtual ~FitComplexPolar();

  virtual std::complex<double> getVal() const;
  virtual std::complex<double> getValInit() const;
  virtual void print(std::ostream& os = std::cout) const;
  virtual bool gotInitialised() const;

  virtual operator std::complex<double>() const{
    return getVal();
  }

  virtual bool isZero()const{
    return (0.0 == amp());
  }

};
}//namespace MINT
#endif
//
