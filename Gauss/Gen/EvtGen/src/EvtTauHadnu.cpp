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
// Module: EvtTauHadnu.cc
//
// Description: The leptonic decay of the tau meson.
//              E.g., tau- -> e- nueb nut
//
// Modification history:
//
//    RYD       January 17, 1997       Module created
//
//------------------------------------------------------------------------
//
#include <stdlib.h>
#include <iostream>
#include <string>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenModels/EvtTauHadnu.hh"
#include "EvtGenBase/EvtDiracSpinor.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtVector4C.hh"
#include "EvtGenBase/EvtIdSet.hh"

EvtTauHadnu::~EvtTauHadnu() {}

void EvtTauHadnu::getName(std::string& model_name){

  model_name="TAUHADNU";     

}


EvtDecayBase* EvtTauHadnu::clone(){

  return new EvtTauHadnu;

}

void EvtTauHadnu::init() {

  // check that there are 0 arguments

  checkSpinParent(EvtSpinType::DIRAC);

  //the last daughter should be a neutrino
  checkSpinDaughter(getNDaug()-1,EvtSpinType::NEUTRINO);

  int i;
  for ( i=0; i<(getNDaug()-1);i++) {
    checkSpinDaughter(i,EvtSpinType::SCALAR);
  }

  bool validndaug=false;

  if ( getNDaug()==4 ) {
    //pipinu
    validndaug=true;
    checkNArg(7);
    _beta = getArg(0);
    _mRho = getArg(1);
    _gammaRho = getArg(2);
    _mRhopr = getArg(3);
    _gammaRhopr = getArg(4);
    _mA1 = getArg(5);
    _gammaA1 = getArg(6);
  }
  if ( getNDaug()==3 ) {
    //pipinu
    validndaug=true;
    checkNArg(5);
    _beta = getArg(0);
    _mRho = getArg(1);
    _gammaRho = getArg(2);
    _mRhopr = getArg(3);
    _gammaRhopr = getArg(4);
  }
  if ( getNDaug()==2 ) {
    //pipinu
    validndaug=true;
    checkNArg(0);
  }

  if ( !validndaug ) {
    report(ERROR,"EvtGen") 
      << "Have not yet implemented this final state in TAUHADNU model" 
      << std::endl;
    report(ERROR,"EvtGen") << "Ndaug="<<getNDaug() << std::endl;
    int id;
    for ( id=0; id<(getNDaug()-1); id++ ) 
      report(ERROR,"EvtGen") << "Daug " << id << " "
                             <<EvtPDL::name(getDaug(id)).c_str() << std::endl;

  }

}

void EvtTauHadnu::initProbMax(){

  if ( getNDaug()==2 )  setProbMax(90.0);
  if ( getNDaug()==3 )  setProbMax(2500.0);
  if ( getNDaug()==4 )  setProbMax(5000.0);

}

