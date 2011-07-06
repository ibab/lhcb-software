// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:06 GMT
#ifndef DBLEAMPLITUDEWITHRHO
#define DBLEAMPLITUDEWITHRHO
#include "TObject.h"
#include "Rtypes.h"

#include <complex>
#include <string>
#include <iostream>

#include "RooRealProxy.h"
#include "RooRealVar.h"
#include "AbsComplex.h"

#include "DcyAmplitude.h"
#include "rhoOmega.h"


RooRealVar DAwithRho_defAmp("DAwithRho_defAmp", "DAwithRho_defAmp", 1.0);

RooRealVar DAwithRho_defPhase("DAwithRho_defPhase", "DAwithRho_defPhase", 0.0);

RooRealVar DAwithRho_defSpinFactor("DAwithRho_defSpinFactor", "DAwithRho_defSpinFactor", 1.0);


class DbleAmplitudeWithRho : public AbsComplex{
  mutable std::complex<Double_t> _result;
 protected:

  RooRealProxy spinFactor;
  RooRealProxy s1;
  RooRealProxy s2;
  RooRealProxy amp;
  RooRealProxy phase;
  RooRealProxy m1;
  RooRealProxy G1;
  RooRealProxy m1d1_nominal;
  RooRealProxy m1d1;
  RooRealProxy m1d2_nominal;
  RooRealProxy m1d2;
  RooRealProxy R1;
  Int_t L1;
  RooRealProxy mrho;
  RooRealProxy Grho;
  RooRealProxy momega;
  RooRealProxy Gomega;
  RooRealProxy omega_by_rhoAmp;
  RooRealProxy omega_by_rhoPhase;
  RooRealProxy m2d1_nominal;
  RooRealProxy m2d1;
  RooRealProxy m2d2_nominal;
  RooRealProxy m2d2;
  RooRealProxy R2;
  Int_t L2;


  DcyAmplitude *A1Ptr, *ArhoPtr, *AomegaPtr;
  rhoOmega *A2Ptr;

