// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
#ifndef RHO_OMEGA_H
#define RHO_OMEGA_H
#include "TObject.h"
#include "Rtypes.h"

#include <complex>

#include "RooRealProxy.h"
#include "AbsComplex.h"

#include "DcyAmplitude.h"

class rhoOmega : public AbsComplex{
  mutable std::complex<Double_t> _result;
 protected:
  RooRealProxy amp, phase;
  RooRealProxy rhoAmp, omegaAmp;

  std::complex<Double_t> omegaFactor() const;
 public:
  rhoOmega(){}
  rhoOmega(const char* name
	   , const char* title
	   , RooAbsReal& _amp
	   , RooAbsReal& _phase
	   , DcyAmplitude& _rhoAmp
	   , DcyAmplitude& _omegaAmp
	   )
    : AbsComplex(name, title)
    , _result(0)
    , amp("amp", "amp", this, _amp)
    , phase("phase", "phase", this, _phase)
    , rhoAmp("rhoAmp", "rhoAmp", this, _rhoAmp)
    , omegaAmp("omegaAmp", "omegaAmp", this, _omegaAmp)
  {}
  rhoOmega(const rhoOmega& other
	    , const char* name=0) 
    : AbsComplex(other, name)
    , _result(other._result)
    , amp("amp", this, other.amp)
    , phase("phase", this, other.phase)
    , rhoAmp("rhoAmp", this, other.rhoAmp)
    , omegaAmp("omegaAmp", this, other.omegaAmp)
    {}

  virtual TObject* clone(const char* newname) const{
    return new rhoOmega(*this, newname);}
  virtual TObject* clone() const{
    return new rhoOmega(*this);}
  inline virtual ~rhoOmega(){}

  virtual Double_t getVal() const;
  virtual Double_t evaluate() const;
  virtual std::complex<Double_t> getCVal() const;

  ClassDef(rhoOmega, 0)
};

#endif
//