void EvtTauHadnu::decay(EvtParticle *p){

  static EvtId TAUM=EvtPDL::getId("tau-");

  EvtIdSet thePis("pi+","pi-","pi0");
  EvtIdSet theKs("K+","K-");

  p->initializePhaseSpace(getNDaug(),getDaugs());
  
  EvtParticle *nut;
  nut = p->getDaug(getNDaug()-1);
  EvtVector4R momscalar = p->getDaug(0)->getP4();

  //get the leptonic current 
  EvtVector4C tau1, tau2;
  
  if (p->getId()==TAUM) {
    tau1=EvtLeptonVACurrent(nut->spParentNeutrino(),p->sp(0));
    tau2=EvtLeptonVACurrent(nut->spParentNeutrino(),p->sp(1));
  }
  else{
    tau1=EvtLeptonVACurrent(p->sp(0),nut->spParentNeutrino());
    tau2=EvtLeptonVACurrent(p->sp(1),nut->spParentNeutrino());
  }

  EvtVector4C hadCurr;
  bool foundHadCurr=false;
  if ( getNDaug() == 2 ) {
    hadCurr = p->getDaug(0)->getP4();
    foundHadCurr=true;
  }
  if ( getNDaug() == 3 ) {

    //pi pi nu with rho and rhopr resonance
    if ( thePis.contains(getDaug(0)) &&
	 thePis.contains(getDaug(1)) ) {

      EvtVector4R q1 = p->getDaug(0)->getP4();
      EvtVector4R q2 = p->getDaug(1)->getP4();
      double m1=p->getDaug(0)->mass();
      double m2=p->getDaug(1)->mass();
  
      hadCurr = Fpi(q1,q2)*(q1-q2);

      foundHadCurr = true;
    }

  }
  if ( getNDaug() == 4 ) {
    if ( thePis.contains(getDaug(0)) &&
	 thePis.contains(getDaug(1)) &&
	 thePis.contains(getDaug(2)) ) {
      foundHadCurr = true;
      //figure out which is the different charged pi
      //want it to be q3

      int diffPi(0),samePi1(0),samePi2(0);
      if ( getDaug(0) == getDaug(1) ) {diffPi=2; samePi1=0; samePi2=1;}
      if ( getDaug(0) == getDaug(2) ) {diffPi=1; samePi1=0; samePi2=2;}
      if ( getDaug(1) == getDaug(2) ) {diffPi=0; samePi1=1; samePi2=2;}

      EvtVector4R q1=p->getDaug(samePi1)->getP4();
      EvtVector4R q2=p->getDaug(samePi2)->getP4();
      EvtVector4R q3=p->getDaug(diffPi)->getP4();
      
      EvtVector4R Q=q1+q2+q3;
      double qMass2=Q.mass2();

      double GA1=_gammaA1*pi3G(Q.mass2(),samePi1)/pi3G(_mA1*_mA1,samePi1);

      EvtComplex denBA1(_mA1*_mA1 - Q.mass2(),-1.*_mA1*GA1);
      EvtComplex BA1 = _mA1*_mA1 / denBA1;

      hadCurr = BA1*( (q1-q3) - (Q*(Q*(q1-q3))/qMass2)*Fpi(q2,q3) +
		      (q2-q3) - (Q*(Q*(q2-q3))/qMass2)*Fpi(q1,q3) ); 


    }


  }



  if ( !foundHadCurr ) {
    report(ERROR,"EvtGen") 
      << "Have not yet implemented this final state in TAUHADNU model" 
      << std::endl;
    report(ERROR,"EvtGen") << "Ndaug="<<getNDaug() << std::endl;
    int id;
    for ( id=0; id<(getNDaug()-1); id++ ) 
      report(ERROR,"EvtGen") << "Daug " << id << " "
                             <<EvtPDL::name(getDaug(id)).c_str() << std::endl;

  }

  
  vertex(0,tau1*hadCurr);
  vertex(1,tau2*hadCurr);

  return;

}


double EvtTauHadnu::pi3G(double m2,int dupD) {
  double mPi= EvtPDL::getMeanMass(getDaug(dupD));
  if ( m2 > (_mRho+mPi) ) {
    return m2*(1.623 + 10.38/m2 - 9.32/(m2*m2) + 0.65/(m2*m2*m2));
  }
  else {
    double t1=m2-9.0*mPi*mPi;
    return 4.1*pow(t1,3.0)*(1.0 - 3.3*t1+5.8*t1*t1);
  }
  return 0.;
}

EvtComplex EvtTauHadnu::Fpi( EvtVector4R q1, EvtVector4R q2) {

  double m1=q1.mass();
  double m2=q2.mass();
  EvtVector4R q = q1 - q2;
  EvtVector4R Q = q1 + q2;
  double mQ2= Q*Q;
  
  // momenta in the rho->pipi decay
  double dRho= _mRho*_mRho - m1*m1 - m2*m2;
  double pPiRho = (1.0/_mRho)*sqrt((dRho*dRho)/4.0 - m1*m1*m2*m2);
  
  double dRhopr= _mRhopr*_mRhopr - m1*m1 - m2*m2;
  double pPiRhopr = (1.0/_mRhopr)*sqrt((dRhopr*dRhopr)/4.0 - m1*m1*m2*m2);
  
  double dQ= mQ2 - m1*m1 - m2*m2;
  double pPiQ = (1.0/sqrt(mQ2))*sqrt((dQ*dQ)/4.0 - m1*m1*m2*m2);
  
  
  double gammaRho = _gammaRho*_mRho/sqrt(mQ2)*pow((pPiQ/pPiRho),3);
  EvtComplex BRhoDem(_mRho*_mRho - mQ2,-1.0*_mRho*gammaRho);
  EvtComplex BRho= _mRho*_mRho / BRhoDem;
  
  double gammaRhopr = _gammaRhopr*_mRhopr/sqrt(mQ2)*pow((pPiQ/pPiRhopr),3);
  EvtComplex BRhoprDem(_mRhopr*_mRhopr - mQ2,-1.0*_mRho*gammaRhopr);
  EvtComplex BRhopr= _mRhopr*_mRhopr / BRhoprDem;
  
  return (BRho + _beta*BRhopr)/(1+_beta);
}





