//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 2000      Caltech, UCSB
//
// Module: EvtbTosllBall.cc
//
// Description: Routine to implement b->sll decays according to Ball et al. 
//
// Modification history:
//
//    Ryd     January 5, 2000        Module created
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtbTosllBall.hh"
#include "EvtGen/EvtbTosllBallFF.hh"
#include "EvtGen/EvtbTosllAmp.hh"
#include "EvtGen/EvtbTosllScalarAmp.hh"
#include "EvtGen/EvtbTosllVectorAmp.hh"

#include "EvtGen/EvtString.hh"

EvtbTosllBall::~EvtbTosllBall() {}

void EvtbTosllBall::getName(EvtString& model_name){

  model_name="BTOSLLBALL";     
}


EvtDecayBase* EvtbTosllBall::clone(){

  return new EvtbTosllBall;

}

void EvtbTosllBall::decay( EvtParticle *p ){

  p->makeDaughters(getNDaug(),getDaugs());

  EvtParticle* k=p->getDaug(0);
  EvtParticle* leptonp=p->getDaug(1);
  EvtParticle* leptonn=p->getDaug(2);
  
  EvtVector4R p4[3];

  double mass[3];
   
  double m_b = p->mass();
 
  findMasses( p, getNDaug(), getDaugs(), mass );

 
  EvtVector4R p4temp[3];
  setWeight(EvtGenKine::PhaseSpacePole( m_b, mass[1],mass[2],mass[0],
					_poleSize, p4temp ));
  p4[0]=p4temp[2];
  p4[1]=p4temp[0];
  p4[2]=p4temp[1];
  
  k->init( getDaug(0), p4[0] );
  leptonp->init( getDaug(1), p4[1]);
  leptonn->init( getDaug(2), p4[2]);

  _calcamp->CalcAmp(p,_amp2,_ballffmodel);

}


void EvtbTosllBall::initProbMax(){

  EvtId parnum,mesnum,l1num,l2num;
  
  parnum = getParentId();
  mesnum = getDaug(0);
  l1num = getDaug(1);
  l2num = getDaug(2);
  
  //This routine sets the _poleSize.
  double mymaxprob = _calcamp->CalcMaxProb(parnum,mesnum,
					   l1num,l2num,
					   _ballffmodel,_poleSize);

  setProbMax(mymaxprob);

}


void EvtbTosllBall::init(){

  checkNArg(0);
  checkNDaug(3);

  //We expect the parent to be a scalar 
  //and the daughters to be X lepton+ lepton-

  checkSpinParent(EvtSpinType::SCALAR);

  EvtSpinType::spintype mesontype=EvtPDL::getSpinType(getDaug(0));

  if ( !(mesontype == EvtSpinType::VECTOR||
	mesontype == EvtSpinType::SCALAR)) {
    report(ERROR,"EvtGen") << "EvtbTosllBall generator expected "
                           << " a SCALAR or VECTOR 1st daughter, found:"<<
                           EvtPDL::name(getDaug(0))<<endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<endl;
    ::abort();
  }

  checkSpinDaughter(1,EvtSpinType::DIRAC);
  checkSpinDaughter(2,EvtSpinType::DIRAC);

  _ballffmodel = new EvtbTosllBallFF();
  if (mesontype == EvtSpinType::SCALAR){
    _calcamp = new EvtbTosllScalarAmp; 
  }
  if (mesontype == EvtSpinType::VECTOR){
    _calcamp = new EvtbTosllVectorAmp; 
  }

}





