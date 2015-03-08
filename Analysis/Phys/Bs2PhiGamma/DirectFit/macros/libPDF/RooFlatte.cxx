 /*****************************************************************************
  * Project: BaBar detector at the SLAC PEP-II B-factory
  * Package: RooRarFit
  *    File: $Id: RooFlatte.cc,v 1.3 2011/08/26 17:54:16 fwilson Exp $
  * Authors: Fergus Wilson
  * Hist        ory:
  * 
  * Copyright (C) 2005-2012, RAL
  *****************************************************************************/

 // -- CLASS DESCRIPTION [PDF] --
 // This is an implementation of the BES parameterisation of the Flatte
 // distribution
 //     
// Reference : S.M.Flatte Phys. Rev. B63, 224 (1976); 
// B.S.Zou and D.V.Bugg Phys Rev. D48, R3948 (1993);
// M.Ablikim wt al (BES collaboration), Phys. Rev. D70, 092002, (2004)
//
//
// BEGIN_HTML
// This is an implementation of the BES parameterisation of the Flatte
// distribution
//
// Reference : S.M.Flatte Phys. Rev. B63, 224 (1976); 
// B.S.Zou and D.V.Bugg Phys Rev. D48, R3948 (1993);
// M.Ablikim wt al (BES collaboration), Phys. Rev. D70, 092002, (2004)
//
// END_HTML
//

#include "Riostream.h"
#include "RooFlatte.h"
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include "RooComplex.h"
#include <math.h> 
#include "TMath.h" 

ClassImp(RooFlatte)
  
//------------------------------------------------------------------
  RooFlatte::RooFlatte(const char *name, const char *title,
                       RooAbsReal& _x, RooAbsReal& _mean,
                       RooAbsReal& _g0, RooAbsReal& _m0a, RooAbsReal& _m0b, 
                       RooAbsReal& _g1, RooAbsReal& _m1a, RooAbsReal& _m1b) :
    RooAbsPdf(name,title),
    x("x","Dependent",this,_x),
    mean("mean","Mean",this,_mean),
    g0("g0","Channel 1 coupling",this,_g0),
    m0a("m0a","Mass of particle 1 in channel 1",this,_m0a),
    m0b("m0b","Mass of particle 2 in channel 1",this,_m0b),
    g1("g1","Channel 2 coupling",this,_g1),
    m1a("m1a","Mass of particle 1 in channel 2",this,_m1a),
    m1b("m1b","Mass of particle 2 in channel 2",this,_m1b){}
 
//------------------------------------------------------------------
 RooFlatte::RooFlatte(const RooFlatte& other, 
                      const char* name) : 
   RooAbsPdf(other,name), 
   x("x",this,other.x), 
   mean("mean",this,other.mean),
   g0("g0",this,other.g0),
   m0a("m0a",this,other.m0a),
   m0b("m0b",this,other.m0b),
   g1("g1",this,other.g1),
   m1a("m1a",this,other.m1a),
   m1b("m1b",this,other.m1b){ }

//------------------------------------------------------------------
Double_t RooFlatte::evaluate() const{
  // calculate Flatte amplitude
  
  if (g0<0 || g1<0) {return(0);}
  
  Double_t s = x*x;
  
  // Energy, centre of mass p^2 of first channel
  Double_t E0a = 0.5 * (s + m0a*m0a - m0b*m0b) / x;
  Double_t qSq0 = E0a*E0a - m0a*m0a; 
  
  // Energy, centre of mass p^2 of second channel
  Double_t E1a = 0.5 * (s + m1a*m1a - m1b*m1b) / x;
  Double_t qSq1 = E1a*E1a - m1a*m1a; 
  
  RooComplex gamma0 = (qSq0 > 0) ? RooComplex(g0*sqrt(qSq0),0) : 
    RooComplex(0, g0*sqrt(-qSq0));
  
  RooComplex gamma1 = (qSq1 > 0) ? RooComplex(g1*sqrt(qSq1),0) : 
    RooComplex(0, g1*sqrt(-qSq1));
  
  RooComplex gamma = gamma0 + gamma1;
  
  RooComplex partB = RooComplex(0.0, 2*mean/x) * gamma;
  RooComplex partA(mean*mean - s, 0);
  
  RooComplex denom = partA - partB;
  
  //RooComplex T(mean*sqrt(g0*g1),0);
  RooComplex T(1,0);
  T = T / denom;
  return(T.abs2()); // Amplitude (arbitrary scale) 
}
