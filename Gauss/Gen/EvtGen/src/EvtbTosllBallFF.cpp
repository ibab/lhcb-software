//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 2000      Caltech, UCSB
//
// Module: 
// Description: Form factors for b->sll according to Ali, Ball et al.
//              hep-ph/9910221v2
//
// Modification history:
//
//    Ryd     January 5, 2000         Module created
//
//------------------------------------------------------------------------

#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtbTosllBallFF.hh"
#include <math.h>

EvtbTosllBallFF::EvtbTosllBallFF(){}


void EvtbTosllBallFF::getScalarFF(EvtId parent, EvtId daught,
				  double t, double mass, 
				  double& fp,double& f0,double& ft){

  int model = 1;
  
  double m=EvtPDL::getNominalMass(parent);
  double md=EvtPDL::getNominalMass(daught);
  
  double shat=t/(m*m);
  double shat2=shat*shat;
  double shat3=shat2*shat;

  if (model == 1) {
        //this is Ali-Ball
    fp = 0.319*exp(1.465*shat+0.372*shat2+0.782*shat3);
    f0 = 0.319*exp(0.633*shat-0.095*shat2+0.591*shat3);
    ft = 0.355*exp(1.478*shat+0.373*shat2+0.700*shat3);
  }
  if (model == 2) {
        //QCD sum rules (Colangelo et al)
    fp = 0.25/(1.-t/(5.0*5.0));
    f0 = 0.25/(1.-t/(7.0*7.0));
    ft = - 0.14/((1.0 - t/(5.0*5.0))*(1.0 - t/(7.0*7.0)));
  }
  if (model == 3) {
        // Quark model  (Melikhov et al)         
    fp = 0.36/(1. - 0.048*t + 0.00063*t*t);
    double fm = -0.30/(1. - 0.050*t + 0.00061*t*t);
    f0 = fp + fm*(t/(m*m - md*md));
    ft  = -(m+md)*0.06/(1 -0.049*t + 0.00064*t*t);
  }
  
}


void EvtbTosllBallFF::getVectorFF(EvtId parent, EvtId daught,
				  double t, double mass, 
				  double& a1,double& a2,double& a0, double& v,
				  double& t1, double& t2, double& t3 ){

  int model = 1;
  
  double m=EvtPDL::getNominalMass(parent);
  double md=EvtPDL::getNominalMass(daught);
  
  double shat=t/(m*m);
  double shat2=shat*shat;

  if (model == 1) {
        //this is Ali-Ball
    a1=0.337*exp(0.602*shat+0.258*shat2);
    a2=0.282*exp(1.172*shat+0.567*shat2);
    a0=0.471*exp(1.505*shat+0.710*shat2);
    v=0.457*exp(1.482*shat+1.015*shat2);
    
    t1=0.379*exp(1.519*shat+1.030*shat2);
    t2=0.379*exp(0.517*shat+0.426*shat2);
    t3=0.260*exp(1.129*shat+1.128*shat2);
  }
  if (model == 2) {
        //QCD sum rules (Colangelo et al)
    a1 = 0.30*(1 - 0.023*t);
    a2 = 0.40*(1 + 0.034*t);
    a0 = 0.3/(1.- t/(4.8*4.8));
    v = 0.47/(1.- t/(5.0*5.0));
    
    t1 = 0.19/(1.-t/(5.3*5.3));
    t2 = 0.19*(1. - 0.02*t);
    t3 = -0.7*(1. + 0.005*t); 
  }

  if (model == 3) {
        // Quark model  (Melikhov et al)         
    a1 = 1.6/(1 - 0.0288*t + 0.00028*t*t); a1 = a1/(m+md);
    a2 = (m+md)*0.036/(1. - 0.053*t + 0.00082*t*t);
    double aminus = 0.041/(1. - 0.055*t + 0.00088*t*t);
    double f =  1.60/(1. - 0.0288*t + 0.00028*t*t);
    double aplus = -0.036/(1. - 0.053*t + 0.00082*t*t);
    a0 = (t*aminus + f + (m*m-md*md)*aplus)/(2.0*md);
    v = (m+md)*0.048/(1. - 0.057*t + 0.00085*t*t);

    t1 = 0.28/(1. - 0.058*t + 0.0009*t*t);
    double gplus = -0.28/(1. - 0.058*t + 0.0009*t*t);
    double gminus = 0.24/(1. - 0.059*t + 0.00096*t*t);
    t2 = -gplus - (t*gminus)/(m*m-md*md);
    double h = 0.0037/(1. - 0.075*t + 0.0016*t*t);
    t3 = (m+md)*(m+md)*((gminus/(m*m-md*md) - h/2.));
        
  }

}


