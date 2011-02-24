// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:11 GMT
#ifndef DALITZABSCOMPLEXBRACKETS
#define DALITZABSCOMPLEXBRACKETS
#include "TObject.h"
#include "Rtypes.h"

#include <complex>

class AbsComplexBrackets{
 public:
  virtual std::complex<Double_t> getCVal() const=0;
  virtual std::complex<Double_t> operator()() const{return getCVal();}
  //ClassDef(AbsComplexBrackets, 0)
};
//#ifdef __MAKECINT__
//#pragma link C++ class std::complex<Double_t>+;
//#endif

#endif
//
