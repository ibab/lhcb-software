// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:11 GMT
#ifndef DALITZABSCOMPLEX
#define DALITZABSCOMPLEX
#include "TObject.h"
//#include "Rtypes.h"

#include <complex>
#include "RooAbsReal.h"
#include "RooAbsArg.h"
#include "AbsComplexBrackets.h"

class AbsComplex : public RooAbsReal, public AbsComplexBrackets{
 public:
  AbsComplex() : RooAbsReal(){}
  AbsComplex(const char *name, const char *title, const char *unit= "") :
    RooAbsReal(name, title, unit)
    , AbsComplexBrackets(){
    constOptimizeTestStatistic(RooAbsArg::DeActivate);
  }

  AbsComplex(const char *name, const char *title
	     , Double_t minVal, Double_t maxVal, 
             const char *unit= ""):
    RooAbsReal(name, title, minVal, maxVal, unit)
    , AbsComplexBrackets(){
    constOptimizeTestStatistic(RooAbsArg::DeActivate);
  }

  AbsComplex(const AbsComplex& other, const char* name=0) :
    RooAbsReal(other, name)
    , AbsComplexBrackets(){
    constOptimizeTestStatistic(RooAbsArg::DeActivate);
  }
  
  virtual ~AbsComplex(){}

  virtual Double_t getVal(const RooArgList* al=0) const{
    return evaluate();
  }
  virtual Double_t evaluate() const{
    return abs( (*this)() );
  }

  //ClassDef(AbsComplex, 0)
};
//#ifdef __MAKECINT__
//#pragma link C++ class std::complex<Double_t>+;
//#endif

#endif
//
