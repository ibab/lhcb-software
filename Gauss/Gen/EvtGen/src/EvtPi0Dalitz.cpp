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
// Module: EvtPi0Dalitz.cc
//
// Description: pi0 -> e+ e- gamma 
//
// Modification history:
//
//    DJL/RYD    June 30, 1998          Module created
//
//------------------------------------------------------------------------
//
#include <stdlib.h>
#include <stdio.h>
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtPi0Dalitz.hh"
#include "EvtGen/EvtVector4C.hh"
#include "EvtGen/EvtDiracSpinor.hh"

EvtPi0Dalitz::~EvtPi0Dalitz() {}

void EvtPi0Dalitz::getName(EvtString& model_name){

    model_name="PI0_DALITZ";

}

EvtDecayBase* EvtPi0Dalitz::clone(){

  return new EvtPi0Dalitz;

}


void EvtPi0Dalitz::initProbMax(){

  setProbMax(3.5);

}


void EvtPi0Dalitz::init(){

  // check that there are 0 arguments
  checkNArg(0);
  checkNDaug(3);

    
  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(0,EvtSpinType::DIRAC);
  checkSpinDaughter(1,EvtSpinType::DIRAC);
  checkSpinDaughter(2,EvtSpinType::PHOTON);

}


void EvtPi0Dalitz::decay( EvtParticle *p){

  EvtParticle *ep, *em, *gamma;
  
  p->makeDaughters(getNDaug(),getDaugs());
  ep=p->getDaug(0);
  em=p->getDaug(1);
  gamma=p->getDaug(2);

 

  double mass[3];
   
  double m = p->mass();
 
  findMasses( p, getNDaug(), getDaugs(), mass );

  EvtVector4R p4[3];

  // the next four lines generates events with a weight such that
  // the efficiency for selecting them is good. The parameter below of
  // 0.1 is the size of the peak at low q^2 (in arbitrary units).
  // The value of 0.1 is appropriate for muons. 
  // when you use this remember to remove the cut on q^2!
   
    setWeight(EvtGenKine::PhaseSpacePole
  	    (m,mass[0],mass[1],mass[2],0.00000002,p4));

  ep->init( getDaug(0), p4[0] );
  em->init( getDaug(1), p4[1]);
  gamma->init( getDaug(2), p4[2]);

  //ep em invariant mass^2
  double m2=(p4[0]+p4[1]).mass2();
  EvtVector4R q=p4[0]+p4[1];
  //Just use the prob summed over spins...

  EvtTensor4C w,v;

  v=2.0*(p4[2]*q)*directProd(q,p4[2]) 
    - (p4[2]*q)*(p4[2]*q)*EvtTensor4C::g()
    -m2*directProd(p4[2],p4[2]);
 
  w=4.0*( directProd(p4[0],p4[1]) + directProd(p4[1],p4[0])
	   -EvtTensor4C::g()*(p4[0]*p4[1]-p4[0].mass2()));

  double prob=(real(cont(v,w)))/(m2*m2);
  prob *=(1.0/( (0.768*0.768-m2)*(0.768*0.768-m2)
           +0.768*0.768*0.151*0.151));

  //  report(INFO,"EvtGen") << "prob is "<<prob<<std::endl;
  setProb(prob);

  return;
}


