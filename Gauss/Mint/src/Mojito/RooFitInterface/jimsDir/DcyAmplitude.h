// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:06 GMT
#ifndef DCYAMPLITUDE
#define DCYAMPLITUDE
#include "TObject.h"
#include "Rtypes.h"

#include <complex>

#include "RooRealProxy.h"
#include "AbsComplex.h"

class DcyAmplitude : public AbsComplex{
  mutable std::complex<Double_t> _result;
 protected:
  RooRealProxy spinFactor;
  RooRealProxy s;
  RooRealProxy amp, phase;
  RooRealProxy mR, G;
  RooRealProxy m1_nominal, m1, m2_nominal, m2, R;

  Int_t spin;

  Double_t den() const;
  Double_t twoBody_dgtPsq_in_MumsFrame(Double_t mumM
				       , Double_t m1
				       , Double_t m2
				       ) const;

  Double_t prSq() const;
  Double_t pABSq() const;

 public:
  DcyAmplitude(){}
  DcyAmplitude(const char* name
	       , const char* title
	       , RooAbsReal& _spinFactor
	       , RooAbsReal& _s
	       , RooAbsReal& _amp, RooAbsReal& _phase
	       , RooAbsReal& _mR, RooAbsReal& _G
	       , RooAbsReal& _m1_nominal
	       , RooAbsReal& _m1
	       , RooAbsReal& _m2_nominal
	       , RooAbsReal& _m2
	       , RooAbsReal& _Radius
	       , Int_t _spin
	       )
    : AbsComplex(name, title)
    , _result(0)
    , spinFactor("spinFactor", "spinFactor", this, _spinFactor)
    , s("s", "s", this, _s)
    , amp("amp", "amp", this, _amp)
    , phase("phase", "phase", this, _phase)
    , mR("mR", "mR", this, _mR)
    , G("G", "G", this, _G)
    , m1_nominal("m1_nominal", "m1_nominal", this, _m1_nominal)
    , m1("m1", "m1", this, _m1)
    , m2_nominal("m2_nominal", "m2_nominal", this, _m2_nominal)
    , m2("m2", "m2", this, _m2)
    , R("R", "R", this, _Radius)
    , spin(_spin)
  {}
  DcyAmplitude(const DcyAmplitude& other
	    , const char* name=0) 
    : AbsComplex(other, name)
    , _result(other._result)
    , spinFactor("spinFactor", this, other.spinFactor)
    , s("s", this, other.s)
    , amp("amp", this, other.amp)
    , phase("phase", this, other.phase)
    , mR("mR",this, other.mR)
    , G("G",this, other.G)
    , m1_nominal("m1_nominal", this, other.m1_nominal)
    , m1("m1", this, other.m1)
    , m2_nominal("m2_nominal", this, other.m2_nominal)
    , m2("m2", this, other.m2)
    , R("R", this, other.R)
    , spin(other.spin)
    {}

  virtual TObject* clone(const char* newname) const{
    return new DcyAmplitude(*this, newname);}
  virtual TObject* clone() const{
    return new DcyAmplitude(*this);}
  inline virtual ~DcyAmplitude(){}

  // we're calculating sj/( M(resonance)^2 - M(2-body system)^2 - i*M(res)Gamma)
  // = sj / (m^2 - s - i m*G)
  // =    sj (m^2 - s) / |m^2 - s - i m*G|^2
  // + i* sj (m*G)     / |m^2 - s - i m*G|^2
  Double_t ReBW() const;
  Double_t ImBW() const;
  std::complex<Double_t> BW() const;
  Double_t ReM() const;
  Double_t ImM() const;
  std::complex<Double_t> M() const;
  virtual Double_t evaluate() const;
  virtual std::complex<Double_t> getCVal() const;


 public:
  Double_t GofM() const;
  Double_t sJ() const;
  Double_t Fr() const;

  Double_t nominalMass() const{ return mR;}
  Double_t myS() const{ return s;}
  Double_t recoMass() const{ return sqrt(s);}
  Double_t absAmp() const{ return amp;}
  Double_t ampsPhase() const{ return phase;}
  Double_t mySpinFactor() const{return spinFactor;}

  ClassDef(DcyAmplitude, 0)
};

#endif
//
