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
// Module: EvtKsLLLiCo.cc
//
// Description: B -> K*LL 
//              The symbols used for the form factors and the costants
//              (when possible) in agreement with the Aliev paper.
//              Usage:
//               BrFr  K*   e+    e-    KSLLLCQCD;
//               BrFr  K*   mu+   mu-   KSLLLCQCD; 
//               BrFr  K*   tau+  tau-  KSLLLCQCD;         
//
// Implementation:   
//   Based on :  hep-ph/9704323v2 (T.M.Aliev et. al.)
//   The matrix element of the eff. Hamiltonian responsible of the decay 
//   B->K* are calculated in the framework of the light-cone QCD sum rules.
//   WARNING 1: in the Aliev's paper are taken in account the leptons mass term
//   to explore the effect of the 2HDM (two Higgs doublet model) on the 
//   relevant distribution. 
//   WARNING 2:Only Short Distance contribute are considered. 
//
//   more detail in the code !!
//
// Modification history:
//
//    M. Rotondo     January 8, 1998        Module created
//
//------------------------------------------------------------------------
//
#include <stdlib.h>
#include <fstream.h>
#include <stdio.h>
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtDiracSpinor.hh"
#include "EvtGen/EvtTensor4C.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtKsLLLiCo.hh"
#include "EvtGen/EvtConst.hh"

#ifdef WIN32
extern "C" {
  extern double __stdcall LI2SPENCE(double*);
}
#else
extern "C" {
  extern double li2spence_(double*);
}
#endif

EvtKsLLLC::~EvtKsLLLC() {}

void EvtKsLLLC::getName(EvtString& model_name){

    model_name="KSLLLCQCD";

}


EvtDecayBase* EvtKsLLLC::clone(){

  return new EvtKsLLLC;

}

void EvtKsLLLC::initProbMax() {

   setProbMax(500000.0);

}      


void EvtKsLLLC::init(){

  // check that there are 0 arguments
  checkNArg(0);
  checkNDaug(3);

  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(0,EvtSpinType::VECTOR);
  checkSpinDaughter(1,EvtSpinType::DIRAC);
  checkSpinDaughter(2,EvtSpinType::DIRAC);

}


