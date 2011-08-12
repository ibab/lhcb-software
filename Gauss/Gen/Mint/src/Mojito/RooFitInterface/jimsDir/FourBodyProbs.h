// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT
#ifndef FOURBODYPROBS
#define FOURBODYPROBS

#include "TObject.h"
#include "Rtypes.h"

#include "RooRealProxy.h"
#include "RooAbsReal.h"

#include "Calculate4BodyProps.h"

class FourBodyProbs : public RooAbsReal{
 public: 
  FourBodyProbs(const char* name
		, const char* title
		, RooAbsReal& _t01
		, RooAbsReal& _s12
		, RooAbsReal& _s23
		, RooAbsReal& _s34
		, RooAbsReal& _t40
		, RooAbsReal& _m0
		, RooAbsReal& _m1
		, RooAbsReal& _m2
		, RooAbsReal& _m3
		, RooAbsReal& _m4		
		)
    : RooAbsReal(name, title) 
    , t01("t01", "t01", this, _t01)
    , s12("s12", "s12", this, _s12)
    , s23("s23", "s23", this, _s23)
    , s34("s34", "s34", this, _s34)
    , t40("t40", "t40", this, _t40)
    , m0("m0", "m0", this, _m0)
    , m1("m1", "m1", this, _m1)
    , m2("m2", "m2", this, _m2)
    , m3("m3", "m3", this, _m3)
    , m4("m4", "m4", this, _m4)
    , c4bp(t01, s12, s23, s34, t40
	   , m0
	   , m1, m2, m3, m4
	   );
  {}
  
  FourBodyProbs(const FourBodyProbs& other
	 , const char* name=0)
    : RooAbsReal(other, name)
      , t01("t01", this, other.t01)
      , s12("s12", this, other.s12)
      , s23("s23", this, other.s23)
      , s34("s34", this, other.s34)
      , t40("t40", this, other.t40)
      , m0("m0",this, other.m0)
      , m1("m1",this, other.m1)
      , m2("m2",this, other.m2)
      , m3("m3",this, other.m3)
      , m4("m4",this, other.m4)
    {}
  FourBodyProbs(const FourBodyProbs& other
	  , int newOpt
	  , const char* name=0)
    : RooAbsReal(other, name)
    , t01("t01", this, other.t01)
    , s12("s12", this, other.s12)
    , s23("s23", this, other.s23)
    , s34("s34", this, other.s34)
    , t40("t40", this, other.t40)
    , m0("m0",this, other.m0)
    , m1("m1",this, other.m1)
    , m2("m2",this, other.m2)
    , m3("m3",this, other.m3)
    , m4("m4",this, other.m4)
    {}
  virtual TObject* clone(  const char* newname) const{
    return new FourBodyProbs(*this, newname);}
  virtual TObject* clone(  const char* newname, int newOpt) const{
    return new FourBodyProbs(*this, newOpt, newname);}
  inline virtual ~FourBodyProbs(){}
  void setOpt(int _opt){
    opt = _opt;    
  }

 protected:
  RooRealProxy t01, s12, s23, s34, t40;
  RooRealProxy m0, m1, m2, m3, m4;
  CalculateFourBodyProbs c4bp;

  Double_t evaluate() const{
    c4bp.recalculate(t01, s12, s23, s34, t40
		     , m0
		     , m1, m2, m3, m4
		     );
    return -1;
  }

 private:
  ClassDef(FourBodyProbs, 0)
};
#endif
//
