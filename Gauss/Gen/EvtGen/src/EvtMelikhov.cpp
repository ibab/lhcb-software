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
// Module: EvtMelikhov.cc
//
// Description: Routine to implement semileptonic B->D*lnu decays according
//              to the model HQET
//
// Modification history:
//
//    DJL     April 20, 1998        Module created
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtMelikhov.hh"

#include "EvtGen/EvtString.hh"

EvtMelikhov::~EvtMelikhov() {}

void EvtMelikhov::getName(EvtString& model_name){

  model_name="MELIKHOV";     
}


EvtDecayBase* EvtMelikhov::clone(){

  return new EvtMelikhov;

}

void EvtMelikhov::decay( EvtParticle *p ){

  p->initializePhaseSpace(getNDaug(),getDaugs());
  calcamp->CalcAmp(p,_amp2,Melikhovffmodel);
}


void EvtMelikhov::init(){

  checkNArg(1);
  checkNDaug(3);

  //We expect the parent to be a scalar 
  //and the daughters to be X lepton neutrino

  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(0,EvtSpinType::VECTOR);
  checkSpinDaughter(1,EvtSpinType::DIRAC);
  checkSpinDaughter(2,EvtSpinType::NEUTRINO);


  Melikhovffmodel = new EvtMelikhovFF(getArg(0));
  calcamp = new EvtSemiLeptonicVectorAmp; 
  
}