void EvtKsLLLC::decay( EvtParticle *pb){

  EvtParticle *kstar, *leptonp, *leptonn;
  
  //report(INFO,"EvtGen")<<"ERvt Gen: inizio !!!!!!!"<<std::endl;
  
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
  // The value of 0.1 is appropriate for muons. 
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

  //
  // q       -> L+ L- pair momentum trasfered to the leptons
  // p       -> K* momentum
  // pbmeson -> B  momentum
  //

  EvtVector4R q,p,pbmeson; 
  pbmeson.set(pb->mass(),0.0,0.0,0.0);
  q = p4[1] + p4[2];       
  p = pbmeson - q;   
  
  //
  // The matrix element is written in function of two invariants:
  // q2  --> invariant mass squared of the leptons system
  // p2  --> mass of the K*
 
  double q2 = q.mass2();
  double mks = mass[0];

  // costants, form-factor and Wilson-coefficient (Aliev paper)
  //  EvtComplex A, B1, B2, B3;
  //double C, D1, D2, D3, F1, F2;
  double V, A0, A1, A2, A3, T1, T2, T3;
  // ... input parameters of the Wilson coefficient used ... 
  double c1 = -0.249;
  double c2 = 1.108;
  double c3 = 1.112 * 0.01;
  double c4 = -2.569 * 0.01;
  double c5 = 7.4 * 0.001;
  double c6 = -3.144 * 0.01;
  double c7 = -0.315;
  double c10 = -4.642;
  double c9sm = 4.227;
  //
  double mb = 4.8;  // quark b mass
  double mc = 1.35; // quark c mass
  double mq = 0.2;  // quark s mass 

  // some reduced quantity:
  double srid  = q2/mb; 
  double mcrid = mc/mb;
  double msrid = mq/mb;
  
  // Expression of c9eff in the NLO approximation:
  // The function omega2 and oneLoopFourQ2 are static (!) member function of 
  // the class the EvtKsLLLC. 
  EvtComplex c9eff = EvtComplex(c9sm,0.0) + 0.124 * EvtComplex(omega2(srid/mb),0.0)+ 
    oneLoopFourQ2(mcrid , srid) * ( 3.0*c1 + c2 + 3.0*c3 + c4 +
				    3.0*c5 + c6) - 0.5 * oneLoopFourQ2(msrid , srid) * (c3 + 3.0*c4)-
    0.5 * oneLoopFourQ2(1.0,srid)*(4.0*c3 + 4.0*c4 + 3.0*c5 + c6) +
    (2.0/9.0)*EvtComplex(( 3.0*c3 + c4 + 3.0*c5 + c6),0.0);
  
  //...form factors...calculated in the framework of light-cone QCD sum rules ...
  V  = 0.55 / (( 1.0 - q2 / 30.0 )*( 1.0 - q2 / 30.0 ));
  A0 = 0.26 / (( 1.0 - q2 / 45.0 )* ( 1.0 - q2 / 45.0 ));
  A1 = 0.36 / (( 1.0 - q2 / 64.0 )*( 1.0 - q2 / 64.0 ));
  A2 = 0.40 / (( 1.0 - q2 / 33.0 )*( 1.0 - q2 / 33.0 ));
  A3 = A1*(m_b+mks)/(2*mks)-(m_b-mks)/(2*mks)*A2;
  T1 = 0.18 / (( 1.0 - q2 / 31.0 )*( 1.0 - q2 / 31.0 ));
  T2 = 0.18 / (( 1.0 - q2 / 64.4 )*( 1.0 - q2 / 64.4 ));
  T3 = 0.14 / (( 1.0 - q2 / 32.5 )*( 1.0 - q2 / 32.5 ));

  //
  // Expression of the hadronic matrix elements 
  EvtTensor4C g_metric;
  g_metric.setdiag(1.0,-1.0,-1.0,-1.0);

  EvtTensor4C MatHadVA, MatHadTs;

  MatHadVA=EvtComplex((2.0*V/(m_b+mks)),0.0)*dual(directProd(p,q))-
    EvtComplex(0.0,((m_b+mks)*A1))*g_metric+
    EvtComplex(0.0,(A2/(m_b+mks)))*EvtTensor4C(directProd((2.0*p + q),q))+
    EvtComplex(0.0,(2.0*mks*(A3-A0)/q2))*EvtTensor4C(directProd(q,q));
  
  MatHadTs=EvtComplex(-4.0*T1,0.0)*dual(directProd(p,q))+
    EvtComplex(0.0,2.0*T2*(m_b*m_b-mks*mks))*g_metric-
    EvtComplex(0.0,(2.0*T2))*directProd((2.0*p + q),q)+
    EvtComplex(0.0,2.0*T3)*directProd(q,q)-
    EvtComplex(0.0,(2.0*q2*T3/(m_b*m_b-mks*mks)))*directProd((2.0*p+q),q); 
  
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

  //
  // This cut is to avoid the left in efficiency (large time to compute an event
  // in agreement ) due to the very 
// higt peak near the 0 q2. The matrix element is zero for q2 < 0.08 Gev^2.
  // 

  EvtComplex tmp;

  
  tmp=c9eff*(lv11.cont(et0VA))+EvtComplex(c10,0.0)*(la11.cont(et0VA))-
    EvtComplex((2.0*c7*mb/q2),0.0)*(lv11.cont(et0Ts));
  //cout <<"tmp000:"<<tmp<<endl;
  vertex(0,0,0,tmp);

  tmp=c9eff*(lv12.cont(et0VA))+EvtComplex(c10,0.0)*(la12.cont(et0VA))-
    EvtComplex((2.0*c7*mb/q2),0.0)*(lv12.cont(et0Ts));
  //cout <<"tmp001:"<<tmp<<endl;
  vertex(0,0,1,tmp);

  tmp=c9eff*(lv21.cont(et0VA))+EvtComplex(c10,0.0)*(la21.cont(et0VA))-
    EvtComplex((2.0*c7*mb/q2),0.0)*(lv21.cont(et0Ts));
  //cout <<"tmp010:"<<tmp<<endl;
  vertex(0,1,0,tmp);

  tmp=c9eff*(lv22.cont(et0VA))+EvtComplex(c10,0.0)*(la22.cont(et0VA))-
    EvtComplex((2.0*c7*mb/q2),0.0)*(lv22.cont(et0Ts));
  //cout <<"tmp011:"<<tmp<<endl;
  vertex(0,1,1,tmp);

  tmp=c9eff*(lv11.cont(et1VA))+EvtComplex(c10,0.0)*(la11.cont(et1VA))-
    EvtComplex((2.0*c7*mb/q2),0.0)*(lv11.cont(et1Ts));
  //cout <<"tmp100:"<<tmp<<endl;
  vertex(1,0,0,tmp);

  tmp=c9eff*(lv12.cont(et1VA))+EvtComplex(c10,0.0)*(la12.cont(et1VA))-
    EvtComplex((2.0*c7*mb/q2),0.0)*(lv12.cont(et1Ts));
  //cout <<"tmp101:"<<tmp<<endl;
  vertex(1,0,1,tmp);

  tmp=c9eff*(lv21.cont(et1VA))+EvtComplex(c10,0.0)*(la21.cont(et1VA))-
    EvtComplex((2.0*c7*mb/q2),0.0)*(lv21.cont(et1Ts));
  //cout <<"tmp110:"<<tmp<<endl;
  vertex(1,1,0,tmp);

  tmp=c9eff*(lv22.cont(et1VA))+EvtComplex(c10,0.0)*(la22.cont(et1VA))-
    EvtComplex((2.0*c7*mb/q2),0.0)*(lv22.cont(et1Ts));
  //cout <<"tmp111:"<<tmp<<endl;
  vertex(1,1,1,tmp);
  
  tmp=c9eff*(lv11.cont(et2VA))+EvtComplex(c10,0.0)*(la11.cont(et2VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv11.cont(et2Ts));
  //cout <<"tmp200:"<<tmp<<endl;
  vertex(2,0,0,tmp);

  tmp=c9eff*(lv12.cont(et2VA))+EvtComplex(c10,0.0)*(la12.cont(et2VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv12.cont(et2Ts));
  //cout <<"tmp201:"<<tmp<<endl;
  vertex(2,0,1,tmp);

  tmp=c9eff*(lv21.cont(et2VA))+EvtComplex(c10,0.0)*(la21.cont(et2VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv21.cont(et2Ts));
  //cout <<"tmp210:"<<tmp<<endl;
  vertex(2,1,0,tmp);

  tmp=c9eff*(lv22.cont(et2VA))+EvtComplex(c10,0.0)*(la22.cont(et2VA))-
	 EvtComplex((2.0*c7*mb/q2),0.0)*(lv22.cont(et2Ts));
  //cout <<"tmp211:"<<tmp<<endl;
  vertex(2,1,1,tmp);

  return;

}

//////////////////////////////////////////////////////////////////////////
// The function oneLoop(x,s) arises from the one loop contributoin of
// the four quark operator O1 - O6. In particular in some approxmiation 
// g describes the loop of charm quarks.
    EvtComplex EvtKsLLLC::oneLoopFourQ2(double mqr,double sr) {
    double reG, imG;
    double x2 = mqr*mqr;
    double x2s = x2 / sr;
    if(sr < 4.0*x2) {
       reG = (4.0/9.0) * sqrt( 4.0*x2s - 1.0 ) * atan( sqrt(4.0*x2s - 1.0 ));
       imG = 0.0;      
    }
    if(sr > 4.0*x2) {
       reG = (2.0/9.0) * sqrt( 1.0 - 4.0*x2s )* 
       log( fabs(( 1.0+sqrt(1.0-4.0*x2s)/(1.0-sqrt(1.0-4.0*x2s )))));
       imG = -(2.0/9.0) * sqrt( 1.0 - 4.0*x2s ) * EvtConst::pi;     
    }

    //report(INFO,"EvtGen")<<"EvtGen in oneLoop reG -> "<<reG<<endl;
    //report(INFO,"EvtGen")<<"EvtGen in oneLoop imG -> "<<imG<<endl;

    EvtComplex gend= - EvtComplex((2.0+4.0*x2s)*reG+(4.0/9.0)*log(x2)-
                       (8.0/27.0)-16.0*x2s/9.0,(2.0+4.0*x2s)*imG);
    return gend;
    }

///////////////////////////////////////////////////////////////////////////
// The function omega(s) represent the O(alfa strong) correction from 
// the one-gluon exchange in the matrix element of O9
// li2spence is the real spencer function which is calculate in a Fortran Function 
    double EvtKsLLLC::omega2(double sr) {

       double o = -(2.0/9.0)*EvtConst::pi*EvtConst::pi-
#ifdef WIN32
	 (4.0/3.0)*LI2SPENCE(&sr)-
#else
	 (4.0/3.0)*li2spence_(&sr)-
#endif
           (2.0/3.0)*log(sr)*log(1.0-sr)-
           log(1.0-sr)*(5.0+4.0*sr)/(3.0+6.0*sr)-
           log(sr)*2.0*sr*(1.0+sr)*(1.0-2.0*sr)/(3.0*((1.0-sr)*(1.0-sr))*
           (1.0+2.0*sr))+(5.0+9.0*sr-6.0*sr*sr)/(6.0*(1.0-sr)*(1.0+2.0*sr));
       return o;
    }
//
