/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
  * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/

#ifndef ROOPSI2M
#define ROOPSI2M

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"
 
class RooPsi2m : public RooAbsPdf {
public:
  RooPsi2m() {} ; 
  RooPsi2m(const char *name, const char *title,
	      RooAbsReal& _x,
	      RooAbsReal& _p1,
	      RooAbsReal& _p2,
	      RooAbsReal& _p3,
	   RooAbsReal& _lambda,
	   RooAbsReal& _ap0,
           RooAbsReal& _ap1,
RooAbsReal& _ap2 );
  RooPsi2m(const RooPsi2m& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new RooPsi2m(*this,newname); }
  inline virtual ~RooPsi2m() { }

protected:

  RooRealProxy ap0 ;
  RooRealProxy ap1 ;
  RooRealProxy ap2 ;
  RooRealProxy x ;
  RooRealProxy p1 ;
  RooRealProxy p2 ;
  RooRealProxy p3 ;
  RooRealProxy lambda ;
  
  Double_t evaluate() const ;

private:

  double formFactor() const;

  double PS() const;

  ClassDef(RooPsi2m,1) // Your description goes here...
};
 
#endif
