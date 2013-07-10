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
// Module: EvtPhspFlatlifetime.cc
//
// Description: Routine to decay a particle according to phase space,
//              with a flat lifetime distribution
//
// Modification history:
//
//    RYD       January 8, 1997       Module created
//    PR        July 10, 2013         Add flat lifetime generation
//
//------------------------------------------------------------------------
//
#include "EvtGenBase/EvtPatches.hh"
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtPhspFlatLifetime.hh"
#include "EvtGenBase/EvtRandom.hh"
#include <string>

//==============================================================================
// Return the name of the model
//==============================================================================
std::string EvtPhspFlatLifetime::getName(){
  return "PHSPFLATLIFETIME";
}

//==============================================================================
// Copy the model
//==============================================================================
EvtDecayBase* EvtPhspFlatLifetime::clone() {
  return new EvtPhspFlatLifetime;
}

//==============================================================================
// Initialize the model
//==============================================================================
void EvtPhspFlatLifetime::init() {
  // check that there is 1 argument in the decay file
  checkNArg(1);
  // this argument is the lifetime upper edge (in ps)
  m_maxLifetime = getArg( 0 ) ;
}

//==============================================================================
// Compute the maximum probability (max of the pdf)
//==============================================================================
void EvtPhspFlatLifetime::initProbMax() {
  noProbMax();
}

//==============================================================================
// Decay the particle according to the model
//==============================================================================
void EvtPhspFlatLifetime::decay( EvtParticle *p ) {
  p->initializePhaseSpace(getNDaug(),getDaugs());
  // generate the lifetime flat between 0 and max
  double l = EvtRandom::Flat( 0. , m_maxLifetime ) ;
  // modify the lifetime of the particle (in mm)
  p -> setLifetime( l * EvtConst::c * 1.e-12 ) ;
}


