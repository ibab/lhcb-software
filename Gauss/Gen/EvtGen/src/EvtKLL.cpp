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
// Module: EvtKLL.cc
//
// Description: B -> KLL (L is a mu or a e)
//      usage :
//               BrFr   K   e+   e-   KLL3    (K can be charged or neutral)
//               BrFr   K   mu+  mu-  KLL2     
//
// Modification history:
//
//   M. Rotondo     January 19, 1998        Module created
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
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtKsLLLiCo.hh"
#include "EvtGen/EvtKLL.hh"

EvtKLL::~EvtKLL() {}

void EvtKLL::getName(EvtString& model_name){

    model_name="KLL3P";

}

EvtDecayBase* EvtKLL::clone(){

  return new EvtKLL;

}

void EvtKLL::init(){

  // check that there are 0 arguments
  checkNArg(0);
  checkNDaug(3);

  //We expect the parent to be a scalar 
  //and the daughters to be K lepton lepton

  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(0,EvtSpinType::SCALAR);
  checkSpinDaughter(1,EvtSpinType::DIRAC);
  checkSpinDaughter(2,EvtSpinType::DIRAC);



}



void EvtKLL::decay( EvtParticle *pb){

  pb->initializePhaseSpace(getNDaug(),getDaugs());
 
  EvtParticle *leptonp, *leptonn;
  leptonp = pb->getDaug(1);
  leptonn = pb->getDaug(2);
  
  //report(INFO,"EvtGen")<<"ERvt Gen: inizio !!!!!!!"<<endl;
  
  double m_b = pb->mass();
  
  EvtVector4R pbmeson;
  pbmeson.set(pb->mass(),0.0,0.0,0.0);
  EvtVector4R q = leptonp->getP4() + leptonn->getP4();
  EvtVector4R pk = pb->getDaug(0)->getP4(); 

  // Costants and invariant (the parameters are in Agreement with the 
  // Colangelo paper.
  double mb = 4.8; 
  double mc = 1.35; 
  double q2 = q.mass2();
  double mk = pb->getDaug(0)->mass();
  double srid  = q2/mb; 
  double mcrid = mc/mb;

  double c1 = -0.243;
  double c2 = 1.105;
  double c7 = -0.312;
  double c10 = -4.578;
  double c9sm = 4.193;

  // oneLoopFourQ2 is a static function (!) defined in the class EvtKstar2
  // so I 
  EvtComplex c9eff=EvtComplex(c9sm,0.0)+
    (EvtKsLLLC::oneLoopFourQ2(mcrid,srid))*EvtComplex((3.0*c1+c2),0.0);
  
  double F0 = 0.25/(1.0-q2/49.0); 
  double F1 = 0.25/(1.0-q2/25.0);
  double Ft = -0.14/((1.0-q2/25.0)*(1.0-q2/49.0));
  
  EvtVector4C MatHadT=(Ft/(m_b+mk))*(q2*(pbmeson+pk)-(m_b*m_b-mk*mk)*q);
  EvtVector4C MatHadV=(pbmeson+pk)*F1 + (F0-F1)*(m_b*m_b-mk*mk)*(1.0/q2)*q;
  
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

  if (q2>0.08){
    vertex(0,0,c9eff*MatHadV*lv11+EvtComplex(c10,0.0)*MatHadV*la11-
	   EvtComplex(2.0*c7*mb/q2,0.0)*MatHadT*lv11);
    vertex(0,1,c9eff*MatHadV*lv12+EvtComplex(c10,0.0)*MatHadV*la12-
	   EvtComplex(2.0*c7*mb/q2,0.0)*MatHadT*lv12);
    vertex(1,0,c9eff*MatHadV*lv21+EvtComplex(c10,0.0)*MatHadV*la21-
	   EvtComplex(2.0*c7*mb/q2,0.0)*MatHadT*lv21);
    vertex(1,1,c9eff*MatHadV*lv22+EvtComplex(c10,0.0)*MatHadV*la22-
	   EvtComplex(2.0*c7*mb/q2,0.0)*MatHadT*lv22);
  }
  else{
    vertex(0,0,0.0);
    vertex(1,0,0.0);
    vertex(0,1,0.0);
    vertex(1,1,0.0);
  }

  return;

}
