// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:10 GMT
#ifndef JONASROO3DALTIZ_H
#define JONASROO3DALITZ_H

#include "TObject.h"
#include "Rtypes.h"

#include "RooListProxy.h"
#include "RooRealProxy.h"
#include "RooAbsReal.h"
#include "RooAbsPdf.h"
#include "RooAbsPhaseSpace.h"

#include <string>

class RooDataSet;

class RooDalitz : public RooAbsPdf{
 public: 
  RooDalitz(const char* name
	    , const char* title
	    , RooAbsReal& Resonances
	    , RooAbsPhaseSpace& PhaseSpace
	    , RooArgList& _allVars
	    , std::string whichIntegrator="useOwnIntegrator"
	   );
  RooDalitz(const RooDalitz& copyMe
	   , const char* name=0);

  virtual TObject* clone(  const char* newname) const{
    return new RooDalitz(*this, newname);}
  virtual ~RooDalitz();
  

  //  virtual Bool_t selfNormalized() const { return kTRUE ; }

  
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
 

  void useRooFitsIntegrator(){
    _useRooFitsIntegrator = true;
  }
  void useOwnIntegrator(){
    _useRooFitsIntegrator = false;
  }
  
 protected:
  RooRealProxy Resonances, PhaseSpace;
  RooListProxy allVars;
  mutable RooAbsReal* resClonePtr;
  mutable RooArgSet* resCloneSet;

  bool _useRooFitsIntegrator;

  mutable RooDataSet* phaseSpaceData;//! don't persistify. for integration

  Double_t evaluate() const;

  void updatePhaseSpaceDataSet(int Nevents) const;
  Double_t evalInt(double& variance) const;
  Bool_t withinLimits(const RooArgSet* emms, bool debug=false) const;
  void initInteg() const;
  void finishInteg() const;

  void setupRooFitsIntegrator();
 private:
  ClassDef(RooDalitz, 0)
};
#endif
//
