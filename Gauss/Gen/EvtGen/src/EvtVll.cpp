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
// Module: EvtVll.cc
//
// Description: The decay of a vector meson to two leptons,
//              or generally, two spin 1/2 particles.
//              E.g., J/psi -> e+ e-
//
// Modification history:
//
//    RYD       January 17, 1997       Module created
//
//------------------------------------------------------------------------
//
#include <stdlib.h>
#include <iostream>
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtVll.hh"
#include "EvtGen/EvtDiracSpinor.hh"
#include "EvtGen/EvtReport.hh"

EvtVll::~EvtVll() {}

void EvtVll::getName(EvtString& model_name){

  model_name="VLL";     

}


EvtDecayBase* EvtVll::clone(){

  return new EvtVll;

}

void EvtVll::init(){

  // check that there are 0 arguments
  checkNArg(0);
  checkNDaug(2);

  checkSpinParent(EvtSpinType::VECTOR);
  
  checkSpinDaughter(0,EvtSpinType::DIRAC);
  checkSpinDaughter(1,EvtSpinType::DIRAC);

}

void EvtVll::initProbMax(){

  static EvtId PHI=EvtPDL::getId("phi");
  static EvtId PSI=EvtPDL::getId("J/psi");
  static EvtId PSI2S=EvtPDL::getId("psi(2S)");
  static EvtId UPS1=EvtPDL::getId("Upsilon");
  static EvtId UPS2=EvtPDL::getId("Upsilon(2S)");
  static EvtId UPS3=EvtPDL::getId("Upsilon(3S)");
  static EvtId PSI4040=EvtPDL::getId("psi(4040)");
  static EvtId PSI4160=EvtPDL::getId("psi(4160)");
  static EvtId PSI4415=EvtPDL::getId("psi(4415)");


  if (getParentId()==PHI) setProbMax(2.2);
  if (getParentId()==PSI) setProbMax(20.0);
  if (getParentId()==PSI2S) setProbMax(30.0);
  if (getParentId()==UPS1) setProbMax(170.0);
  if (getParentId()==UPS2) setProbMax(180.0);
  if (getParentId()==UPS3) setProbMax(250.0);
  if (getParentId()==PSI4040) setProbMax(50.0);
  if (getParentId()==PSI4160) setProbMax(50.0);
  if (getParentId()==PSI4415) setProbMax(60.0);

}

void EvtVll::decay(EvtParticle *p){

  p->initializePhaseSpace(getNDaug(),getDaugs());

  EvtParticle *l1, *l2;
  l1 = p->getDaug(0);
  l2 = p->getDaug(1);

  EvtVector4C l11, l12, l21, l22;
  l11=EvtLeptonVCurrent(l1->spParent(0),l2->spParent(0));
  l12=EvtLeptonVCurrent(l1->spParent(0),l2->spParent(1));
  l21=EvtLeptonVCurrent(l1->spParent(1),l2->spParent(0));
  l22=EvtLeptonVCurrent(l1->spParent(1),l2->spParent(1));

  EvtVector4C eps0=p->eps(0);
  EvtVector4C eps1=p->eps(1);
  EvtVector4C eps2=p->eps(2);


  vertex(0,0,0,eps0*l11);
  vertex(0,0,1,eps0*l12);
  vertex(0,1,0,eps0*l21);
  vertex(0,1,1,eps0*l22);
  
  vertex(1,0,0,eps1*l11);
  vertex(1,0,1,eps1*l12);
  vertex(1,1,0,eps1*l21);
  vertex(1,1,1,eps1*l22);
  
  vertex(2,0,0,eps2*l11);
  vertex(2,0,1,eps2*l12);
  vertex(2,1,0,eps2*l21);
  vertex(2,1,1,eps2*l22);
  
  return;

}













