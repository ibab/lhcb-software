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
// Description: Routine to decay vector-> vector pi pi where the 
//              decay is S-wave dominated.
//
// Modification history:
//
//    RYD       December 11, 1999       Module created
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
#include "EvtGen/EvtVVpipi.hh"
#include "EvtGen/EvtString.hh"

EvtVVpipi::~EvtVVpipi() {}

void EvtVVpipi::getName(EvtString& model_name){

  model_name="VVPIPI";     

}


EvtDecayBase* EvtVVpipi::clone(){

  return new EvtVVpipi;

}

void EvtVVpipi::init(){

  static EvtId PIP=EvtPDL::getId("pi+");
  static EvtId PIM=EvtPDL::getId("pi-");
  static EvtId PI0=EvtPDL::getId("pi0");

  // check that there are 0 arguments
  checkNArg(0);
  checkNDaug(3);

  checkSpinParent(EvtSpinType::VECTOR);
  checkSpinDaughter(0,EvtSpinType::VECTOR);



  if ((!(getDaug(1)==PIP&&getDaug(2)==PIM))&&
      (!(getDaug(1)==PI0&&getDaug(2)==PI0))) {
    report(ERROR,"EvtGen") << "EvtVVpipi generator expected "
                           << " pi+ and pi- (or pi0 and pi0) "
			   << "as 2nd and 3rd daughter. "<<std::endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
    ::abort();
  }

}

void EvtVVpipi::initProbMax() {

  //Hard coded... should not be hard to calculate...
  setProbMax(0.08);

}      

void EvtVVpipi::decay( EvtParticle *p){

  EvtParticle *v,*s1,*s2;
  
  p->makeDaughters(getNDaug(),getDaugs());
  v=p->getDaug(0);
  s1=p->getDaug(1);
  s2=p->getDaug(2);



  double m_parent,mass[3];
  int n_daug;
  EvtVector4R p4[3];

// Prepare for phase space routine.  M_b should be
// already set somewhere above.

  m_parent = p->mass();
  n_daug = 3;

  EvtDecayBase::findMasses(p,n_daug,getDaugs(),mass);

//  Need phase space random numbers
  EvtGenKine::PhaseSpace( n_daug, mass, p4, m_parent );

//  Put phase space results into the daughters.
  
  v->init( getDaug(0), p4[0] );
  s1->init( getDaug(1), p4[1] );
  s2->init( getDaug(2), p4[2] );
  
  EvtVector4C ep0,ep1,ep2;  
  
  ep0=p->eps(0);
  ep1=p->eps(1);
  ep2=p->eps(2);

  double fac=(p4[1]+p4[2]).mass2()-4*p4[1].mass()*p4[2].mass();

  vertex(0,0,fac*(ep0*v->epsParent(0).conj()));
  vertex(0,1,fac*(ep0*v->epsParent(1).conj()));
  vertex(0,2,fac*(ep0*v->epsParent(2).conj()));
  
  vertex(1,0,fac*(ep1*v->epsParent(0).conj()));
  vertex(1,1,fac*(ep1*v->epsParent(1).conj()));
  vertex(1,2,fac*(ep1*v->epsParent(2).conj()));
  
  vertex(2,0,fac*(ep2*v->epsParent(0).conj()));
  vertex(2,1,fac*(ep2*v->epsParent(1).conj()));
  vertex(2,2,fac*(ep2*v->epsParent(2).conj()));

  return ;

}




