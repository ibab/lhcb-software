// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:06 GMT
#ifndef DCYCOUPLEDAMPLITUDE
#define DCYCOUPLEDAMPLITUDE
#include "TObject.h"
#include "Rtypes.h"

#include <complex>

#include "RooRealProxy.h"
#include "AbsComplex.h"

class DcyCoupledAmplitude : public AbsComplex{
  mutable std::complex<Double_t> _result;
 protected:
  RooRealProxy s,mAC,mBC;
  RooRealProxy amp, phase;
  RooRealProxy m, G, J, gres, gother;
  RooRealProxy mA,mB,mC,mD,mE,mF;

  Double_t pres() const
  {
    Double_t numsq = (s-pow(mA+mB,2))*(s-pow(mA-mB,2));
    Double_t num=0.0;
    if (numsq>0.0) num=sqrt(numsq);
    return num/2.0*sqrt(s);
  }

  Double_t pother() const
  {
    Double_t numsq = (s-pow(mE+mF,2))*(s-pow(mE-mF,2));
    Double_t num=0.0;
    if (numsq>0.0) num=sqrt(numsq);
    return num/2.0*sqrt(s);
  }

Double_t GofM() const
{
  //  return G;
  Double_t rhores = 2.0*pres()/sqrt(s);
  Double_t rhoother = 2.0*pother()/sqrt(s);
  
  return rhores*pow(gres,2)+rhoother*pow(gother,2);   
  
}

Double_t sJ() const
{
  //  return 1;
  
  if (J==0) return 1;
  if (J==1) {

    
    Double_t spinFactor = (mAC-mBC+(pow(mD,2)-pow(mC,2))*(pow(mB,2)-pow(mA,2))/pow(m,2));
    //cout << spinFactor << endl;
    
    return spinFactor;
    
  }
  if (J==2) { 
    //return 1;
    
    Double_t term1=mBC-mAC+((pow(mD,2)-pow(mC,2))*(pow(mA,2)-pow(mB,2))/pow(m,2));
    Double_t term2=s-2.0*pow(mD,2)-2.0*pow(mC,2)+(pow(mD,2)-pow(mC,2))*(pow(mD,2)-pow(mC,2))/pow(m,2);
    Double_t term3=s-2.0*pow(mA,2)-2.0*pow(mB,2)+(pow(mA,2)-pow(mB,2))*(pow(mA,2)-pow(mB,2))/pow(m,2);
    //    cout << "BlattWeisskopf " << BlattWeisskopf() << endl;
    //cout << "term1 = " << term1 << " term2 = " << term2 << " term 3 " << term3 << endl;
    
    Double_t SpinFactor = (pow(term1,2)-(1.0/3.0)*term2*term3);
    //cout << "Total spin factor " << SpinFactor << endl;
    
    return SpinFactor;
    
  }
  if (J>2) {
    cout << "For J= " << J << " spin dependence not supported " << endl;
  }
  
  
}

  

  Double_t den() const{
    Double_t r = m*m - s;
    Double_t c = m*GofM();
    return r*r + c*c; 
  }
  

 public:
  DcyCoupledAmplitude(){}
  DcyCoupledAmplitude(const char* name
                      , const char* title
                      , RooAbsReal& _s ,RooAbsReal& _mAC, RooAbsReal& _mBC
                      , RooAbsReal& _amp, RooAbsReal& _phase 
                      , RooAbsReal& _m, RooAbsReal& _G, RooAbsReal& _J
                      , RooAbsReal& _gres, RooAbsReal& _gother
                      , RooAbsReal& _mA, RooAbsReal& _mB
                      , RooAbsReal& _mC, RooAbsReal& _mD
                      , RooAbsReal& _mE, RooAbsReal& _mF)
    : AbsComplex(name, title)
    , _result(0)
    , s("s", "s", this, _s)
    , mAC("mAC", "mAC", this, _mAC)
    , mBC("mBC", "mBC", this, _mBC)
    , amp("amp", "amp", this, _amp)
    , phase("phase", "phase", this, _phase)
    , m("m", "m", this, _m)
    , G("G", "G", this, _G)
    , J("J", "J", this, _J)
    , gres("gres", "gres", this, _gres)
    , gother("gother", "gother", this, _gother)
    , mA("mA","mA",this,_mA)
    , mB("mB","mB",this,_mB)
    , mC("mC","mC",this,_mC)
    , mD("mD","mD",this,_mD)
    , mE("mE","mE",this,_mE)
    , mF("mF","mF",this,_mF)
{
  }
  DcyCoupledAmplitude(const DcyCoupledAmplitude& other
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
    , J("J",this, other.J)
      , gres("gres",this, other.gres)
     , gother("gother",this, other.gother)
    , mA("mA",this, other.mA)
    , mB("mB",this, other.mB)
    , mC("mC",this, other.mC)
      , mD("mD",this, other.mD)
      , mE("mE",this, other.mE)
    , mF("mF",this, other.mF)
{
  }
  virtual TObject* clone(const char* newname) const{
    return new DcyCoupledAmplitude(*this, newname);}
  virtual TObject* clone() const{
    return new DcyCoupledAmplitude(*this);}
  inline virtual ~DcyCoupledAmplitude(){}

  Double_t ReBW() const{
    return sJ() * (m*m - s)/den();
  }
  Double_t ImBW() const{
    return sJ() * GofM()/den();
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
    getVal();
    if(false)std::cout << " for " << s.arg().GetName() 
		       << " = " << s 
		       << " I, " << this->GetName() 
		       << ", return "  << _result 
		       << " (" << abs(_result) << ")" 
		       << std::endl;
    return _result;
  }

  ClassDef(DcyCoupledAmplitude, 0)
};

#endif
//
