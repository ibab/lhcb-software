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
//    Joe Izen        Dec  16, 2002         Fix cos_theta distribution - prevents boom at cos_theta=+/-1 
//    RYD/Adriano     June 16, 1998         Module created
//
//------------------------------------------------------------------------
//
#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenModels/EvtVectorIsr.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtConst.hh"
#include <string>
#include "EvtGenBase/EvtVector4C.hh"

EvtVectorIsr::~EvtVectorIsr() {}

void EvtVectorIsr::getName(std::string& model_name){

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

  //the elctron mass
  double electMass=EvtPDL::getMeanMass(EvtPDL::getId("e-"));


  //get pointers to the daughters set
  //get masses/initial phase space - will overwrite the
  //p4s below to get the kinematic distributions correct
  p->initializePhaseSpace(getNDaug(),getDaugs());
  phi=p->getDaug(0);
  gamma=p->getDaug(1);

  double wcm=p->mass();
  double beta=2.*electMass/wcm; //electMass/Ebeam = betagamma
  beta=sqrt(1. - beta*beta);   //sqrt (1 - (m/ebeam)**2)

  //gamma momentum in the parents restframe
  double pg=(wcm*wcm-phi->mass()*phi->mass())/(2*wcm);

//    //generate kinematics according to Bonneau-Martin article
//    //Nucl. Phys. B27 (1971) 381-397

//    double y=pow((1+csmn)/(1-csmn),EvtRandom::Flat(0.0,1.0));
//    double cs=kcs*(y-1)/(y+1);

  // For backward compatibility with .dec files before SP5, the backward cos limit for
  //the ISR photon is actually given as *minus* the actual limit. Sorry, this wouldn't be
  //my choice.  -Joe

  double ymax=log((1.+beta*csfrmn)/(1.-beta*csfrmn));
  double ymin=log((1.-beta*csbkmn)/(1.+beta*csbkmn));

  // photon theta distributed as  2*beta/(1-beta**2*cos(theta)**2)
  double y=(ymax-ymin)*EvtRandom::Flat(0.0,1.0) + ymin;
  double cs=exp(y);
  cs=(cs - 1.)/(cs + 1.)/beta;
  double sn=sqrt(1-cs*cs);

  double fi=EvtRandom::Flat(EvtConst::twoPi);

  //four-vector for the phi
  EvtVector4R p4phi(sqrt(phi->mass()*phi->mass()+pg*pg),pg*sn*cos(fi),
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

