#ifndef FIT_COMPLEX_CART_HH
#define FIT_COMPLEX_CART_HH
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
#include "Mint/IFitParRegister.h"
#include "Mint/FitParDependent.h"


namespace MINT{
  class FitComplexCart : public FitComplex{
  //  FitComplexCart(const FitComplexCart& ){};
  // no copying for now.
  // dangerous because for each
  // FitParameter, a pointer
  // is held in MinuitParametSet.
  // Anyway, what would the copied parameter
  // mean?
 protected:
  FitParameter _real, _imag;

  void defaultInit();

  static double _degFac;
  static double degFac();
 public:
  inline const FitParameter& real() const{return _real;}
  inline const FitParameter& imag() const{return _imag;}

  FitParameter& real(){return _real;}
  FitParameter& imag(){return _imag;}

  virtual FitComplex::TYPE type()const{return FitComplex::CARTESIAN;}

  virtual FitParameter& p1(){return _real;}
  virtual FitParameter& p2(){return _imag;}
  virtual const FitParameter& p1() const{return _real;}
  virtual const FitParameter& p2() const{return _imag;}

  static std::string makeRealName(const std::string& varName);
  static std::string makeImagName(const std::string& varName);

  virtual void set(std::complex<double> z);

  FitComplexCart(const std::string& varName
		 , const char* fname=0
		 , MinuitParameterSet* pset=0
		 , MINT::IFitParRegister* daddy=0
		 , FitParameter::FIX_OR_WHAT fow=FitParameter::FIX
		 , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		 );
  FitComplexCart(const std::string& varName
		 , MinuitParameterSet* pset
		 , MINT::IFitParRegister* daddy=0
		 , FitParameter::FIX_OR_WHAT fow=FitParameter::FIX
		 , NamedParameterBase::VERBOSITY vb=NamedParameterBase::VERBOSE
		 );

  virtual ~FitComplexCart();

  virtual std::complex<double> getVal() const;
  virtual std::complex<double> getValInit() const;
  virtual bool gotInitialised()const;
  virtual void print(std::ostream& os = std::cout) const;

  virtual operator std::complex<double>() const{
    return getVal();
  }

  virtual bool isZero()const{
    return (0.0 == real() && 0.0 == imag());
  }
};
}//namespace MINT
#endif
//
