// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:07 GMT
#ifndef DALITZDKSPIPI
#define DALITZDKSPIPI

#include "TObject.h"
#include "Rtypes.h"

#include "RooRealProxy.h"
#include "RooListProxy.h"
#include "RooAbsReal.h"
#include "RooAbsPdf.h"

#include <complex>
#include "AbsComplexPdf.h"
#include "RooM13.h"

class DKsPiPi : public AbsComplexPdf{
  mutable std::complex<Double_t> _result;
 public: 
  DKsPiPi(const char* name
	  , const char* title
	  , RooAbsReal& _m12sq
	  , RooAbsReal& _m23sq
	  );
  DKsPiPi(const DKsPiPi& copyMe
	   , const char* name=0);
  virtual TObject* clone(  const char* newname) const{
    return new DKsPiPi(*this, newname);}
  inline virtual ~DKsPiPi(){}
  
 protected:
  RooM13 m13sq;
  ComplexSum resSum;

  virtual std::complex<Double_t> getCVal() const{
    getVal();
    return _result;
  }
  Double_t evaluate() const{
    _result = resSum.getCVal();
    return _result.real() * _result.real()
      + _result.imag() * _result.imag();
  }

 private:
  ClassDef(DKsPiPi, 0)
};
#endif
//
