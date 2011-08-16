// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:10 GMT
#ifndef JONASROO3BODYPHASESPACE
#define JONASROO3BODYPHASESPACE

#include "TObject.h"
#include "Rtypes.h"

#include "RooRealProxy.h"
#include "RooAbsReal.h"

class RooM13 : public RooAbsReal{
 public: 
  RooM13(const char* name
	 , const char* title
	 , RooAbsReal& _m12sq
	 , RooAbsReal& _m23sq
	 , RooAbsReal& _mumM
	 , RooAbsReal& _d1M
	 , RooAbsReal& _d2M
	 , RooAbsReal& _d3M
	 )
    : RooAbsReal(name, title) 
    , m12sq("m12sq", "m12sq", this, _m12sq)
    , m23sq("m23sq", "m23sq", this, _m23sq)
    , mumM("mumM", "mumM", this, _mumM)
    , d1M("d1M", "d1M", this, _d1M)
    , d2M("d2M", "d2M", this, _d2M)
    , d3M("d3M", "d3M", this, _d3M)
    {}
  
  RooM13(const RooM13& other
	 , const char* name=0)
    : RooAbsReal(other, name)
    , m12sq("m12sq", this, other.m12sq)
    , m23sq("m23sq",this, other.m23sq)
    , mumM("mumM",this, other.mumM)
    , d1M("d1M",this, other.d1M)
    , d2M("d2M",this, other.d2M)
    , d3M("d3M",this, other.d3M)
    {}
  virtual TObject* clone(  const char* newname) const{
    return new RooM13(*this, newname);}
  inline virtual ~RooM13(){}
  
 protected:
  RooRealProxy m12sq;
  RooRealProxy m23sq;
  RooRealProxy mumM, d1M, d2M, d3M;


  Double_t evaluate() const{
    if(m12sq < 0 || m23sq < 0) return -9999;
    
    Double_t mumMsq = mumM*mumM;
    //    if(m12sq > mumMSq || m23sq > mumMsq) return 0;
    
    Double_t m1sq=d1M*d1M, m2sq=d2M*d2M, m3sq=d3M*d3M;
    Double_t K = mumMsq + m1sq + m2sq + m3sq;
    return K - m12sq - m23sq;
  }

 private:
  ClassDef(RooM13, 0)
};
#endif
//
