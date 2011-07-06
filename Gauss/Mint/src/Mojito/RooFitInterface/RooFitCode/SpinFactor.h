// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:11 GMT
#ifndef SPINFACTOR
#define SPINFACTOR
#include "TObject.h"
#include "Rtypes.h"
#include "TLorentzVector.h"

#include <complex>
#include <iostream>

#include "RooRealProxy.h"
#include "RooAbsReal.h"

#include "Calculate4BodyProps.h"
class SpinFactor : public RooAbsReal{

 protected:
  bool getMomenta() const{

    Calculate4BodyProps c4bp(t01, s12, s23, s34, t40
			     , m0
			     , m1, m2, m3, m4
			     );
    Double_t psf = c4bp.phaseSpaceFactor();
    if(psf <=0){
      std::cout << "SpinFactor::getMomenta ERROR:"
	   << " Unphysical values. Phase space factor = "
	   << psf << std::endl;
      std::cout << " This is for: " 
	   << t01.arg().GetName() << "=" << t01 << ", "
	   << s12.arg().GetName() << "=" << s12 << ", "
	   << s23.arg().GetName() << "=" << s23 << ", "
	   << s34.arg().GetName() << "=" << s34 << ", "
	   << t40.arg().GetName() << "=" << t40 << ", and \n"
	   << m0 << " -> "
	   << m1 << ", "
	   << m2 << ", "
	   << m3 << ", "
	   << m4 << std::endl;

      return false;
    }
    
    p1 = c4bp.pVec(p1_i);
    p2 = c4bp.pVec(p2_i);
    p3 = c4bp.pVec(p3_i);
    p4 = c4bp.pVec(p4_i);

    return true;
  }
 bool getFastMomenta() const{

    Calculate4BodyProps c4bp(t01, s12, s23, s34, t40
			     , m0
			     , m1, m2, m3, m4
			     );

    p11 = c4bp.pij(p1_i,p1_i);
    p12 = c4bp.pij(p1_i,p2_i);
    p13 = c4bp.pij(p1_i,p3_i);
    p14 = c4bp.pij(p1_i,p4_i);
    p22 = c4bp.pij(p2_i,p2_i);
    p23 = c4bp.pij(p2_i,p3_i);
    p24 = c4bp.pij(p2_i,p4_i);
    p33 = c4bp.pij(p3_i,p3_i);
    p34 = c4bp.pij(p3_i,p4_i);
    p44 = c4bp.pij(p4_i,p4_i);

    return true;
  }
  // protected: // this is an abstract base class, no access to constructor.
 public:
  SpinFactor(const char* name
	     , const char* title
	     , int _p1_i
	     , int _p2_i
	     , int _p3_i
	     , int _p4_i
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
    , p1_i(_p1_i)
    , p2_i(_p2_i)
    , p3_i(_p3_i)
    , p4_i(_p4_i)
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
    , db(false)
  {}
  
  SpinFactor(const SpinFactor& other
	 , const char* name=0)
    : RooAbsReal(other, name)
    , p1_i(other.p1_i)
    , p2_i(other.p2_i)
    , p3_i(other.p3_i)
    , p4_i(other.p4_i)
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
    , db(other.db)
    {}

 public:
  
  virtual TObject* clone(const char* newname) const{
    return new SpinFactor(*this, newname);}
  virtual TObject* clone() const{
    return new SpinFactor(*this);}
  inline virtual ~SpinFactor(){}
  
  virtual Double_t evaluate() const{
    std::cout << "THIS SHOULD NEVER BE CALLED - SPIN FACTOR.H" 
	      << std::endl;
    return -1;
  }

 protected:
  int p1_i, p2_i, p3_i, p4_i;
  RooRealProxy t01, s12, s23, s34, t40;
  RooRealProxy m0, m1, m2, m3, m4;
  bool db;

  mutable TLorentzVector p1, p2, p3, p4;

  mutable Double_t p11;
  mutable Double_t p12;
  mutable Double_t p13;
  mutable Double_t p14;
  mutable Double_t p22;
  mutable Double_t p23;
  mutable Double_t p24;
  mutable Double_t p33;
  mutable Double_t p34;
  mutable Double_t p44;

  //  ClassDef(SpinFactor, 0)
};

#endif
//
