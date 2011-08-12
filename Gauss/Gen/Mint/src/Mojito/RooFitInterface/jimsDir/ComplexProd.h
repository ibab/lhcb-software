// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#ifndef JONASCOMPLEXSUM
#define JONASCOMPLEXSUM

#include "TObject.h"
#include "Rtypes.h"

#include "RooRealProxy.h"
#include "RooListProxy.h"
#include "RooAbsReal.h"
#include "RooAbsPdf.h"

#include <complex>
#include "AbsComplexPdf.h"

class ComplexProd : public AbsComplexPdf{
  mutable complex<Double_t> _result;
  complex<Double_t> getProd() const;
 public: 
  ComplexProd(const char* name
	   , const char* title
	   , RooArgList& _resonances
	   );
  ComplexProd(const ComplexProd& copyMe
	   , const char* name=0);
  virtual TObject* clone(  const char* newname) const{
    return new ComplexProd(*this, newname);}
  inline virtual ~ComplexProd(){}
  
 protected:
  RooListProxy resonances;

  virtual complex<Double_t> getCVal() const{
    this->getVal();
    return _result;
  }
  Double_t evaluate() const;

 private:
  ClassDef(ComplexProd, 0)
};
#endif
//
