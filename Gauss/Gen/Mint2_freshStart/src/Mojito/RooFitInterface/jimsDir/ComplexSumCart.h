// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:06 GMT
#ifndef COMPLEXSUMCART
#define COMPLEXSUMCART

#include "TObject.h"
#include "Rtypes.h"

#include "RooRealProxy.h"
#include "RooListProxy.h"
#include "RooRealVar.h"
#include "RooAbsReal.h"
#include "RooAbsPdf.h"

#include <complex>
#include <map>

#include "AbsComplex.h"

RooRealVar null("null", "null", 0);
RooRealVar unit("unit", "unit", 1);

class ComplexSumCart : public AbsComplex{
  mutable std::complex<Double_t> _result;
  bool debug;
  std::complex<Double_t> getSum() const;
 public: 
  ComplexSumCart(const char* name
	     , const char* title
	     , RooArgList& _resonances
             , RooAbsReal& _amp = unit
             , RooAbsReal& _phase = null 
	   );
  ComplexSumCart(const ComplexSumCart& copyMe
	   , const char* name=0);
  virtual TObject* clone(  const char* newname) const{
    return new ComplexSumCart(*this, newname);}
  inline virtual ~ComplexSumCart(){}
  
 protected:
  RooListProxy resonances;
  RooRealProxy amp, phase;

  virtual std::complex<Double_t> getComplexFactor() const{
    return std::complex<Double_t>(amp, phase);
  }
 public:
  void addValues(std::map<std::string, Double_t>& valMap) const;
  virtual std::complex<Double_t> getCVal() const{
    getVal();
    return _result;
  }
  
  Double_t evaluate() const;

 private:
  ClassDef(ComplexSumCart, 0)
};
#endif
//
