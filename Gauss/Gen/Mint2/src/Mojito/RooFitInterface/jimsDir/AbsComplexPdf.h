// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#ifndef DALITZABSCOMPLEXPDF
#define DALITZABSCOMPLEXPDF
#include "TObject.h"
#include "Rtypes.h"

#include <complex>
#include "RooAbsPdf.h"
#include "AbsComplexBrackets.h"

class AbsComplexPdf : public RooAbsPdf, public AbsComplexBrackets{
 protected:
  AbsComplexPdf(const RooAbsPdf& other, const char* name=0) :
    RooAbsPdf(other, name), AbsComplexBrackets(){}
 public:
  inline AbsComplexPdf() : RooAbsPdf(), AbsComplexBrackets() { }
  AbsComplexPdf(const char *name, const char *title) :
    RooAbsPdf(name, title), AbsComplexBrackets(){}
  AbsComplexPdf(const char *name, const char *title
		, Double_t minVal, Double_t maxVal) :
    RooAbsPdf(name, title, minVal, maxVal)
    , AbsComplexBrackets(){}

  virtual ~AbsComplexPdf(){}
  
  virtual Double_t evaluate() const{
    return abs( (*this)() );
  }

  ClassDef(AbsComplexPdf, 0)
};
//#ifdef __MAKECINT__
//#pragma link C++ class std::complex<Double_t>+;
//#endif

#endif
//
