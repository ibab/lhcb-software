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
// Module: EvtPhsp.cc
//
// Description: Routine to decay a particle according th phase space
//
// Modification history:
//
//    RYD       January 8, 1997       Module created
//
//------------------------------------------------------------------------
//
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtPhsp.hh"
#include "EvtGen/EvtString.hh"

EvtPhsp::~EvtPhsp() {}

void EvtPhsp::getName(EvtString& model_name){

  model_name="PHSP";     

}

EvtDecayBase* EvtPhsp::clone(){

  return new EvtPhsp;

}


void EvtPhsp::init(){

  // check that there are 0 arguments
  checkNArg(0);

}

void EvtPhsp::initProbMax(){

  noProbMax();

}

void EvtPhsp::decay( EvtParticle *p ){

  if ( p->getNDaug() != 0 ) {
    //Will end up here because maxrate multiplies by 1.2
    report(DEBUG,"EvtGen") << "In EvtPhsp: has "
			   <<" daugthers should not be here!"<<std::endl;
    return;
  }

  p->initializePhaseSpace(getNDaug(),getDaugs());

  return ;
}


