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
// Module: EvtSTS.cc
//
// Description: Routine to decay scalar -> tensor scalar.
//
//
// Modification history:
//
//    RYD       Aug  21, 1998       Module created
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtCPUtil.hh"
#include "EvtGen/EvtTensor4C.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtSTS.hh"
#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtString.hh"

EvtSTS::~EvtSTS() {}

void EvtSTS::getName(EvtString& model_name){

  model_name="STS";     

}


EvtDecayBase* EvtSTS::clone(){

  return new EvtSTS;

}

void EvtSTS::initProbMax(){

  setProbMax(20.0);

}

void EvtSTS::init(){

  // check that there are 0 arguments
  checkNArg(0);
  checkNDaug(2);
    
  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(0,EvtSpinType::TENSOR);
  checkSpinDaughter(1,EvtSpinType::SCALAR);

}


void EvtSTS::decay( EvtParticle *p){

  p->initializePhaseSpace(getNDaug(),getDaugs());

  EvtParticle* t1=p->getDaug(0);

  EvtVector4R momt = t1->getP4();
  EvtVector4R moms = p->getDaug(1)->getP4();
  double masst = t1->mass();

  EvtVector4R p4_parent=momt+moms;

  double m_parent=p4_parent.mass();

  double norm=masst*masst/(m_parent*momt.d3mag()*momt.d3mag());
   
  vertex(0,norm*t1->epsTensorParent(0).cont1(p4_parent)*p4_parent);
  vertex(1,norm*t1->epsTensorParent(1).cont1(p4_parent)*p4_parent);
  vertex(2,norm*t1->epsTensorParent(2).cont1(p4_parent)*p4_parent);
  vertex(3,norm*t1->epsTensorParent(3).cont1(p4_parent)*p4_parent);
  vertex(4,norm*t1->epsTensorParent(4).cont1(p4_parent)*p4_parent);

  return ;
}

