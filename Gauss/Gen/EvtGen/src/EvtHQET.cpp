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
// Module: EvtHQET.cc
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
#include "EvtGen/EvtHQET.hh"

#include "EvtGen/EvtString.hh"

EvtHQET::EvtHQET():
  hqetffmodel(0)
  ,calcamp(0)
{}

EvtHQET::~EvtHQET() {
  delete hqetffmodel;
  hqetffmodel=0;
  delete calcamp;
  calcamp=0;
}

void EvtHQET::getName(EvtString& model_name){

  model_name="HQET";     

}



EvtDecayBase* EvtHQET::clone(){

  return new EvtHQET;

}


void EvtHQET::decay( EvtParticle *p ){


  p->initializePhaseSpace(getNDaug(),getDaugs());
  calcamp->CalcAmp(p,_amp2,hqetffmodel);
}

void EvtHQET::initProbMax(){

EvtId parnum,mesnum,lnum,nunum;

parnum = getParentId();
mesnum = getDaug(0);
lnum = getDaug(1);
nunum = getDaug(2);

double mymaxprob = calcamp->CalcMaxProb(parnum,mesnum,
                           lnum,nunum,hqetffmodel);

setProbMax(mymaxprob);

}


void EvtHQET::init(){

  checkNArg(3);
  checkNDaug(3);

  //We expect the parent to be a scalar 
  //and the daughters to be X lepton neutrino
  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(1,EvtSpinType::DIRAC);
  checkSpinDaughter(2,EvtSpinType::NEUTRINO);

  hqetffmodel = new EvtHQETFF(getArg(0),getArg(1),getArg(2));
  calcamp = new EvtSemiLeptonicVectorAmp; 
  
}

