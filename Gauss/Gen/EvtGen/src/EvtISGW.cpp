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
// Module: EvtISGW.cc
//
// Description: Routine to implement semileptonic decays according
//              to the model ISGW
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
#include "EvtGen/EvtISGW.hh"

#include "EvtGen/EvtString.hh"

EvtISGW::~EvtISGW() {}

void EvtISGW::getName(EvtString& model_name){

  model_name="ISGW";     

}


EvtDecayBase* EvtISGW::clone(){

  return new EvtISGW;

}

void EvtISGW::decay( EvtParticle *p ){


  p->initializePhaseSpace(getNDaug(),getDaugs());

  calcamp->CalcAmp(p,_amp2,isgwffmodel);
  return;

}


void EvtISGW::init(){

  checkNArg(0);
  checkNDaug(3);


  //We expect the parent to be a scalar 
  //and the daughters to be X lepton neutrino

  EvtSpinType::spintype mesontype=EvtPDL::getSpinType(getDaug(0));

  checkSpinParent(EvtSpinType::SCALAR);
  checkSpinDaughter(1,EvtSpinType::DIRAC);
  checkSpinDaughter(2,EvtSpinType::NEUTRINO);


  isgwffmodel = new EvtISGWFF;
  
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

