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
// Module: EvtScalarParticle.cc
//
// Description: Class to describe scalar particles
//
// Modification history:
//
//    DJL/RYD   September 25, 1996           Module created
//
//------------------------------------------------------------------------
// 
#include <iostream.h>
#include <math.h>
#include "EvtGen/EvtScalarParticle.hh"
#include "EvtGen/EvtVector4R.hh"


void EvtScalarParticle::init(EvtId part_n,double e,double px,double py,double pz){

  setp(e,px,py,pz);
  setpart_num(part_n);

  setLifetime();

}

EvtScalarParticle::~EvtScalarParticle() {}


void EvtScalarParticle::init(EvtId part_n,const EvtVector4R& p4){

  setp(p4);
  setpart_num(part_n);

  setLifetime();

}

EvtSpinDensity EvtScalarParticle::rotateToHelicityBasis() const{

  EvtSpinDensity R;
  R.SetDim(1);
      
  R.Set(0,0,1.0);

  return R;

}


EvtSpinDensity EvtScalarParticle::rotateToHelicityBasis(double alpha,
						       double beta,
						       double gamma) const{

  EvtSpinDensity R;
  R.SetDim(1);
      
  R.Set(0,0,1.0);

  return R;

}

