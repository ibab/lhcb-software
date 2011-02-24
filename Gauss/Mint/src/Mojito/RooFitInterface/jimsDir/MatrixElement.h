// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
#ifndef DALITZMATRIXELEMENT
#define DALITZMATRIXELEMENT
#include "TObject.h"
#include "Rtypes.h"

#include <complex>
#include "RooAbsReal.h"
#include "RooRealProxy.h"
#include "RooRealVar.h"
#include "AbsComplex.h"

class UnitMatrixElement : public AbsComplex{
 public:
  UnitMatrixElement() : AbsComplex(){}
  UnitMatrixElement(const char *name, const char *title)
    : AbsComplex(name, title) {}
  UnitMatrixElement(const UnitMatrixElement& other, const char* name=0)
    : AbsComplex(other, name){}
  virtual std::complex<Double_t> getCVal() const{
    return std::complex<Double_t>(1, 0);
  }
  virtual Double_t evaluate() const{
    return abs(this->getCVal());
  }
  ClassDef(UnitMatrixElement,0)
};

class RealMatrixElement : public AbsComplex{
  RooRealProxy re;
 public:
  RealMatrixElement() : AbsComplex(){}
  RealMatrixElement(const char *name, const char *title, RooAbsReal& _re)
    : AbsComplex(name, title)
    , re("re", "real part", this, _re)
    {}
  RealMatrixElement(const char *name, const char *title, Double_t _re)
    : AbsComplex(name, title)
    , re("re", "real part", this
	 ,*( new RooRealVar( ((std::string)"re" + name).c_str()
			     , ((std::string)"re" + name).c_str()
			     , _re)
	     )
	 )
    {}
  RealMatrixElement(const RealMatrixElement& other, const char* name=0)
    : AbsComplex(other, name)
    , re("re", this, other.re)
    {}
  virtual std::complex<Double_t> getCVal() const{
    return std::complex<Double_t>(re, 0);
  }
  virtual Double_t evaluate() const{
    return re;
  }
  ClassDef(RealMatrixElement,0)
};

class MatrixElementWithPhase : public AbsComplex{
 protected:
  RooRealProxy absVal, phase;
 public:
  MatrixElementWithPhase() : AbsComplex(){}
  MatrixElementWithPhase(const char *name, const char *title
			 , RooAbsReal& _absVal
			 , RooAbsReal& _phase)
    : AbsComplex(name, title)
    , absVal("absVal", "absVal", this, _absVal)
    , phase("phase", "phase", this, _phase){
    std::cout << "Hi, I'm a matrix element";
  }
  
  MatrixElementWithPhase(const MatrixElementWithPhase& other
			 , const char* name=0)
    : AbsComplex(other, name)
    , absVal("absVal", this, other.absVal)
    , phase("phase", this, other.phase){}
  
  virtual ~MatrixElementWithPhase(){}
  
  Double_t Re() const{
    return absVal * cos(phase);
  }
  Double_t Im() const{
    return absVal * sin(phase);
  }
  virtual std::complex<Double_t> getCVal() const{
    return std::complex<Double_t>(Re(), Im());
  }
  virtual Double_t evaluate() const{
    return absVal;
  }
  
  ClassDef(MatrixElementWithPhase, 0)
};
//#ifdef __MAKECINT__
//#pragma link C++ class std::std::complex<Double_t>+;
//#endif

#endif
//
