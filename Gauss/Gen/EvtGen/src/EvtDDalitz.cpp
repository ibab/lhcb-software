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
// Module: EvtDDalitz.cc
//
// Description: Routine to handle three-body decays of D0/D0_bar or D+/D-
//
// Modification history:
//
//    NK     September 3, 1997         Module created
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtResonance.hh"
#include "EvtGen/EvtResonance2.hh"
#include "EvtGen/EvtDDalitz.hh"
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtConst.hh"

EvtDDalitz::~EvtDDalitz() {}

void EvtDDalitz::getName(EvtString& model_name){
  
  model_name="D_DALITZ";     

}


EvtDecayBase* EvtDDalitz::clone(){

  return new EvtDDalitz;

}

void EvtDDalitz::init(){

  // check that there are 0 arguments

  static EvtId DM=EvtPDL::getId("D-");
  static EvtId DP=EvtPDL::getId("D+");
  static EvtId D0=EvtPDL::getId("D0");
  static EvtId D0B=EvtPDL::getId("anti-D0");
  static EvtId KM=EvtPDL::getId("K-");
  static EvtId KP=EvtPDL::getId("K+");
  static EvtId K0=EvtPDL::getId("K0");
  static EvtId KB=EvtPDL::getId("anti-K0");
  static EvtId PIM=EvtPDL::getId("pi-");
  static EvtId PIP=EvtPDL::getId("pi+");
  static EvtId PI0=EvtPDL::getId("pi0");

  checkNArg(0);
  checkNDaug(3);

  checkSpinParent(EvtSpinType::SCALAR);

  checkSpinDaughter(0,EvtSpinType::SCALAR);
  checkSpinDaughter(1,EvtSpinType::SCALAR);
  checkSpinDaughter(2,EvtSpinType::SCALAR);

  EvtId parnum=getParentId();
  EvtId d1=getDaug(0);
  EvtId d2=getDaug(1);
  EvtId d3=getDaug(2);

  if ( parnum == D0 ) {
    //look for either a K- pi+ pi0 or K0bar pi+ pi-
    if ( d1==KM && d2==PIP && d3==PI0 ) { _flag=4; _d1=0; _d2=1; _d3=2;}
    if ( d1==KM && d3==PIP && d2==PI0 ) { _flag=4; _d1=0; _d2=2; _d3=1;}
    if ( d2==KM && d1==PIP && d3==PI0 ) { _flag=4; _d1=1; _d2=0; _d3=2;}
    if ( d2==KM && d3==PIP && d1==PI0 ) { _flag=4; _d1=1; _d2=2; _d3=0;}
    if ( d3==KM && d1==PIP && d2==PI0 ) { _flag=4; _d1=2; _d2=0; _d3=1;}
    if ( d3==KM && d2==PIP && d1==PI0 ) { _flag=4; _d1=2; _d2=1; _d3=0;}

    if ( d1==KB && d2==PIP && d3==PIM )  { _flag=3; _d1=0; _d2=1; _d3=2;}
    if ( d1==KB && d3==PIP && d2==PIM ) { _flag=3; _d1=0; _d2=2; _d3=1;}
    if ( d2==KB && d1==PIP && d3==PIM ) { _flag=3; _d1=1; _d2=0; _d3=2;}
    if ( d2==KB && d3==PIP && d1==PIM ) { _flag=3; _d1=1; _d2=2; _d3=0;}
    if ( d3==KB && d1==PIP && d2==PIM ) { _flag=3; _d1=2; _d2=0; _d3=1;}
    if ( d3==KB && d2==PIP && d1==PIM ) { _flag=3; _d1=2; _d2=1; _d3=0;}
  }
  if ( parnum == D0B ) {
    //look for either a K+ pi- pi0 or K0 pi+ pi-
    if ( d1==KP && d2==PIM && d3==PI0 )  { _flag=4; _d1=0; _d2=1; _d3=2;}
    if ( d1==KP && d3==PIM && d2==PI0 ) { _flag=4; _d1=0; _d2=2; _d3=1;}
    if ( d2==KP && d1==PIM && d3==PI0 ) { _flag=4; _d1=1; _d2=0; _d3=2;}
    if ( d2==KP && d3==PIM && d1==PI0 ) { _flag=4; _d1=1; _d2=2; _d3=0;}
    if ( d3==KP && d1==PIM && d2==PI0 ) { _flag=4; _d1=2; _d2=0; _d3=1;}
    if ( d3==KP && d2==PIM && d1==PI0 ) { _flag=4; _d1=2; _d2=1; _d3=0;}

    if ( d1==K0 && d2==PIP && d3==PIM )  { _flag=3; _d1=0; _d2=1; _d3=2;}
    if ( d1==K0 && d3==PIP && d2==PIM ) { _flag=3; _d1=0; _d2=2; _d3=1;}
    if ( d2==K0 && d1==PIP && d3==PIM ) { _flag=3; _d1=1; _d2=0; _d3=2;}
    if ( d2==K0 && d3==PIP && d1==PIM ) { _flag=3; _d1=1; _d2=2; _d3=0;}
    if ( d3==K0 && d1==PIP && d2==PIM ) { _flag=3; _d1=2; _d2=0; _d3=1;}
    if ( d3==K0 && d2==PIP && d1==PIM ) { _flag=3; _d1=2; _d2=1; _d3=0;}
  }

  if ( parnum == DP ) {
    //look for K- pi+ pi+
    if ( d1==KB && d2==PIP && d3==PI0 )  { _flag=2; _d1=0; _d2=1; _d3=2;}
    if ( d1==KB && d3==PIP && d2==PI0 ) { _flag=2; _d1=0; _d2=2; _d3=1;}
    if ( d2==KB && d1==PIP && d3==PI0 ) { _flag=2; _d1=1; _d2=0; _d3=2;}
    if ( d2==KB && d3==PIP && d1==PI0 ) { _flag=2; _d1=1; _d2=2; _d3=0;}
    if ( d3==KB && d1==PIP && d2==PI0 ) { _flag=2; _d1=2; _d2=0; _d3=1;}
    if ( d3==KB && d2==PIP && d1==PI0 ) { _flag=2; _d1=2; _d2=1; _d3=0;}

    if ( d1==KM && d2==PIP && d3==PIP )  { _flag=1; _d1=0; _d2=1; _d3=2;}
    if ( d2==KM && d1==PIP && d3==PIP ) { _flag=1; _d1=1; _d2=0; _d3=2;}
    if ( d3==KM && d1==PIP && d2==PIP ) { _flag=1; _d1=2; _d2=0; _d3=1;}
  }

  if ( parnum == DM ) {
    //look for K- pi+ pi+
    if ( d1==K0 && d2==PIM && d3==PI0 )  { _flag=2; _d1=0; _d2=1; _d3=2;}
    if ( d1==K0 && d3==PIM && d2==PI0 ) { _flag=2; _d1=0; _d2=2; _d3=1;}
    if ( d2==K0 && d1==PIM && d3==PI0 ) { _flag=2; _d1=1; _d2=0; _d3=2;}
    if ( d2==K0 && d3==PIM && d1==PI0 ) { _flag=2; _d1=1; _d2=2; _d3=0;}
    if ( d3==K0 && d1==PIM && d2==PI0 ) { _flag=2; _d1=2; _d2=0; _d3=1;}
    if ( d3==K0 && d2==PIM && d1==PI0 ) { _flag=2; _d1=2; _d2=1; _d3=0;}

    if ( d1==KP && d2==PIM && d3==PIM )  { _flag=1; _d1=0; _d2=1; _d3=2;}
    if ( d2==KP && d1==PIM && d3==PIM ) { _flag=1; _d1=1; _d2=0; _d3=2;}
    if ( d3==KP && d1==PIM && d2==PIM ) { _flag=1; _d1=2; _d2=0; _d3=1;}
  }
  
  if ( _flag==0) {
    report(ERROR,"EvtGen") << "EvtDDaltiz: Invalid mode."<<std::endl;
    assert(0);
  }
}

