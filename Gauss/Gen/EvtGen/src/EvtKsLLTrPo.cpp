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
// Module: EvtKsLLTrPo.cc
//
// Description: B -> K*LL 
//              The symbols used for the form factors and the costants
//              (when possible) in agreement with the Colangelo paper.
//              Usage:
//               BrFr K*   e+   e-   KSLL3PQCD;
//               BrFr K*   mu+  mu-  KSLL2PQCD;       
//
// Implementation:   
//   Based on :  hep-ph/9510403v2 (P. Colangelo et. al.)
//   The matrix element of the eff. Hamiltonian responsible of the decay 
//   B->K* are calculated in the framework of the 3-point QCD sum rules.
//
//   WARNING:Only Short Distance contribute are considered. 
//
// Modification history:
//
//    M. Rotondo     January 16, 1998        Module created
//
//------------------------------------------------------------------------
//
#include <stdlib.h>
#include <stdio.h>
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtTensor4C.hh"
#include "EvtGen/EvtDiracSpinor.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtKsLLLiCo.hh"
#include "EvtGen/EvtKsLLTrPo.hh"

EvtKsLLTrPo::~EvtKsLLTrPo() {}

void EvtKsLLTrPo::getName(EvtString& model_name){

    model_name="KSLL3PQCD";

}

void EvtKsLLTrPo::initProbMax() {

  setProbMax(150000.0);

}      


EvtDecayBase* EvtKsLLTrPo::clone(){

  return new EvtKsLLTrPo;

}

void EvtKsLLTrPo::init(){

  // check that there are 0 arguments
  checkNArg(0);
  checkNDaug(3);

  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(0,EvtSpinType::VECTOR);
  checkSpinDaughter(1,EvtSpinType::DIRAC);
  checkSpinDaughter(2,EvtSpinType::DIRAC);

}


