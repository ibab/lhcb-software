// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:10 GMT
#ifndef ROOBINOMIDALITZ_H
#define ROOBINOMIDALITZ_H

#include "TObject.h"
#include "Rtypes.h"

#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"
#include "RooAbsPdf.h"

class RooBinomiDalitz : public RooAbsPdf{
 public: 
  RooBinomiDalitz(const char* name
	    , const char* title
	    , RooAbsReal& _ResonancesPlus
	    , RooAbsReal& _ResonancesMinus
	    , RooAbsCategory& _bcharge
	    , RooAbsReal& _prodRatio
	   );
  RooBinomiDalitz(const RooBinomiDalitz& copyMe
	   , const char* name=0);

  virtual TObject* clone(  const char* newname) const{
    return new RooBinomiDalitz(*this, newname);}
  inline virtual ~RooBinomiDalitz(){}


  virtual Bool_t selfNormalized() const { return kTRUE ; }

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

 protected:
  mutable int counter;
  RooRealProxy ResonancesPlus, ResonancesMinus, prodRatio;
  RooCategoryProxy bcharge;

  Double_t evaluate() const;

 private:
  ClassDef(RooBinomiDalitz, 0)
};
#endif
//
