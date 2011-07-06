// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
#ifndef ROOABSPHASESPACE
#define ROOABSPHASESPACE

#include "RooArgSet.h"
#include "RooArgList.h"
#include "RooAbsPdf.h"

class RooAbsPhaseSpace : public RooAbsPdf{
 public:

  RooAbsPhaseSpace(const char* name
		   , const char* title
		   )
    : RooAbsPdf(name, title){}
  RooAbsPhaseSpace(const RooAbsPhaseSpace& other
		   , const char* name
		   )
    : RooAbsPdf(other, name){}
  RooAbsPhaseSpace(const RooAbsPdf& other
		   , const char* name
		   )
    : RooAbsPdf(other, name){}
  

  virtual RooArgSet defaultParameterSet() const=0;
  virtual RooArgList defaultParameterList() const=0;
  RooDataSet* generateDefault(int nEvents){
    return generate(defaultParameterSet(), nEvents);
  }
  Double_t getNormDefault() const{
    return getNorm(defaultParameterSet());
  }

 private:
  ClassDef(RooAbsPhaseSpace, 0)

};
#endif
//
