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
// Module: EvtWSB.cc
//
// Description: Routine to implement semileptonic decays according
//              to the model WSB
//
// Modification history:
//
//    DJL/RYD   September 25, 1996           Module created
//
//------------------------------------------------------------------------
//
#include <stdlib.h> 
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtWSB.hh"

#include "EvtGen/EvtString.hh"

EvtWSB::~EvtWSB() {}

void EvtWSB::getName(EvtString& model_name){

  model_name="WSB";     

}


EvtDecayBase* EvtWSB::clone(){

  return new EvtWSB;

}


void EvtWSB::decay( EvtParticle *p ){


  p->initializePhaseSpace(getNDaug(),getDaugs());

  calcamp->CalcAmp(p,_amp2,wsbffmodel);
  return;

}


void EvtWSB::init(){

  // check that there are 0 arguments
  checkNArg(0);
  checkNDaug(3);

  //We expect the parent to be a scalar 
  //and the daughters to be X lepton neutrino

  EvtSpinType::spintype mesontype=EvtPDL::getSpinType(getDaug(0));

  checkSpinParent(EvtSpinType::SCALAR);
  checkSpinDaughter(1,EvtSpinType::DIRAC);
  checkSpinDaughter(2,EvtSpinType::NEUTRINO);

  wsbffmodel = new EvtWSBFF;
  
  if ( mesontype==EvtSpinType::SCALAR ) { 
    calcamp = new EvtSemiLeptonicScalarAmp; 
  }
  if ( mesontype==EvtSpinType::VECTOR ) { 
    calcamp = new EvtSemiLeptonicVectorAmp; 
  }
  
}

