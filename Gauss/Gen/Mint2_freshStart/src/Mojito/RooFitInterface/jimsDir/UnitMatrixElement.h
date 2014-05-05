// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:10 GMT
#ifndef DALITZMATRIXELEMENT
#define DALITZMATRIXELEMENT
#include "TObject.h"
#include "Rtypes.h"

#include <complex>
#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "AbsComplex.h"

class MatrixElement : public AbsComplex{
 protected:
  RooRealProxy re, im;
 public:
  MatrixElement() : AbsComplex(){}
  MatrixElement(const char *name, const char *title, RooAbsReal& _re, RooAbsReal& _im)
    : AbsComplex(name, title)
    , re("re", "real part", this, _re)
    , im("im", "imag part", this, _im)
    {}

  MatrixElement(const MatrixElement& other, const char* name=0)
    : AbsComplex(other, name)
    , re("re", this, other.re)
    , im("im", this, other.im){}
  
  virtual ~MatrixElement(){}

  virtual complex<Double_t> operator()() const{
    return complex<Double_t>(re, im);
  }
  virtual Double_t evaluate() const{
    return abs( (*this)() );
  }

  ClassDef(MatrixElement, 0)
};
//#ifdef __MAKECINT__
//#pragma link C++ class std::complex<Double_t>+;
//#endif

#endif
//
