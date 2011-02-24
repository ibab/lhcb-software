// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT
#ifndef FOURMXY
#define FOURMXY

#include "TObject.h"
#include "Rtypes.h"

#include "RooRealProxy.h"
#include "RooAbsReal.h"

#include "Calculate4BodyProps.h"

class fourMxy : public RooAbsReal{
 public: 
  fourMxy(const char* name
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
	  , int _opt
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
      , opt(_opt)
  {}
  
  fourMxy(const fourMxy& other
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
      , opt(other.opt)
    {}
  fourMxy(const fourMxy& other
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
    , opt(newOpt)
    {}
  virtual TObject* clone(  const char* newname) const{
    return new fourMxy(*this, newname);}
  virtual TObject* clone(  const char* newname, int newOpt) const{
    return new fourMxy(*this, newOpt, newname);}
  inline virtual ~fourMxy(){}
  void setOpt(int _opt){
    opt = _opt;    
  }
 protected:
  RooRealProxy t01, s12, s23, s34, t40;
  RooRealProxy m0, m1, m2, m3, m4;
  int opt;

  Double_t evaluate() const{
    Calculate4BodyProps c4bp(t01, s12, s23, s34, t40
			     , m0
			     , m1, m2, m3, m4
			     );

    switch ( opt ){
    case 12:
      return c4bp.s12();
      break;
    case 13:
      return c4bp.s13();
      break;
    case 14:
      return c4bp.s14();
      break;
    case 23:
      return c4bp.s23();
      break;
    case 24:
      return c4bp.s24();
      break;
    case 34:
      return c4bp.s34();
      break;
    case 123:
      return c4bp.s123();
      break;
    case 124:
      return c4bp.s124();
      break;
    case 134:
      return c4bp.s134();
      break;
    case 234:
      return c4bp.s234();
      break;
    default:
      cout << "Error from fourMxy, " << this->GetName()
	   << ": Don't understand option " << opt;
      return -9999;
    }    
  }

 private:
  ClassDef(fourMxy, 0)
};
#endif
//
