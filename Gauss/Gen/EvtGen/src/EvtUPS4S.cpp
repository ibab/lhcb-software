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
// Module: EvtUPS4S.cc
//
// Description: Routine to "decay a VPHO to an UPS4S". This
//              routine is a little special since it can violate
//              4 momentum conservation. The problem is what should we
//              do of the CM energy is to low to produce a B\barB 
//              decay. 
//
// Modification history:
//
//    RYD       May 11, 1997       Module created
//
//------------------------------------------------------------------------
//
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtUPS4S.hh"
#include "EvtGen/EvtString.hh"

EvtUPS4S::~EvtUPS4S() {}

void EvtUPS4S::getName(EvtString& model_name){

  model_name="UPS4S";     

}


EvtDecayBase* EvtUPS4S::clone(){

  return new EvtUPS4S;

}

void EvtUPS4S::init(){

  // check that there are 0 arguments
  checkNArg(0);
  checkNDaug(1);

  checkSpinParent(EvtSpinType::VECTOR);

  checkSpinDaughter(0,EvtSpinType::SCALAR);
}



void EvtUPS4S::decay( EvtParticle *p){

  //added by Lange Jan4,2000
  static EvtId B0=EvtPDL::getId("B0");
  static EvtId B0B=EvtPDL::getId("anti-B0");
  static EvtId BP=EvtPDL::getId("B+");
  static EvtId BM=EvtPDL::getId("B-");
  static EvtId UPS4=EvtPDL::getId("Upsilon(4S)");
  EvtParticle *v;
  
  EvtVector4R p4[MAX_DAUG];
  double mass[MAX_DAUG],m;


  // Prepare for phase space routine. 
 
  findMasses(p,getNDaug(),getDaugs(),mass);

  //  Need phase space random numbers

  m = p->mass();
  EvtGenKine::PhaseSpace( getNDaug(), mass, p4, m );

  double mups4s=p4[0].mass();  
  double bmass_max=EvtPDL::getMass(B0);

  if (bmass_max<EvtPDL::getMass(B0B)) bmass_max=EvtPDL::getMass(B0B);
  if (bmass_max<EvtPDL::getMass(BP)) bmass_max=EvtPDL::getMass(BP);
  if (bmass_max<EvtPDL::getMass(BM)) bmass_max=EvtPDL::getMass(BM);

  if (mups4s<2*bmass_max) {
    report(ERROR,"EvtGen") << "VPHO mass to low="<<mups4s
			 <<" to low, modified to UPS(4S) mass!"<<endl<<
                         " this will violate 4-momnetum conservation"<<endl;
    p4[0].set(0,EvtPDL::getMass(UPS4));
  }

  p->makeDaughters(getNDaug(),getDaugs());
  v=p->getDaug(0);
  v->init(getDaug(0),p4[0]);

  vertex(0,0,EvtComplex(1.0,0.0));
  vertex(0,1,EvtComplex(0.0,0.0));
  vertex(0,2,EvtComplex(0.0,0.0));

  vertex(1,0,EvtComplex(0.0,0.0));
  vertex(1,1,EvtComplex(1.0,0.0));
  vertex(1,2,EvtComplex(0.0,0.0));

  vertex(2,0,EvtComplex(0.0,0.0));
  vertex(2,1,EvtComplex(0.0,0.0));
  vertex(2,2,EvtComplex(1.0,0.0));

  return ;
}

