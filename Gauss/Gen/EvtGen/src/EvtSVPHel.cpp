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
// Module: EvtSVPHel.cc
//
// Description: Routine to handle decays like S -> V P
//              where the amplitude is given using the
//              helicity basis.
//
// Modification history:
//
//    RYD       July 26, 1997       Module created
//
//------------------------------------------------------------------------
// 
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtDecayBase.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtTensor4C.hh"
#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtAmp.hh"


void EvtSVPHel(EvtParticle *parent,EvtAmp& amp,EvtId n_v,EvtId n_ph,
	       const EvtComplex& hp, const EvtComplex& hm){

  //  Routine to decay a vector into a vector and scalar.  Started
  //  by ryd on Oct 17, 1996.

      
  // This routine is adopted from EvtSVVHel and since there is
  // a photon that can not have helicity 0 this is put in by 
  // setting the h0 amplitude to 0.
  EvtComplex h0=EvtComplex(0.0,0.0);

  EvtParticle *v1,*ph;
  EvtId daug[2];
  daug[0]=n_v;
  daug[1]=n_ph;

  parent->makeDaughters(2,daug);
  v1=parent->getDaug(0);
  ph=parent->getDaug(1);

  double m_parent,mass[2];
  int n_daug;
  EvtVector4R p4[2];

// Prepare for phase space routine. 

  m_parent = parent->mass();
  n_daug = 2;

  EvtDecayBase::findMasses(parent,n_daug,daug,mass);

//  Need phase space random numbers

  EvtGenKine::PhaseSpace( n_daug, mass, p4, m_parent );

//  Put phase space results into the daughters.
 
  v1->init( n_v, p4[0] );
  ph->init( n_ph, p4[1] );

  EvtTensor4C d,g;

  g.setdiag(1.0,-1.0,-1.0,-1.0);

  EvtVector4R v,vp;

  v=p4[0]/p4[0].d3mag();
  vp=(p4[0]+p4[1])/(p4[0]+p4[1]).mass();   

  d=((1.0/sqrt(3.0))*(h0-(hp+hm))*(-1.0/sqrt(3.0)))*g+
    ((1.0/sqrt(2.0))*(hp-hm)*EvtComplex(0.0,1.0)*(sqrt(1.0/2.0)))*dual(directProd(v,vp))+
    (sqrt(2.0/3.0)*(h0+0.5*(hp+hm))*sqrt(3.0/2.0))*(directProd(v,v)+(1.0/3.0)*g);

  EvtVector4C ep0,ep1,ep2;  
  
  ep0=d.cont1(v1->eps(0).conj());
  ep1=d.cont1(v1->eps(1).conj());
  ep2=d.cont1(v1->eps(2).conj());

  EvtVector4C ep20,ep21,ep22;

  ep20=ph->epsParentPhoton(0).conj();  
  ep21=ph->epsParentPhoton(1).conj();  

  amp.vertex(0,0,ep0*ep20);
  amp.vertex(0,1,ep0*ep21);
  
  amp.vertex(1,0,ep1*ep20);
  amp.vertex(1,1,ep1*ep21);
   
  amp.vertex(2,0,ep2*ep20);
  amp.vertex(2,1,ep2*ep21);

  return ;
}