void EvtKsLLTrPo::decay( EvtParticle *pb){

  EvtParticle *kstar, *leptonp, *leptonn;
  
  // report(INFO,"EvtGen")<<"ERvt Gen: inizio !!!!!!!"<<endl;
  
  pb->makeDaughters(getNDaug(),getDaugs());
  kstar=pb->getDaug(0);
  leptonp=pb->getDaug(1);
  leptonn=pb->getDaug(2);
  
  EvtVector4R p4[3];
  
  double mass[3];
  
  double m_b = pb->mass();
 
  findMasses( pb, getNDaug(), getDaugs(), mass );

   // the next four lines generates events with a weight such that
  // the efficiency for selecting them is good. The parameter below of
  // 0.1 is the size of the peak at low q^2 (in arbitrary units).
  // when you use this remember to remove the cut on q^2!

  //let's remove the pole for now.... (ryd 99-12-10)

  do{
    EvtVector4R p4temp[3];
    setWeight(EvtGenKine::PhaseSpacePole( m_b, mass[1],mass[2],mass[0],0.0, p4temp ));
    p4[0]=p4temp[2];
    p4[1]=p4temp[0];
    p4[2]=p4temp[1];
  }while((p4[1]+p4[2]).mass2()<0.1);


  //this is the old scheme that generates uniform events.
  //coment out line below if using the 4 lines above!

  //EvtGenKine::PhaseSpace( getNDaug(), mass, p4, m_b );

  
  kstar->init( getDaug(0), p4[0] );
  leptonp->init( getDaug(1), p4[1]);
  leptonn->init( getDaug(2), p4[2]);
  
  EvtVector4R q,p,pk,pbmeson; 
  pbmeson.set(pb->mass(),0.0,0.0,0.0);
  
  q = p4[1] + p4[2];     
  p = p4[0] + pbmeson;
  pk = p4[0]; 
  
  double q2 = q.mass2();

  double mks = mass[0];
  
  // costants, form-factor and Wilson-coefficient (Colangelo paper)
  //EvtComplex A, B1, B2, B3;
  //double C, D1, D2, D3,F1, F2;
  double V, A0, A1, A2, A3, T1, T2, T3;
  // ... input parameters of the Wilson coefficient used ... 
  double c1 = -0.243;
  double c2 = 1.105;
  //double c3 = 1.083 * 0.01;
  //double c4 = -2.514 * 0.01;
  //double c5 = 7.266 * 0.001;
  //double c6 = -3.063 * 0.01;
  double c7 = -0.312;
  double c10 = -4.578;
  double c9sm = 4.193;
  //
  double mb = 4.8;  // quark b mass
  double mc = 1.35; // quark c mass
  //double mq = 0.2;  // quark s mass 
  
  // srid  : dilepton mass square reducend
  // mcrid : c quark mass reduced
  // msrid : s quark mass reduced
  double srid  = q2/mb; 
  double mcrid = mc/mb;
  //
  EvtComplex c9eff=EvtComplex(c9sm,0.0)+
    (EvtKsLLLC::oneLoopFourQ2(mcrid,srid))*EvtComplex((3.0*c1+c2),0.0);
  
  //EvtComplex c9eff = EvtComplex(c9sm,0.0) + 0.124 * EvtComplex(omega2(srid),0.0)+ oneLoopFourQ2(mcrid , srid) * ( 3.0*c1 + c2 + 3.0*c3 + c4 + 3.0*c5 + c6) - 0.5 * oneLoopFourQ2(msrid , srid) * (c3 + 3.0*c4) - 0.5 * oneLoopFourQ2(1.0,srid)*(4.0*c3 + 4.0*c4 + 3.0*c5 + c6) + (2.0/9.0)*EvtComplex(( 3.0*c3 + c4 + 3.0*c5 + c6),0.0);
  
  
  //...form factors...calculated in the framework of QCD sum rules ...
  V  = 0.47/(1.0 - q2 / 25.0 );
  A0 = 0.30/(1.0 - q2 / (4.8*4.8));
  A1 = 0.37*(1.0 - 0.023*q2);
  A2 = 0.40*(1.0 + 0.034*q2);
  A3 = A1*(m_b+mks)/(2*mks)-(m_b-mks)/(2*mks)*A2;
  T1 = 0.19/(1.0 - q2 /(5.3*5.3));
  T2 = 0.19*(1.0 - 0.02*q2);
  T3 = -0.7*(1.0+0.005*q2) ;
  
  
  EvtTensor4C g_metric;
  g_metric.setdiag(1.0,-1.0,-1.0,-1.0);
  
  EvtTensor4C MatHadVA, MatHadTs;
  
  MatHadVA=EvtComplex((2.0*V/(m_b+mks)),0.0)*dual(directProd(pk,pbmeson))-
    EvtComplex(0.0,((m_b+mks)*A1))*g_metric+
    EvtComplex(0.0,(A2/(m_b+mks)))*EvtTensor4C(directProd((pk + pbmeson),q))+
    EvtComplex(0.0,(2.0*mks*(A3-A0)/q2))*EvtTensor4C(directProd(q,q));
  
  MatHadTs=EvtComplex(-4.0*T1,0.0)*dual(directProd(pk,pbmeson))+
    EvtComplex(0.0,2.0*T2*(m_b*m_b-mks*mks))*g_metric-
    EvtComplex(0.0,(2.0*T2))*directProd((pk + pbmeson),q)+
    EvtComplex(0.0,2.0*T3)*directProd(q,q)-     
    EvtComplex(0.0,(2.0*q2*T3/(m_b*m_b-mks*mks)))*directProd((pk + pbmeson),q); 
           
  EvtVector4C et0VA = MatHadVA.cont2(kstar->epsParent(0).conj());
  EvtVector4C et1VA = MatHadVA.cont2(kstar->epsParent(1).conj());
  EvtVector4C et2VA = MatHadVA.cont2(kstar->epsParent(2).conj());
  
  EvtVector4C et0Ts = MatHadTs.cont2(kstar->epsParent(0).conj());
  EvtVector4C et1Ts = MatHadTs.cont2(kstar->epsParent(1).conj());
  EvtVector4C et2Ts = MatHadTs.cont2(kstar->epsParent(2).conj());  
  
  // ... leptonic current ...
  EvtVector4C lv11, lv12, lv21, lv22;
  EvtVector4C la11, la12, la21, la22;
  
  lv11=EvtLeptonVCurrent(leptonp->spParent(0),leptonn->spParent(0));
  lv12=EvtLeptonVCurrent(leptonp->spParent(0),leptonn->spParent(1));
  lv21=EvtLeptonVCurrent(leptonp->spParent(1),leptonn->spParent(0));
  lv22=EvtLeptonVCurrent(leptonp->spParent(1),leptonn->spParent(1));
  
  la11=EvtLeptonACurrent(leptonp->spParent(0),leptonn->spParent(0));
  la12=EvtLeptonACurrent(leptonp->spParent(0),leptonn->spParent(1));
  la21=EvtLeptonACurrent(leptonp->spParent(1),leptonn->spParent(0));
  la22=EvtLeptonACurrent(leptonp->spParent(1),leptonn->spParent(1));

  vertex(0,0,0,c9eff*(lv11.cont(et0VA))+EvtComplex(c10,0.0)*(la11.cont(et0VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv11.cont(et0Ts)));
  vertex(0,0,1,c9eff*(lv12.cont(et0VA))+EvtComplex(c10,0.0)*(la12.cont(et0VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv12.cont(et0Ts)));
  vertex(0,1,0,c9eff*(lv21.cont(et0VA))+EvtComplex(c10,0.0)*(la21.cont(et0VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv21.cont(et0Ts)));
  vertex(0,1,1,c9eff*(lv22.cont(et0VA))+EvtComplex(c10,0.0)*(la22.cont(et0VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv22.cont(et0Ts)));
  
  vertex(1,0,0,c9eff*(lv11.cont(et1VA))+EvtComplex(c10,0.0)*(la11.cont(et1VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv11.cont(et1Ts)));
  vertex(1,0,1,c9eff*(lv12.cont(et1VA))+EvtComplex(c10,0.0)*(la12.cont(et1VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv12.cont(et1Ts)));
  vertex(1,1,0,c9eff*(lv21.cont(et1VA))+EvtComplex(c10,0.0)*(la21.cont(et1VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv21.cont(et1Ts)));
  vertex(1,1,1,c9eff*(lv22.cont(et1VA))+EvtComplex(c10,0.0)*(la22.cont(et1VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv22.cont(et1Ts)));
 
  vertex(2,0,0,c9eff*(lv11.cont(et2VA))+EvtComplex(c10,0.0)*(la11.cont(et2VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv11.cont(et2Ts)));
  vertex(2,0,1,c9eff*(lv12.cont(et2VA))+EvtComplex(c10,0.0)*(la12.cont(et2VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv12.cont(et2Ts)));
  vertex(2,1,0,c9eff*(lv21.cont(et2VA))+EvtComplex(c10,0.0)*(la21.cont(et2VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv21.cont(et2Ts)));
  vertex(2,1,1,c9eff*(lv22.cont(et2VA))+EvtComplex(c10,0.0)*(la22.cont(et2VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv22.cont(et2Ts)));

  return;

}



