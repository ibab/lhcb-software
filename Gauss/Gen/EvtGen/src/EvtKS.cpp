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
// Module: EvtKS.cc
//
// Description: Routine to implement semileptonic decays according
//              to the model KS
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtKS.hh"

#include "EvtGen/EvtString.hh"

EvtKS::~EvtKS() {}

void EvtKS::getName(EvtString& model_name){

  model_name="KS";     

}


EvtDecayBase* EvtKS::clone(){

  return new EvtKS;

}

void EvtKS::decay( EvtParticle *p ){

  p->initializePhaseSpace(getNDaug(),getDaugs());

  calcamp->CalcAmp(p,_amp2,ksffmodel);
  return;

}


void EvtKS::init(){

  checkNArg(0);
  checkNDaug(3);

  //We expect the parent to be a scalar 
  //and the daughters to be X lepton neutrino

  checkSpinParent(EvtSpinType::SCALAR);
  checkSpinDaughter(1,EvtSpinType::DIRAC);
  checkSpinDaughter(2,EvtSpinType::NEUTRINO);

  EvtSpinType::spintype mesontype=EvtPDL::getSpinType(getDaug(0));

  ksffmodel = new EvtKSFF;
  
  if ( mesontype==EvtSpinType::SCALAR ) { 
    calcamp = new EvtSemiLeptonicScalarAmp; 
  }
  if ( mesontype==EvtSpinType::VECTOR ) { 
    calcamp = new EvtSemiLeptonicVectorAmp; 
  }
  if ( mesontype==EvtSpinType::TENSOR ) { 
    calcamp = new EvtSemiLeptonicTensorAmp; 
  }
  
}

