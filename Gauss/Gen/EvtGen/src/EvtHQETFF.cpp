//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtHQETFF.cc
//
// Description: form factors for B->D*lnu according to HQET
//
// Modification history:
//
//    DJL     April 17, 1998        Module created
//
//------------------------------------------------------------------------
// 
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtHQETFF.hh"
#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtString.hh"
#include <math.h>



EvtHQETFF::EvtHQETFF(double hqetrho2, double hqetr1, double hqetr2) {

  rho2 = hqetrho2;
  r1 = hqetr1;
  r2 = hqetr2;
  return;
}

void EvtHQETFF::getvectorff(EvtId parent,EvtId /*daught*/,
                            double t, double mass, double *a1f,
                            double *a2f, double *vf, double *a0f ){


  double mb=EvtPDL::getNominalMass(parent);
  double w = ((mb*mb)+(mass*mass)-t)/(2.0*mb*mass);

// Form factors have a general form, with parameters passed in
// from the arguements.

  double rstar = ( 2.0*sqrt(mb*mass))/(mb+mass);
  double ha1 = 1-rho2*(w-1);

  *a1f = (1.0 - (t/((mb+mass)*(mb+mass))))*ha1;
  *a1f = (*a1f)/rstar;
  *a2f = (r2/rstar)*ha1;
  *vf = (r1/rstar)*ha1;
  *a0f = 0.0;

  return;
 }

