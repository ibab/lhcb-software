#ifndef ROOFLATTE
#define ROOFLATTE

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"


class RooFlatte : public RooAbsPdf {
public:
  RooFlatte(const char *name, const char *title,
            RooAbsReal& _x, RooAbsReal& _mean, 
            RooAbsReal& _g0, RooAbsReal& _m0a, RooAbsReal& _m0b,
            RooAbsReal& _g1, RooAbsReal& _m1a, RooAbsReal& _m1b);
  RooFlatte(const RooFlatte& other,const char* name=0) ;
  virtual TObject* clone(const char* newname) const {return new RooFlatte(*this,newname);}
  inline virtual ~RooFlatte() { }
  
protected:
  
  RooRealProxy x ;     // 
  RooRealProxy mean ;  // resonance mass
  RooRealProxy g0 ;  // square of the coupling constant to channel 1
  RooRealProxy m0a ; // mass of first final state particle in channel 1
  RooRealProxy m0b ; // mass of second final state particle in channel 1
  RooRealProxy g1 ;  // square of the coupling constant to channel 2
  RooRealProxy m1a ; // mass of first final state particle in channel 2
  RooRealProxy m1b ; // mass of second final state particle in channel 2
  
  Double_t evaluate() const ;
  
private:
  ClassDef(RooFlatte,0); // Flatte PDF using BES parameterisation
};



#endif
