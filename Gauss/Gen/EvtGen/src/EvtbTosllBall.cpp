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
#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtbTosllBall.hh"
#include "EvtGenModels/EvtbTosllBallFF.hh"
#include "EvtGenModels/EvtbTosllAmp.hh"
#include "EvtGenModels/EvtbTosllScalarAmp.hh"
#include "EvtGenModels/EvtbTosllVectorAmp.hh"

#include <string>

EvtbTosllBall::~EvtbTosllBall() {
  delete _ballffmodel ;
  if ( _calcamp ) delete _calcamp ;
}

void EvtbTosllBall::getName(std::string& model_name){

  model_name="BTOSLLBALL";     
}


EvtDecayBase* EvtbTosllBall::clone(){

  return new EvtbTosllBall;

}

void EvtbTosllBall::decay( EvtParticle *p ){

  setWeight(p->initializePhaseSpace(getNDaug(),getDaugs(),_poleSize,1,2));

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
                           EvtPDL::name(getDaug(0)).c_str()<<std::endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
    ::abort();
  }

  checkSpinDaughter(1,EvtSpinType::DIRAC);
  checkSpinDaughter(2,EvtSpinType::DIRAC);

  _ballffmodel = new EvtbTosllBallFF();
  if (mesontype == EvtSpinType::SCALAR){
    _calcamp = new EvtbTosllScalarAmp(-0.313,4.344,-4.669); 
  }
  if (mesontype == EvtSpinType::VECTOR){
    _calcamp = new EvtbTosllVectorAmp(-0.313,4.344,-4.669); 
  }

}





