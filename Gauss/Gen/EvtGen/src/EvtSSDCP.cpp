//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 2001      Caltech
//
// Module: EvtSSDCP.cc
//
// Description: See EvtSSDCP.hh
//
// Modification history:
//
//    RYD       August 12, 2001       Module created
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtRandom.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtCPUtil.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtVector4C.hh"
#include "EvtGen/EvtTensor4C.hh"
#include "EvtGen/EvtSSDCP.hh"
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtConst.hh"

EvtSSDCP::~EvtSSDCP() {}

void EvtSSDCP::getName(EvtString& model_name){

  model_name="SSD_CP";     

}


EvtDecayBase* EvtSSDCP::clone(){

  return new EvtSSDCP;

}

void EvtSSDCP::init(){

  // check that there are 7 or 11 arguments

  checkNArg(11,7);
  checkNDaug(2);

  EvtSpinType::spintype d1type=EvtPDL::getSpinType(getDaug(0));
  EvtSpinType::spintype d2type=EvtPDL::getSpinType(getDaug(1));

  if ( !((getParentId() == EvtPDL::getId("B0"))||(getParentId() == EvtPDL::getId("B0B"))) ) {
    report(ERROR, "EvtGen") << "EvtSSDCP cannot decay "
			    << EvtPDL::name(getParentId()) 
			    << ". Must be specified to decay"
			    << " only B0 or a B0B ." << std::endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
    ::abort();
  }
  if ( d1type != EvtSpinType::SCALAR ) {
    report(ERROR,"EvtGen") << "EvtSSDCP generator expected "
                           << " a SCALAR 1st daughter, found:"<<
                           EvtPDL::name(getDaug(0))<<std::endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
    ::abort();
  }
  if ( (d2type != EvtSpinType::SCALAR)&&(d2type != EvtSpinType::VECTOR)&&(d2type != EvtSpinType::TENSOR) ) {
    report(ERROR,"EvtGen") << "EvtSSDCP generator expected "
                           << " a SCALAR, VECTOR, or TENSOR as 2nd daughter, found:"<<
                           EvtPDL::name(getDaug(1))<<std::endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
    ::abort();
  }


  _phimixing=getArg(0);
  _dm=getArg(1);

  _cp=(int)getArg(2);

  _A_f=EvtComplex(getArg(3)*cos(getArg(4)),getArg(3)*sin(getArg(4)));
  _Abar_f=EvtComplex(getArg(5)*cos(getArg(6)),getArg(5)*sin(getArg(6)));
  
  _A_fbar=_Abar_f;
  _Abar_fbar=_A_f;

  if (getNArg()==11){
    _A_fbar=EvtComplex(getArg(7)*cos(getArg(8)),getArg(7)*sin(getArg(8)));
    _Abar_fbar=EvtComplex(getArg(9)*cos(getArg(10)),getArg(9)*sin(getArg(10)));
  }
}

void EvtSSDCP::initProbMax() {
  double theProbMax = 
    abs(_A_f) * abs(_A_f) +
    abs(_Abar_f) * abs(_Abar_f) +
    abs(_A_fbar) * abs(_A_fbar) +
    abs(_Abar_fbar) * abs(_Abar_fbar);
  
  setProbMax(theProbMax);
}

void EvtSSDCP::decay( EvtParticle *p){

  static EvtId B0=EvtPDL::getId("B0");
  static EvtId B0B=EvtPDL::getId("anti-B0");
  
  double t;
  EvtId other_b;
  EvtId daugs[2];

  int flip=0;
  if (EvtRandom::Flat(0.0,1.0)<getArg(2)) flip=1;

  if (!flip) {
    daugs[0]=getDaug(0);
    daugs[1]=getDaug(1);
  }
  else{
    daugs[0]=EvtPDL::chargeConj(getDaug(0));
    daugs[1]=EvtPDL::chargeConj(getDaug(1));
  }

  EvtParticle *d;
  p->initializePhaseSpace(2, daugs);
  d= p->getDaug(1);
  EvtVector4R momv = d->getP4();
  EvtVector4R moms = p->getDaug(1)->getP4();
  
  EvtComplex amp;


  EvtCPUtil::OtherB(p,t,other_b,0.5);


  if (other_b==B0B){

    amp=_A_f*cos(_dm*t/(2*EvtConst::c))+
      EvtComplex(cos(-2.0*_phimixing),sin(-2.0*_phimixing))*
      EvtComplex(0.0,1.0)*_Abar_f*sin(_dm*t/(2*EvtConst::c));
  }
  if (other_b==B0){
    
    amp=_A_f*EvtComplex(cos(2.0*_phimixing),sin(2.0*_phimixing))*
	 EvtComplex(0.0,1.0)*sin(_dm*t/(2*EvtConst::c))+       
      _Abar_f*cos(_dm*t/(2*EvtConst::c));

  }



  EvtVector4R p4_parent=p->getP4Restframe();
  double m_parent=p4_parent.mass();

  EvtSpinType::spintype d2type=EvtPDL::getSpinType(getDaug(1));


  if (d2type==EvtSpinType::SCALAR) {
    vertex(amp);
  }
  
  if (d2type==EvtSpinType::VECTOR) {
    
    double norm=momv.mass()/(momv.d3mag()*p->mass());
    
    vertex(0,amp*norm*p4_parent*(d->epsParent(0)));
    vertex(1,amp*norm*p4_parent*(d->epsParent(1)));
    vertex(2,amp*norm*p4_parent*(d->epsParent(2)));
  
  }

  if (d2type==EvtSpinType::TENSOR) {

    double norm=d->mass()*d->mass()/(m_parent*d->getP4().d3mag()*d->getP4().d3mag());
 
   
   vertex(0,amp*norm*d->epsTensorParent(0).cont1(p4_parent)*p4_parent);
   vertex(1,amp*norm*d->epsTensorParent(1).cont1(p4_parent)*p4_parent);
   vertex(2,amp*norm*d->epsTensorParent(2).cont1(p4_parent)*p4_parent);
   vertex(3,amp*norm*d->epsTensorParent(3).cont1(p4_parent)*p4_parent);
   vertex(4,amp*norm*d->epsTensorParent(4).cont1(p4_parent)*p4_parent);
  
  }


  return ;
}

