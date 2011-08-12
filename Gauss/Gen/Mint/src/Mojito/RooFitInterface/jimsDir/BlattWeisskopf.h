// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#ifndef BLATTWEISSKOPF_H
#define BLATTWEISSKOPF_H
#include "TObject.h"
#include "Rtypes.h"

#include "RooRealProxy.h"
#include "RooAbsReal.h"


class BlattWeisskopf : public RooAbsReal{
 protected:
  RooRealProxy s12;
  RooRealProxy mumM, m1, m2;
  RooRealProxy R;
  Int_t spin;

 public:
  BlattWeisskopf( const char* name
		  , const char* title
		  , RooAbsReal& _s12
		  , RooAbsReal& _mumM
		  , RooAbsReal& _m1
		  , RooAbsReal& _m2
		  , RooAbsReal& _R
		  , Int_t _spin
		  )
    : RooAbsReal(name, title)
    , s12 ("s12" , "s12" , this, _s12)
    , mumM("mumM", "mumM", this, _mumM)
    , m1  ("m1"  , "m1"  , this, _m1)
    , m2  ("m2"  , "m2"  , this, _m2)
    , R   ("R"   , "R"   , this, _R)
    , spin(_spin)
    {}


  BlattWeisskopf(const BlattWeisskopf& other
		 , const char* name=0)
    : RooAbsReal(other, name)
    , s12 ("s12" , this, other.s12)
    , mumM("mumM", this, other.mumM)
    , m1  ("m1"  , this, other.m1)
    , m2  ("m2"  , this, other.m2)
    , R   ("R"   , this, other.R)
    , spin(other.spin)
    {}

  virtual TObject* clone(const char* newname) const{
    return new BlattWeisskopf(*this, newname);}
  virtual TObject* clone() const{
    return new BlattWeisskopf(*this);}
  inline virtual ~BlattWeisskopf(){}


  virtual Double_t evaluate() const;

  ClassDef(BlattWeisskopf, 0)
};

#endif