void EvtDDalitz::initProbMax() {

//probmax different for different modes!  

  if ( _flag==1 ) {setProbMax(9.6);}
  if ( _flag==2 ) {setProbMax(147.9);}
  if ( _flag==3 ) {setProbMax(80.2);}
  if ( _flag==4 ) {setProbMax(3000.0);}

}

void EvtDDalitz::decay( EvtParticle *p){

  //added by Lange Jan4,2000
  static EvtId DM=EvtPDL::getId("D-");
  static EvtId DP=EvtPDL::getId("D+");
  static EvtId D0=EvtPDL::getId("D0");
  static EvtId D0B=EvtPDL::getId("anti-D0");
  static EvtId KM=EvtPDL::getId("K-");
  static EvtId KP=EvtPDL::getId("K+");
  static EvtId K0=EvtPDL::getId("K0");
  static EvtId KB=EvtPDL::getId("anti-K0");
  static EvtId PIM=EvtPDL::getId("pi-");
  static EvtId PIP=EvtPDL::getId("pi+");
  static EvtId PI0=EvtPDL::getId("pi0");

  double oneby2 = 0.707106782;

//same structure for all of these decays

  p->initializePhaseSpace(getNDaug(),getDaugs());
  EvtVector4R moms1 = p->getDaug(_d1)->getP4();
  EvtVector4R moms2 = p->getDaug(_d2)->getP4();
  EvtVector4R moms3 = p->getDaug(_d3)->getP4();

  EvtVector4R p4_p;
  p4_p.set(p->mass(),0.0,0.0,0.0);

  EvtComplex amp(1.0,0.0);

//now determine which D and which decay

//data from Anjos et al, Phys.Rev.D 1993, v.48,num.1,p.56 (E691 resuls)
//for D+ -> K- pi+ pi+, and from Adler et al, Phys.Lett. B196 (1987), 107
//(Mark III results) for D+ -> K0bar pi+ pi0. 
  //CLEO results for D0->k-pi+pi0

  if ( _flag==1) {

//have a D+ -> K- pi+ pi+ decay, or charge conjugate

    EvtResonance DplusRes11(p4_p,moms1,moms2,0.78,-60.0,0.0498,0.89610,1);
    EvtResonance DplusRes12(p4_p,moms3,moms1,0.78,-60.0,0.0498,0.89610,1);//K*(892)
    
    EvtResonance DplusRes21(p4_p,moms1,moms2,0.53,132.0,0.287,1.429,0);
    EvtResonance DplusRes22(p4_p,moms3,moms1,0.53,132.0,0.287,1.429,0);//K*(1430)
    
    EvtResonance DplusRes31(p4_p,moms1,moms2,0.47,-51.0,0.323,1.714,1);
    EvtResonance DplusRes32(p4_p,moms3,moms1,0.47,-51.0,0.323,1.714,1);//K*(1680)
    
    amp = amp + oneby2*(-DplusRes11.resAmpl()+DplusRes12.resAmpl()) + oneby2*(DplusRes21.resAmpl() + DplusRes22.resAmpl()) + oneby2*(-DplusRes31.resAmpl()+ DplusRes32.resAmpl());
    
 }
  
  if ( _flag==2) {

//have a D+ -> K0bar pi+ pi0 decay (daughters must be listed in this order!)
    
    EvtResonance DplusKpipi0Res1(p4_p,moms2,moms3,1.00,0.00,0.1512,0.7699,1); //rho+  
    EvtResonance DplusKpipi0Res2(p4_p,moms3,moms1,0.8695,0.7191,0.0498,0.89159,1); //K*0
    
    amp = 0.9522*EvtComplex(cos(-1.8565),sin(-1.8565)) - 1.00*DplusKpipi0Res1.relBrWig(0) + 0.8695*EvtComplex(cos(0.7191),sin(0.7191))*DplusKpipi0Res2.relBrWig(1);
    
  }

  if(_flag==3) {

//know it's a D0 -> K0_bar pi+ pi- or charge conjugate

    EvtResonance DK2piRes1(p4_p,moms1,moms2,2.31,109.0,0.0498,0.89610,1);//K*(892)
    EvtResonance DK2piRes2(p4_p,moms3,moms2,1.59,-123.0,0.1491,0.7683,1);//RHO(770)
    
    amp = amp + DK2piRes1.resAmpl() + DK2piRes2.resAmpl();
  }

  
  if(_flag==4) {

    EvtResonance2 DKpipi0Res1(p4_p,moms2,moms3,1.0  ,0.0   ,0.1507,0.770 ,1); //rho
    EvtResonance2 DKpipi0Res2(p4_p,moms1,moms2,0.39, -0.2  ,0.0505,0.8961,1); //k*0
    EvtResonance2 DKpipi0Res3(p4_p,moms1,moms3,0.44, 163.0 ,0.050 ,0.8915,1); //k*-
    
    EvtResonance2 DKpipi0Res4(p4_p,moms1,moms3,0.77 ,55.5  ,0.294 ,1.412 ,0); //k01430-
    EvtResonance2 DKpipi0Res5(p4_p,moms1,moms2,0.85 ,166.0 ,0.294 ,1.412 ,0); //k01430bar
    EvtResonance2 DKpipi0Res6(p4_p,moms2,moms3,2.5  ,171.0 ,0.240 ,1.700 ,1); //rho1700
    EvtResonance2 DKpipi0Res7(p4_p,moms1,moms3,2.5  ,103.0 ,0.322 ,1.717 ,1); //K*1680-
    
    
    
    double pi180inv = 1.0/EvtConst::radToDegrees;
    
    amp = EvtComplex(1.75*cos(31.2*pi180inv),1.75*sin(31.2*pi180inv)) 
      + DKpipi0Res1.resAmpl() + DKpipi0Res2.resAmpl() + DKpipi0Res3.resAmpl()
      + DKpipi0Res4.resAmpl() + DKpipi0Res5.resAmpl() 
      + DKpipi0Res6.resAmpl()
      + DKpipi0Res7.resAmpl();
    
  }
 
  vertex(amp);

  return ;
}

