// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT
#ifndef GENWITHPHASESPACE
#define GENWITHPHASESPACE

#include "TObject.h"
#include "Rtypes.h"

#include "RooRealProxy.h"
#include "RooAbsReal.h"
#include "RooAbsPdf.h"

class GenWithPhaseSpace : public RooAbsPdf{
 public: 
  GenWithPhaseSpace(const char* name
	   , const char* title
	   , RooAbsReal& Resonances
	   , RooRealPdf& PhaseSpace
	   );
  GenWithPhaseSpace(const GenWithPhaseSpace& copyMe
	   , const char* name=0);

  virtual TObject* clone(  const char* newname) const{
    return new GenWithPhaseSpace(*this, newname);}
  inline virtual ~GenWithPhaseSpace(){}
  
 protected:
  RooRealProxy Resonances, PhaseSpace;

  Double_t evaluate() const;

 private:
  ClassDef(GenWithPhaseSpace, 0)
};
#endif
//
