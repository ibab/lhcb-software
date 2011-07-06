// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:10 GMT
#ifndef SPINFACTORTHREEBODY
#define SPINFACTORTHREEBODY
#include "TObject.h"
#include "Rtypes.h"
#include "TLorentzVector.h"

#include <complex>

#include "RooRealProxy.h"
#include "RooAbsReal.h"

#include "Calculate4BodyProps.h"
class SpinFactorThreeBody : public RooAbsReal{

 protected:
  // protected: // this is an abstract base class, no access to constructor.
 public:
  SpinFactorThreeBody(const char* name
                      , const char* title
                      , RooAbsReal& _MV
                      , RooAbsReal& _mAB
                      , RooAbsReal& _mAC
                      , RooAbsReal& _mBC
                      , RooAbsReal& _mA
                      , RooAbsReal& _mB
                      , RooAbsReal& _mC
                      , RooAbsReal& _mD               
	 )
    : RooAbsReal(name, title) 
      , MV("MV","MV",this,_MV)
      , mAB("mAB","mAB",this,_mAB)
      , mAC("mAC","mAC",this,_mAC)
      , mBC("mBC","mBC",this,_mBC)
      , mA("mA", "mA", this, _mA)
      , mB("mB", "mB", this, _mB)
      , mC("mC", "mC", this, _mC)
      , mD("mD", "mD", this, _mD)
  {}
  
  SpinFactorThreeBody(const SpinFactorThreeBody& other
	 , const char* name=0)
    : RooAbsReal(other, name)
      , MV("MV",this,other.MV)
      , mAB("mAB", this, other.mAB)
      , mAC("mAC", this, other.mAC)
      , mBC("mBC", this, other.mBC)
      , mA("mA",this, other.mA)
      , mB("mB",this, other.mB)
      , mC("mC",this, other.mC)
      , mD("mD",this, other.mD)
    {}

 public:
  
  virtual TObject* clone(const char* newname) const{
    return new SpinFactorThreeBody(*this, newname);}
  virtual TObject* clone() const{
    return new SpinFactorThreeBody(*this);}
  inline virtual ~SpinFactorThreeBody(){}
  
  virtual Double_t evaluate() const{
    std::cout << "THIS SHOULD NEVER BE CALLED - SPIN FACTOR.H" 
	      << std::endl;
    return -1;
  }

 protected:
  RooRealProxy MV;
  RooRealProxy mAB, mAC, mBC;
  RooRealProxy mA, mB, mC, mD;
  ClassDef(SpinFactorThreeBody, 0)
};

#endif
//
