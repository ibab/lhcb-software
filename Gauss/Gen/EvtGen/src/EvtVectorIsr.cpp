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
// Module: EvtVectorIsr.cc
//
// Description: Model to simulate e+e- -> vector + gamma
//              This is implemented as a decay of the VPHO.
//
// Modification history:
//
//    RYD/Adriano     June 16, 1998         Module created
//
//------------------------------------------------------------------------
//
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtRandom.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtVectorIsr.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtConst.hh"
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtVector4C.hh"

EvtVectorIsr::~EvtVectorIsr() {}

void EvtVectorIsr::getName(EvtString& model_name){

  model_name="VECTORISR";     

}

EvtDecayBase* EvtVectorIsr::clone(){

  return new EvtVectorIsr;

}

void EvtVectorIsr::init(){

  // check that there are 2 arguments
  checkNArg(2);
  checkNDaug(2);

  checkSpinParent(EvtSpinType::VECTOR);
  checkSpinDaughter(0,EvtSpinType::VECTOR);
  checkSpinDaughter(1,EvtSpinType::PHOTON);

  //copy the arguments into eaiser to remember names

  csfrmn=getArg(0);
  csbkmn=getArg(1);

}

void EvtVectorIsr::initProbMax(){

  noProbMax();

}

void EvtVectorIsr::decay( EvtParticle *p ){

  EvtParticle *phi;
  EvtParticle *gamma;


  //get pointers to the daughters set
  p->makeDaughters(getNDaug(),getDaugs());
  phi=p->getDaug(0);
  gamma=p->getDaug(1);

  double mass[2];
  findMasses(p,getNDaug(),getDaugs(),mass);

  double wcm=p->mass();

  //gamma momentum in the parents restframe
  double pg=(wcm*wcm-mass[0]*mass[0])/(2*wcm);

  int kcs;
  double csmn;

  //warning this does not do quite the right thing since I
  //have ignored the weight for how ofthen to generate forward
  //vs. backward going photons's
  if (EvtRandom::Flat(0.0,1.0)>0.5) {
    kcs=1;
    csmn=csfrmn;
  }
  else{
    kcs=-1;
    csmn=csbkmn;
  }

  //generate kinematics according to Bonneau-Martin article
  //Nucl. Phys. B27 (1971) 381-397
  double y=pow((1+csmn)/(1-csmn),EvtRandom::Flat(0.0,1.0));
  double cs=kcs*(y-1)/(y+1);
  double sn=sqrt(1-cs*cs);

  double fi=EvtRandom::Flat(EvtConst::twoPi);

  //four-vector for the phi
  EvtVector4R p4phi(sqrt(mass[0]*mass[0]+pg*pg),pg*sn*cos(fi),
		 pg*sn*sin(fi),pg*cs);

  EvtVector4R p4gamma(pg,-p4phi.get(1),-p4phi.get(2),-p4phi.get(3));

  //save momenta for particles
  phi->init( getDaug(0),p4phi);

  gamma->init( getDaug(1),p4gamma);

  //try setting the spin density matrix of the phi


  //first get the polarization vectors of the gamma and phi

  EvtVector4C phi0=phi->epsParent(0);
  EvtVector4C phi1=phi->epsParent(1);
  EvtVector4C phi2=phi->epsParent(2);

  EvtVector4C gamma0=gamma->epsParentPhoton(0);
  EvtVector4C gamma1=gamma->epsParentPhoton(1);

  EvtComplex r1p=phi0*gamma0;
  EvtComplex r2p=phi1*gamma0;
  EvtComplex r3p=phi2*gamma0;


  EvtComplex r1m=phi0*gamma1;
  EvtComplex r2m=phi1*gamma1;
  EvtComplex r3m=phi2*gamma1;

  EvtComplex rho33=r3p*conj(r3p)+r3m*conj(r3m);
  EvtComplex rho22=r2p*conj(r2p)+r2m*conj(r2m);
  EvtComplex rho11=r1p*conj(r1p)+r1m*conj(r1m);

  EvtComplex rho13=r3p*conj(r1p)+r3m*conj(r1m);
  EvtComplex rho12=r2p*conj(r1p)+r2m*conj(r1m);
  EvtComplex rho23=r3p*conj(r2p)+r3m*conj(r2m);

  EvtComplex rho31=conj(rho13);
  EvtComplex rho32=conj(rho23);
  EvtComplex rho21=conj(rho12);


  EvtSpinDensity rho;
  rho.SetDim(3);

  rho.Set(0,0,rho11);
  rho.Set(0,1,rho12);
  rho.Set(0,2,rho13);
  rho.Set(1,0,rho21);
  rho.Set(1,1,rho22);
  rho.Set(1,2,rho23);
  rho.Set(2,0,rho31);
  rho.Set(2,1,rho32);
  rho.Set(2,2,rho33);


  setDaughterSpinDensity(0);
  phi->setSpinDensityForward(rho);


  return ;
}

