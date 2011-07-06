// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#ifndef DATLITZCOMPLEXBW
#define DATLITZCOMPLEXBW
#include "TObject.h"
#include "Rtypes.h"

#include <complex>

#include "RooRealProxy.h"
#include "AbsComplex.h"

class ComplexBW : public AbsComplex{
  mutable std::complex<Double_t> _result;
 protected:
  RooRealProxy s, m, G;
  Double_t den() const{
    Double_t r = s-m*m;
    Double_t c = m*G;
    return r*r + c*c; 
  }
 public:
  ComplexBW(){}
  ComplexBW(const char* name
	    , const char* title
	    , RooAbsReal& _s, RooAbsReal& _m, RooAbsReal& _G)
    : AbsComplex(name, title)
    , _result(0)
    , s("s", "s", this, _s)
    , m("m", "m", this, _m)
    , G("G", "G", this, _G){
  }
  ComplexBW(const ComplexBW& other
	    , const char* name=0) 
    : AbsComplex(other, name)
    , _result(other._result)
    , s("s", this, other.s)
    , m("m",this, other.m)
    , G("m",this, other.G){
    std::cout << "copie BW" << std::endl;
  }
  virtual TObject* clone(const char* newname) const{
    return new ComplexBW(*this, newname);}
  virtual TObject* clone() const{
    return new ComplexBW(*this);}
  inline virtual ~ComplexBW(){}

  Double_t Re() const{
    return -m*G * (s - m*m)/den();
  }
  Double_t Im() const{
    return -m*G * m*G/den();
  } 
  
  virtual Double_t evaluate() const{
    _result = std::complex<Double_t>(Re(), Im());
    return _result.real()*_result.real() + _result.imag()*_result.imag();
  }
  virtual std::complex<Double_t> getCVal() const{
    getVal();
    return _result;
  }

  ClassDef(ComplexBW, 0)
};

#endif
//
