// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
#ifndef JROO3BODYPHASESPACE_H
#define JROO3BODYPHASESPACE_H

#include "TObject.h"
//#include "Rtypes.h"

#include "RooRealProxy.h"
#include "RooAbsReal.h"
#include "RooAbsPdf.h"
#include "RooArgList.h"

#include "RooAbsPhaseSpace.h"

class Roo3BodyPhaseSpace : public RooAbsPhaseSpace{
 public: 
  Roo3BodyPhaseSpace(const char* name
		     , const char* title
		     , RooAbsReal& _m12sq
		     , RooAbsReal& _m23sq
		     , RooAbsReal& _mumM
		     , RooAbsReal& _d1M
		     , RooAbsReal& _d2M
		     , RooAbsReal& _d3M
		     );
  Roo3BodyPhaseSpace(const Roo3BodyPhaseSpace& copyMe
		     , const char* name=0);
  virtual TObject* clone(  const char* newname) const{
    return new Roo3BodyPhaseSpace(*this, newname);}
  inline virtual ~Roo3BodyPhaseSpace(){}

  Int_t getAnalyticalIntegral(RooArgSet& allVars
			      , RooArgSet& analVars
			      , const char* rangeName
			      ) const;
  Double_t analyticalIntegral(Int_t code
			      , const char* rangeName
			      ) const;

  Int_t getGenerator(const RooArgSet &allVars
		     , RooArgSet &dirVars
		     , Bool_t staticInitOK
		     ) const;

  void generateEvent(Int_t code);

  static bool boundaries(Double_t& s12Min
			 , Double_t& s12Max
			 , Double_t _s23
			 , Double_t _mumM
			 , Double_t _d1M, Double_t _d2M, Double_t _d3M
			 );
  Double_t m12AbsMin() const;
  Double_t m23AbsMin() const;
  Double_t m12AbsMax() const;
  Double_t m23AbsMax() const;

  virtual RooArgList defaultParameterList() const{
    RooArgList alist(m12sq.arg(), m23sq.arg());
    return alist;
  }
  virtual RooArgSet defaultParameterSet() const{
    RooArgSet aset(m12sq.arg(), m23sq.arg());
    return aset;
  }

 protected:
  RooRealProxy m12sq;
  RooRealProxy m23sq;
  RooRealProxy mumM, d1M, d2M, d3M;
  
  bool m23Boundaries(Double_t& m23sqMin, Double_t& m23sqMax) const;
  bool m12Boundaries(Double_t& m12sqMin, Double_t& m12sqMax) const;
  Double_t evaluate() const;

 private:
  ClassDef(Roo3BodyPhaseSpace, 0)
};
#endif
//