  RooRealProxy A1, Arho, Aomega;
  RooRealProxy A2;
 public:
  DbleAmplitudeWithRho(){}
  DbleAmplitudeWithRho(const char* name
	       , const char* title
		, RooAbsReal& _spinFactor
		, RooAbsReal& _s1
		, RooAbsReal& _s2
		, RooAbsReal& _amp, RooAbsReal& _phase
		, RooAbsReal& _m1
		, RooAbsReal& _G1
		, RooAbsReal& _m1d1_nominal
		, RooAbsReal& _m1d1
		, RooAbsReal& _m1d2_nominal
		, RooAbsReal& _m1d2
		, RooAbsReal& _R1
		, Int_t _L1
		, RooAbsReal& _mrho
		, RooAbsReal& _Grho
		, RooAbsReal& _momega
		, RooAbsReal& _Gomega
		, RooAbsReal& _omega_by_rhoAmp
		, RooAbsReal& _omega_by_rhoPhase
		, RooAbsReal& _m2d1_nominal
		, RooAbsReal& _m2d1
		, RooAbsReal& _m2d2_nominal
		, RooAbsReal& _m2d2
		, RooAbsReal& _R2
		, Int_t _L2
		)
    : AbsComplex(name, title)
    , _result(0)
    , spinFactor("spinFactor", "spinFactor", this, _spinFactor)
    , s1("s1", "s1", this, _s1)
    , s2("s2", "s2", this, _s2)
    , amp("amp", "amp", this, _amp)
    , phase("phase", "phase", this, _phase)
    , m1("m1", "m1", this, _m1)
    , G1("G1", "G1", this, _G1)
    , m1d1_nominal("m1d1_nominal", "m1d1_nominal", this, _m1d1_nominal)
    , m1d1("m1d1", "m1d1", this, _m1d1)
    , m1d2_nominal("m1d2_nominal", "m1d2_nominal", this, _m1d2_nominal)
    , m1d2("m1d2", "m1d2", this, _m1d2)
    , R1("R1", "R1", this, _R1)
    , L1(_L1)
    , mrho("mrho", "mrho", this, _mrho)
    , Grho("Grho", "Grho", this, _Grho)
    , momega("momega", "momega", this, _momega)
    , Gomega("Gomega", "Gomega", this, _Gomega)
    , omega_by_rhoAmp("omega_by_rhoAmp","omega_by_rhoAmp", this, _omega_by_rhoAmp)
    , omega_by_rhoPhase("omega_by_rhoPhase","omega_by_rhoPhase", this,  _omega_by_rhoPhase)
    , m2d1_nominal("m2d1_nominal", "m2d1_nominal", this, _m2d1_nominal)
    , m2d1("m2d1", "m2d1", this, _m2d1)
    , m2d2_nominal("m2d2_nominal", "m2d2_nominal", this, _m2d2_nominal)
    , m2d2("m2d2", "m2d2", this, _m2d2)
    , R2("R2", "R2", this, _R2)
    , L2(_L2)
    , A1Ptr(new DcyAmplitude( (((std::string) name) + "_1").c_str(),
			      (((std::string) title) + "_1").c_str(),
			      _spinFactor
			      , _s1, _amp, _phase
			      , _m1
			      , _G1
			      , _m1d1_nominal
			      , _m1d1
			      , _m1d2_nominal
			      , _m1d2
			      , _R1
			      , _L1
			      ))
    , ArhoPtr(new DcyAmplitude( (((std::string) name) + "_rho").c_str()
				, (((std::string) title) + "_rho").c_str()
				, DAwithRho_defSpinFactor
				, _s2
				, DAwithRho_defAmp, DAwithRho_defPhase
				, _mrho
				, _Grho
				, _m2d1_nominal
				, _m2d1
				, _m2d2_nominal
				, _m2d2
				, _R2
				, _L2
				))
    , AomegaPtr(new DcyAmplitude( (((std::string) name) + "_omega").c_str()
				  , (((std::string) title) + "_omega").c_str()
				  , DAwithRho_defSpinFactor
				  , _s2
				  , _omega_by_rhoAmp, _omega_by_rhoPhase
				  , _momega
				  , _Gomega
				  , _m2d1_nominal
				  , _m2d1
				  , _m2d2_nominal
				  , _m2d2
				  , _R2
				  , _L2
				  ))
    , A2Ptr(new rhoOmega( (((std::string) name) + "_2").c_str()
			  , (((std::string) title) + "_2").c_str()
			  , DAwithRho_defAmp, DAwithRho_defPhase
			  , *ArhoPtr
			  , *AomegaPtr
			  ))
    , A1("A1", "A1", this, *A1Ptr)
    , Arho("Arho", "Arho", this, *ArhoPtr)
    , Aomega("Aomega", "Aomega", this, *AomegaPtr)
    , A2("A2", "A2", this, *A2Ptr)
    {
      bool db=false;
      if(db){
	cout << " DbleAmpitudeWithRho was just created with: "
	     << "\n Name= " << GetName()
	     << "\n Amp1: " << A1Ptr->absAmp()
	     << "\n Arho: " << ArhoPtr->absAmp()
	     << " \n m1 " << m1  << " = " << A1Ptr->nominalMass()
	     << endl;
      }
    }
  DbleAmplitudeWithRho(const DbleAmplitudeWithRho& other
		, const char* name=0) 
    : AbsComplex(other, name)
    , _result(other._result)
    , spinFactor("spinFactor", this, other.spinFactor)
    , s1("s1", this, other.s1)
    , s2("s2", this, other.s2)
    , amp("amp", this, other.amp)
    , phase("phase", this, other.phase)
    , m1("m1", this, other.m1)
    , G1("G1", this, other.G1)
    , m1d1_nominal("m1d1_nominal", this, other.m1d1_nominal)
    , m1d1("m1d1", this, other.m1d1)
    , m1d2_nominal("m1d2_nominal", this, other.m1d2_nominal)
    , m1d2("m1d2", this, other.m1d2)
    , R1("R1", this, other.R1)
    , L1(other.L1)
    , mrho("mrho", this, other.mrho)
    , Grho("Grho", this, other.Grho)
    , momega("momega", this, other.momega)
    , Gomega("Gomega", this, other.Gomega)
    , omega_by_rhoAmp("omega_by_rhoAmp", this, other.omega_by_rhoAmp)
    , omega_by_rhoPhase("omega_by_rhoPhase", this,  other.omega_by_rhoPhase)
    , m2d1_nominal("m2d1_nominal", this, other.m2d1_nominal)
    , m2d1("m2d1", this, other.m2d1)
    , m2d2_nominal("m2d2_nominal", this, other.m2d2_nominal)
    , m2d2("m2d2", this, other.m2d2)
    , R2("R2", this, other.R2)
    , L2(other.L2)
    , A1("A1", this, other.A1)
    , Arho("Arho", this, other.Arho)
    , Aomega("Aomega", this, other.Aomega)
    , A2("A2", this, other.A2)
    {
    }
  virtual TObject* clone(const char* newname) const{
    return new DbleAmplitudeWithRho(*this, newname);}
  virtual TObject* clone() const{
    return new DbleAmplitudeWithRho(*this);}
  inline virtual ~DbleAmplitudeWithRho(){
    /*
    delete A1;
    delete A2;
    delete Arho;
    delete Aomega;
    */
  }
  
  virtual Double_t evaluate() const{ 

    const DcyAmplitude* A1_constPtr
      = dynamic_cast<const DcyAmplitude*>(&(A1.arg()));
    const rhoOmega* A2_constPtr
      = dynamic_cast<const rhoOmega*>(&(A2.arg()));

    std::complex<Double_t> a1amp =  A1_constPtr->getCVal();
    std::complex<Double_t> a2amp =  A2_constPtr->getCVal();

    _result = a1amp * a2amp;

    Double_t returnVal =  _result.real()*_result.real() 
      +                   _result.imag()*_result.imag();

    if(returnVal > 0.5e6){
    cout << GetName() << " a1, a2: " 
	 << a1amp.real()*a1amp.real() + a1amp.imag()*a1amp.imag()
	 << ", " 
	 << a2amp.real()*a2amp.real() + a2amp.imag()*a2amp.imag()
	 << " returning " << returnVal
	 << endl;
    }

    bool db=false;
    if(db){
      cout << " DbleAmpitudeWithRho: " << GetName()
	   << " in evaluate() "
	   << "\n A1.absAmp = "
	   << A1_constPtr->absAmp()
	   << ", A1.getCVal() = " << A1_constPtr->getCVal()
	   << ", A2.getCVal() = " << A2_constPtr->getCVal()
	   << "  result = " << _result
	   << " returning " << returnVal
	   << endl;
    }
    return returnVal;
  }
  virtual Double_t getVal() const{
    return evaluate();
  }
  virtual std::complex<Double_t> getCVal() const{
    getVal();
    return _result;
  }

  ClassDef(DbleAmplitudeWithRho, 0)
};

#endif
//
