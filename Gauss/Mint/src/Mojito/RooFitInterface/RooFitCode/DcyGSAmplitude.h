// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:11 GMT
#ifndef DCYGSAMPLITUDE
#define DCYGSAMPLITUDE
#include "TObject.h"
#include "Rtypes.h"
#include "TMath.h"

#include <complex>
#include <iostream>

#include "RooRealProxy.h"
#include "AbsComplex.h"

class DcyGSAmplitude : public AbsComplex{
  mutable std::complex<Double_t> _result;
protected:
  RooRealProxy s,mAC,mBC;
  RooRealProxy amp, phase;
  RooRealProxy m, G;
  RooRealProxy mpi;


  Double_t d() const
  {
    Double_t mpipi = pow(m,2);
    Double_t logfactor = TMath::Log((m+2*k(mpipi))/(2.0*mpi));
    Double_t term1 = (3.0/TMath::Pi())*pow(mpi/k(mpipi),2)*logfactor;
    Double_t term2 = m/(2.0*TMath::Pi()*k(mpipi));
    Double_t term3 = pow(mpi,2)*m/(TMath::Pi()*pow(k(mpipi),3));
    
    return term1+term2-term3;
    
  }
Double_t BlattWeisskopfR() const
  {
    Double_t R=1.5;
    Double_t num = sqrt(1.0+pow(R*k(pow(m,2)),2));
    Double_t den = sqrt(1.0+pow(R*k(s),2));
    
    return num/den;
  }


  Double_t BlattWeisskopfD() const
  {
    return 1.0;
  }
  

  Double_t sJ() const
  {
    return BlattWeisskopfD()*BlattWeisskopfR()*(1.0+d()*G/m)*(mAC-mBC);
  }
  
  Double_t k(Double_t mpipi) const
  {
    if (0.25*mpipi-pow(mpi,2)<=0.0) return 0.0;
    return sqrt(0.25*mpipi-pow(mpi,2));
  }
  Double_t kprime(Double_t mpipi) const
  {
    if (0.25*mpipi-pow(mpi,2)<=0.0) return 0.0;
    return 1.0/(8.0*sqrt(0.25*mpipi-pow(mpi,2)));
  }

  Double_t h(Double_t mpipi) const
  {
    Double_t logterm = TMath::Log((sqrt(mpipi)+2.0*k(mpipi))/(2.0*mpi));
    return (2.0*k(mpipi)*logterm)/(TMath::Pi()*sqrt(mpipi));
  }
  Double_t hprime(Double_t mpipi) const
  {
    //Double_t logterm = TMath::Log((sqrt(mpipi)+2.0*k(mpipi))/(2.0*mpi));
    //Double_t logtermprime = 2.0*mpi*(0.5/sqrt(mpipi)+2.0*kprime(mpipi))/(sqrt(mpipi)+2.0*k(mpipi));
    //Double_t factor = 2.0/TMath::Pi();
    //Double_t denterm1 = sqrt(mpipi)*(kprime(mpipi)*logterm + k(mpipi)*logtermprime);
    //Double_t denterm2 = logterm*k(mpipi)/(2.0*sqrt(mpipi));
    
    
    return h(mpipi)*(0.125/pow(k(mpipi),2)-0.5/mpipi)+0.5/(TMath::Pi()*mpipi);
    
  }
  

  Double_t den() const{
    Double_t rterm2 = G*(pow(m,2)/pow(k(pow(m,2)),3))
      *(pow(k(s),2)*(h(s)-h(pow(m,2)))
      +pow(k(pow(m,2)),2)*hprime(pow(m,2))*(pow(m,2)-s));

    Double_t r = m*m - s+rterm2;
    Double_t c = sqrt(s)*G*pow(k(s)/k(pow(m,2)),3)*(m/sqrt(s))*pow(BlattWeisskopfR(),2);
    
    return r*r + c*c; 
  }

  public:
  DcyGSAmplitude(){}
  DcyGSAmplitude(const char* name
                 , const char* title
                 , RooAbsReal& _s, RooAbsReal& _mAC, RooAbsReal& _mBC 
                 , RooAbsReal& _amp, RooAbsReal& _phase 
                 , RooAbsReal& _m, RooAbsReal& _G
                 , RooAbsReal& _mpi)
    : AbsComplex(name, title)
    , _result(0)
    , s("s", "s", this, _s)
    , mAC("mAC", "mAC", this, _mAC)
    , mBC("mBC", "mBC", this, _mBC)
    , amp("amp", "amp", this, _amp)
    , phase("phase", "phase", this, _phase)
    , m("m", "m", this, _m)
    , G("G", "G", this, _G)
    , mpi("mpi","mpi",this,_mpi)
{
  }
  DcyGSAmplitude(const DcyGSAmplitude& other
	    , const char* name=0) 
    : AbsComplex(other, name)
    , _result(other._result)
    , s("s", this, other.s)
    , mAC("mAC", this, other.mAC)
    , mBC("mBC", this, other.mBC)
    , amp("amp", this, other.amp)
    , phase("phase", this, other.phase)
    , m("m",this, other.m)
    , G("G",this, other.G)
    , mpi("mpi",this, other.mpi)
{
  }
  virtual TObject* clone(const char* newname) const{
    return new DcyGSAmplitude(*this, newname);}
  virtual TObject* clone() const{
    return new DcyGSAmplitude(*this);}
  inline virtual ~DcyGSAmplitude(){}

  Double_t ReBW() const{
    Double_t rterm2 = G*(pow(m,2)/pow(k(pow(m,2)),3))
      *(pow(k(s),2)*(h(s)-h(pow(m,2)))+pow(k(pow(m,2)),2)*hprime(pow(m,2))*(pow(m,2)-s));
    
    
    return sJ() * (m*m - s+rterm2)/den();
  }
  Double_t ImBW() const{
    return sJ() * sqrt(s)*G*pow(k(s)/k(pow(m,2)),3)*(m/sqrt(s))*pow(BlattWeisskopfR(),2)/den();
  } 
  std::complex<Double_t> BW() const{
    return std::complex<Double_t>(ReBW(), ImBW());
  }
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
    _result = M()*BW();
    return _result.real()*_result.real() + _result.imag()*_result.imag();
  }
  virtual std::complex<Double_t> getCVal() const{
    bool dbThis=false;
    getVal();
    if(dbThis){
      std::cout << "DcyGSAmplitude::evaluate() for " << GetName() << ":"
		<< " total " << _result
		<< "\n   > sJ()  = " << sJ()
		<< "\n   > BW()  = " << BW()
		<< "\n   > nominalWidth " << G
		<< "\n   > FR()   = " << BlattWeisskopfR()
		<< "\n   > FR()*BW()   = " << BlattWeisskopfR()*BW()
		<< "\n   > sJ()*BW()   = " << sJ()*BW()
		<< "\n   > M()   = " << M()
		<< "\n   > for reco mass " << sqrt(s)
		<< std::endl;
    }
    if(false && dbThis) std::cout << "DcyGSAmplitude::getCVal() for " 
			 << s.arg().GetName() 
			 << " = " << s 
			 << " I, " << this->GetName() 
			 << ", return "  << _result 
			 << " (" << abs(_result) << ")" 
			 << std::endl;
    return _result;
  }

  //ClassDef(DcyGSAmplitude, 0)
};

#endif
//
