// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:11 GMT
#ifndef DCYNONRES
#define DCYNONRES
#include "TObject.h"
#include "Rtypes.h"

#include <complex>

#include "RooRealProxy.h"
#include "AbsComplex.h"

class DcyNonRes : public AbsComplex{
  mutable std::complex<Double_t> _result;
 protected:
  RooRealProxy amp, phase;
 public:
  DcyNonRes(){}
  DcyNonRes(const char* name
               , const char* title
               , RooAbsReal& _amp, RooAbsReal& _phase) 
    : AbsComplex(name, title)
    , _result(0)
    , amp("amp", "amp", this, _amp)
    , phase("phase", "phase", this, _phase)
{
  }
  DcyNonRes(const DcyNonRes& other
	    , const char* name=0) 
    : AbsComplex(other, name)
    , _result(other._result)
    , amp("amp", this, other.amp)
    , phase("phase", this, other.phase)
{
  }
  virtual TObject* clone(const char* newname) const{
    return new DcyNonRes(*this, newname);}
  virtual TObject* clone() const{
    return new DcyNonRes(*this);}
  inline virtual ~DcyNonRes(){}

  Double_t ReM() const{
    return amp*cos(phase);
  }
  Double_t ImM() const{
    return amp*sin(phase);
  }
  std::complex<Double_t> M() const{
    return std::complex<Double_t>(ReM(), ImM());
  }
  virtual Double_t evaluate() const{ 
    _result = M();
    return _result.real()*_result.real() + _result.imag()*_result.imag();
  }
  virtual std::complex<Double_t> getCVal() const{
    getVal();
    return _result;
  }

  //ClassDef(DcyNonRes, 0)
};

#endif
//
