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
// Module: EvtTauScalarnu.cc
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
#include <iostream>
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtDiracParticle.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtIdSet.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtBsquark.hh"
#include "EvtGen/EvtDiracSpinor.hh"
#include "EvtGen/EvtGammaMatrix.hh"
#include "EvtGen/EvtReport.hh"

EvtBsquark::~EvtBsquark() {}

void EvtBsquark::getName(EvtString& model_name){

  model_name="BSQUARK";     

}


EvtDecayBase* EvtBsquark::clone(){

  return new EvtBsquark;

}

void EvtBsquark::init(){

  // check that there are 5 arguments
  checkNArg(5);
}

void EvtBsquark::initProbMax(){

  //For now do not set a maximum.

  //SetProbMax(0.000000000005);

}

void EvtBsquark::decay(EvtParticle *p){

  static EvtId cquark=EvtPDL::getId("c");
  static EvtId anticquark=EvtPDL::getId("anti-c");

  static EvtIdSet leptons("e-","mu-","tau-");

  p->initializePhaseSpace(getNDaug(),getDaugs());


  int charge=1;

  EvtParticle* lepton;
  lepton = p->getDaug(1);
  if (leptons.contains(lepton->getId())){
    charge=-1;
  }

  
  EvtDiracParticle charmquark;

  //this is a very crude approximation...
  if (charge==-1){
    charmquark.init(cquark,p->getDaug(0)->getP4());
  }
  else{
    charmquark.init(anticquark,p->getDaug(0)->getP4());
  }
    
  EvtVector4R p4c = p->getDaug(0)->getP4();

  EvtVector4R p4sn = p->getDaug(2)->getP4();

  EvtVector4R p4b(p->mass(),0.0,0.0,0.0);

  EvtComplex M[2][2];

  int il,ic;

  //project out the right handed current
  EvtGammaMatrix PR=0.5*(EvtGammaMatrix::id()+EvtGammaMatrix::g5());

  double tanbeta=getArg(1);
  double cosbeta=cos(atan(tanbeta));
  double sinbeta=sin(atan(tanbeta));

  double mb=4.9;
  double mc=1.3;
  double mw=80.4;

  double Mass=getArg(2);
  double mu=getArg(3);
  double mchargino=getArg(4);


  double tan2phim=2*sqrt(2.0)*mw*(mu*cosbeta+Mass*sinbeta)/
    (Mass*Mass-mu*mu+2*mw*mw*cos(2*atan(tanbeta)));

  double phim=0.5*atan(tan2phim);
  
  EvtComplex U11=cos(phim);
  EvtComplex U12=sin(phim);
  EvtComplex U21=-sin(phim);
  EvtComplex U22=cos(phim);

  double tan2phip=2*sqrt(2.0)*mw*(mu*cosbeta+Mass*sinbeta)/
    (Mass*Mass-mu*mu-2*mw*mw*cos(2*atan(tanbeta)));

  double phip=0.5*atan(tan2phip);

  EvtComplex V11=cos(phip);
  EvtComplex V12=sin(phip);
  EvtComplex V21=-sin(phip);
  EvtComplex V22=cos(phip);


  double theta=getArg(0);
  double ctheta=cos(theta);
  double stheta=sin(theta);


  //double FL=0.5;
  double vcsb=0.08;
  double mchi1=mchargino;  
  double mchi2=mchargino;

  //overall scale factor
  double g=1.0;

  EvtComplex a1=mchi1*(U11*ctheta-mb*U12*stheta/(sqrt(2.0)*mw*cosbeta));
  EvtComplex a2=mchi2*(U21*ctheta-mb*U22*stheta/(sqrt(2.0)*mw*cosbeta));
  
  EvtComplex b1=mc*conj(V12)*ctheta/(sqrt(2.0)*mw*sinbeta);
  EvtComplex b2=mc*conj(V22)*ctheta/(sqrt(2.0)*mw*sinbeta);

  EvtComplex f1=-(g*g*V11*vcsb)/((p4b-p4c).mass2()-mchi1*mchi1);
  EvtComplex f2=-(g*g*V21*vcsb)/((p4b-p4c).mass2()-mchi1*mchi2);

  //cout <<g<<" "<<V11<<" "<<FL<<" "<<vcsb<<" "<<mchi1<<std::endl;
  //cout << "f1:"<<f1<<" "<<(p4b-p4c).mass2()<<std::endl;
  //cout << "f2:"<<f2<<" "<<(p4b-p4c).mass2()<<std::endl;

  //cout << "p4sn:"<<p4sn<<endl;

  EvtGammaMatrix pslash=p4sn.get(0)*EvtGammaMatrix::g0()
                       -p4sn.get(1)*EvtGammaMatrix::g1()
                       -p4sn.get(2)*EvtGammaMatrix::g2()
                       -p4sn.get(3)*EvtGammaMatrix::g3();

  //cout << "pslash:"<<pslash<<endl;



  for(il=0;il<2;il++){
    for(ic=0;ic<2;ic++){

      EvtComplex a=0.0;
      EvtComplex b=0.0;

      if (charge==-1){
	a=charmquark.spParent(ic)*(PR*lepton->spParent(il));
	b=charmquark.spParent(ic)*((pslash*PR)*lepton->spParent(il));
      }
      else{
	a=lepton->spParent(il)*(PR*charmquark.spParent(ic));
	b=lepton->spParent(il)*((pslash*PR)*charmquark.spParent(ic));
      }

      //cout <<"pslash*PR:"<<pslash*PR<<endl;
      //cout <<"sp charm:"<<charmquark.spParent(ic)<<endl;
      //cout <<"sp lepton:"<<lepton->spParent(il)<<endl;

      M[ic][il]=f1*(a1*a+b1*b)+f2*(a2*a+b2*b);

      //cout << "Contr1:"<<a1<<" "<<a<<" "<<b1<<" "<<b<<endl;
      //cout << "Contr2:"<<a2<<" "<<a<<" "<<b2<<" "<<b<<endl;

      //cout <<"case1:"<<f1<<" "<<a1<<" "<<b1<<" "<<a<<" "<<b<<endl;
      //cout <<"case2:"<<f2<<" "<<a2<<" "<<b2<<" "<<a<<" "<<b<<endl;

    }
  }
 
  double prob=real(M[0][0]*conj(M[0][0])+
		   M[1][0]*conj(M[1][0])+
		   M[0][1]*conj(M[0][1])+
		   M[1][1]*conj(M[1][1]));

  //cout <<"prob:"<<prob<<endl;

  setProb(prob);

  return;

}

