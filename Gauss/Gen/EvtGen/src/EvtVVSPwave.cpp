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
// Module: EvtVVSPwave.cc
//
// Description: Routine to decay vector-> vector scalar in Partial-wave
//              Routine to decay a vector into a vector and scalar.  Started
//              by ryd on Aug 20, 1996.
//
// Modification history:
//
//    RYD       November 24, 1996       Module created
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtVector4C.hh"
#include "EvtGen/EvtTensor4C.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtVVSPwave.hh"
#include "EvtGen/EvtString.hh"

EvtVVSPwave::~EvtVVSPwave() {}

void EvtVVSPwave::getName(EvtString& model_name){

  model_name="VVS_PWAVE";     

}


EvtDecayBase* EvtVVSPwave::clone(){

  return new EvtVVSPwave;

}

void EvtVVSPwave::init(){

  // check that there are 6 arguments
  checkNArg(6);
  checkNDaug(2);

  checkSpinParent(EvtSpinType::VECTOR);
  checkSpinDaughter(0,EvtSpinType::VECTOR);
  checkSpinDaughter(1,EvtSpinType::SCALAR);
}

void EvtVVSPwave::initProbMax() {

  //probmax is 1.0 for all possible decays I think!

   setProbMax(1.0);

}      

void EvtVVSPwave::decay( EvtParticle *p){

  EvtComplex as(getArg(0)*cos(getArg(1)),getArg(0)*sin(getArg(1)));
  EvtComplex ap(getArg(2)*cos(getArg(3)),getArg(2)*sin(getArg(3)));
  EvtComplex ad(getArg(4)*cos(getArg(5)),getArg(4)*sin(getArg(5)));

  if (ap!=EvtComplex(0.0,0.0)) {
    report(ERROR,"EvtGen") << "In EvtVectorToVectorScalar.cc"<<endl;
    report(ERROR,"EvtGen") << "P wave not yet implemented!!"<<endl;
    ::abort();
  }
    
  EvtParticle *v,*s;
  
  p->makeDaughters(getNDaug(),getDaugs());
  v=p->getDaug(0);
  s=p->getDaug(1);



  double m_parent,mass[2];
  int n_daug;
  EvtVector4R p4[2];

// Prepare for phase space routine.  M_b should be
// already set somewhere above.

  m_parent = p->mass();
  n_daug = 2;

  EvtDecayBase::findMasses(p,n_daug,getDaugs(),mass);

//  Need phase space random numbers
  EvtGenKine::PhaseSpace( n_daug, mass, p4, m_parent );

//  Put phase space results into the daughters.
  
  v->init( getDaug(0), p4[0] );
  s->init( getDaug(1), p4[1] );
  
  EvtTensor4C d,g;
  
  g.setdiag(1.0,-1.0,-1.0,-1.0);

  d=ad*((1.0/(p4[0].d3mag()*p4[0].d3mag()))*directProd(p4[0],p4[0])+(1/3.0)*g)+
    as*g;

  EvtVector4C ep0,ep1,ep2;  
  
  ep0=d.cont1(p->eps(0));
  ep1=d.cont1(p->eps(1));
  ep2=d.cont1(p->eps(2));

  vertex(0,0,ep0.cont(v->eps(0).conj()));
  vertex(0,1,ep0.cont(v->eps(1).conj()));
  vertex(0,2,ep0.cont(v->eps(2).conj()));
  
  vertex(1,0,ep1.cont(v->eps(0).conj()));
  vertex(1,1,ep1.cont(v->eps(1).conj()));
  vertex(1,2,ep1.cont(v->eps(2).conj()));
  
  vertex(2,0,ep2.cont(v->eps(0).conj()));
  vertex(2,1,ep2.cont(v->eps(1).conj()));
  vertex(2,2,ep2.cont(v->eps(2).conj()));

  return ;

}
