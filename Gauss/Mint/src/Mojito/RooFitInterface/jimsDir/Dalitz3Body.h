// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:06 GMT
#ifndef JONASDALITZ3BODY
#define JONASDALITZ3BODY

#include "TObject.h"
#include "Rtypes.h"

#include "RooRealProxy.h"
#include "RooListProxy.h"
#include "RooAbsReal.h"
#include "RooAbsPdf.h"

#include <complex>
#include "AbsComplexPdf.h"
#include "ComplexSum.h"

class Dalitz3Body : public AbsComplexPdf{
  mutable std::complex<Double_t> _result;
 public: 
  Dalitz3Body(const char* name
	      , const char* title
	      , RooAbsReal& _m12sq
	      , RooAbsReal& _m23sq
	      );
  Dalitz3Body(const Dalitz3Body& other
	   , const char* name=0);
  virtual TObject* clone(  const char* newname) const{
    return new Dalitz3Body(*this, newname);}
  inline virtual ~Dalitz3Body(){}
  
 protected:
  static RooRealVar piM  rhoM, rhoW, BM;
  RooM13 m13sq;
  ComplexBW bw12;
  ComplexBW bw13;
  ComplexBW bw23;
 
  ComplexSum resonances;
  Roo3BodyPhaseSpace phaseSpace;

  virtual std::complex<Double_t> getCVal() const{
    getVal();
    return _result;
  }
  Double_t evaluate() const;

 private:
  ClassDef(Dalitz3Body, 0)
};
#endif
//
