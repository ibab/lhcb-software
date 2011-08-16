// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
#ifndef JONASROO4BODYPHASESPACE_H
#define JONASROO4BODYPHASESPACE_H

#include "TObject.h"
#include "Rtypes.h"
#include "TGenPhaseSpace.h"
#include "TLorentzVector.h"

#include "RooRealProxy.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"

#include "RooAbsPhaseSpace.h"

class Roo4BodyPhaseSpace : public RooAbsPhaseSpace{
 public: 
  Roo4BodyPhaseSpace(const char* name
		     , const char* title
		     , RooAbsReal& t01_in
		     , RooAbsReal& s12_in
		     , RooAbsReal& s23_in
		     , RooAbsReal& s34_in
		     , RooAbsReal& t40_in
		     , RooAbsReal& m0_in
		     , RooAbsReal& m1_in
		     , RooAbsReal& m2_in
		     , RooAbsReal& m3_in
		     , RooAbsReal& m4_in
		     );


  Roo4BodyPhaseSpace(const Roo4BodyPhaseSpace& copyMe
		     , const char* name=0);
  virtual TObject* clone(  const char* newname) const{
    return new Roo4BodyPhaseSpace(*this, newname);}
  inline virtual ~Roo4BodyPhaseSpace(){}
 
  Bool_t decayInLimits(TLorentzVector p[4]) const;


  Int_t getAnalyticalIntegral(RooArgSet& allVars
			      , RooArgSet& analVars
			      , const char* rangeName
			      ) const;
  Double_t analyticalIntegral(Int_t code
			      , const char* rangeName
			      ) const;


  void getInvariantsFromPArray(TLorentzVector p[4]);

 
  /*
  Int_t getAnalyticalIntegral(RooArgSet& allVars
			      , RooArgSet& analVars
			      , const char* rangeName
			      ) const{
    return 1;
  }
  Double_t analyticalIntegral(Int_t code
			      , const char* rangeName
			      ) const{
    return 1;
  }
  */

  Int_t getGenerator(const RooArgSet &allVars
		     , RooArgSet &dirVars
		     , Bool_t staticInitOK
		     ) const;

  void generateEvent(Int_t code);
  virtual RooArgSet defaultParameterSet() const{
    RooArgSet aset( t01.arg()
		    , s12.arg(), s23.arg(), s34.arg()
		    , t40.arg());
    return aset;
  }
  virtual RooArgList defaultParameterList() const{
    RooArgList alist( t01.arg()
		      , s12.arg(), s23.arg(), s34.arg()
		      , t40.arg());
    return alist;
  }
  

 protected:

  mutable TGenPhaseSpace gps;

  RooRealProxy t01;
  RooRealProxy s12;
  RooRealProxy s23;
  RooRealProxy s34;
  RooRealProxy t40;
  RooRealProxy m0;
  RooRealProxy m1;
  RooRealProxy m2;
  RooRealProxy m3;
  RooRealProxy m4;

  Double_t evaluate() const;

 private:
  ClassDef(Roo4BodyPhaseSpace, 0)
};
#endif
//
