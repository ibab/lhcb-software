/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#ifndef ROOBACKPDF
#define ROOBACKPDF

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooAbsReal.h"
 
class RooBackPdf : public RooAbsPdf {
public:
  RooBackPdf(const char *name, const char *title, RooAbsReal& _x,RooAbsReal& _mr, RooAbsReal& _p1,RooAbsReal& _p2,RooAbsReal& _p3);
  RooBackPdf(const RooBackPdf& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new RooBackPdf(*this,newname); }
  inline virtual ~RooBackPdf() { }

protected:

  RooRealProxy x ;
  RooRealProxy  mr ;
  RooRealProxy  p1 ;
  RooRealProxy  p2 ;
  RooRealProxy  p3 ;
  
  Double_t evaluate() const ;

private:

  ClassDef(RooBackPdf,0) // Your description goes here...
};
 
#endif
