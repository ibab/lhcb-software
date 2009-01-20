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
// Module: EvtVSSExtra.cc
//
// Description: Routine to decay vector-> scalar scalar
//
// Modification history:
//
//    RYD       November 24, 1996       Module created
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtVector4C.hh"
#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtVSSExtra.hh"
#include <iostream>
#include <string>

#include "gsl/gsl_sf_fermi_dirac.h"

EvtVSSExtra::~EvtVSSExtra() {}

void EvtVSSExtra::getName(std::string& model_name){

  model_name="VSSExtra"; 
    
}


EvtDecayBase* EvtVSSExtra::clone(){

  return new EvtVSSExtra;

}

void EvtVSSExtra::init(){

  std::string name;
  getName(name);
  std::cout << "Initialising: " << name << std::endl;

  // check that there are 0 arguments
  checkNArg(0);

  // check that there are 2 daughters
  checkNDaug(2);

  // check the parent and daughter spins
  checkSpinParent(EvtSpinType::VECTOR);
  checkSpinDaughter(0,EvtSpinType::SCALAR);
  checkSpinDaughter(1,EvtSpinType::SCALAR);
}

void EvtVSSExtra::initProbMax() {

   setProbMax(1.0);

}      

void EvtVSSExtra::decay( EvtParticle *p){

  p->initializePhaseSpace(getNDaug(),getDaugs());

  EvtVector4R pdaug = p->getDaug(0)->getP4();
  
  double norm=1.0/pdaug.d3mag();

  vertex(0,norm*pdaug*(p->eps(0)));
  vertex(1,norm*pdaug*(p->eps(1)));
  vertex(2,norm*pdaug*(p->eps(2)));

  std::string name;
  getName(name);
  std::cout << "Decaying: " << name << std::endl;

  return;
}

