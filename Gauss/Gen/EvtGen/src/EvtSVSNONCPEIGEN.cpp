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
// Module: EvtSVSNONCPEIGEN.cc
//
// Description: Routine to decay scalar -> vectors scalar
//              and has CP violation.
//
// Modification history:
//
//    RYD       April 26, 1997       Module created
//
//------------------------------------------------------------------------
// 
#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtIncoherentMixing.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtVector4C.hh"
#include "EvtGenModels/EvtSVSNONCPEIGEN.hh"
#include <string>
#include "EvtGenBase/EvtConst.hh"

EvtSVSNONCPEIGEN::~EvtSVSNONCPEIGEN() {}

void EvtSVSNONCPEIGEN::getName(std::string& model_name){

  model_name="SVS_NONCPEIGEN";     

}


EvtDecayBase* EvtSVSNONCPEIGEN::clone(){

  return new EvtSVSNONCPEIGEN;

}

void EvtSVSNONCPEIGEN::init(){

  // check that there are 11 arguments
  checkNArg(11,7);
  checkNDaug(2);

  if ( getParentId() != EvtPDL::getId("B0") ) {
    report(ERROR, "EvtGen") << "EvtSVSNONCPEIGEN cannot decay "
			    << EvtPDL::name(getParentId()).c_str() 
			    << ". Must be specified to decay"
			    << " only B0 or a B0 alias." << std::endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
    ::abort();
  }

  checkSpinDaughter(0,EvtSpinType::VECTOR);
  checkSpinDaughter(1,EvtSpinType::SCALAR);

  _dm=getArg(1);
  _phickm=getArg(0);

  _A_f=EvtComplex(getArg(3)*cos(getArg(4)),getArg(3)*sin(getArg(4)));
  _Abar_f=EvtComplex(getArg(5)*cos(getArg(6)),getArg(5)*sin(getArg(6)));
  
  _A_fbar=_Abar_f;
  _Abar_fbar=_A_f;

  if (getNArg()==11){
    _A_fbar=EvtComplex(getArg(7)*cos(getArg(8)),getArg(7)*sin(getArg(8)));
    _Abar_fbar=EvtComplex(getArg(9)*cos(getArg(10)),getArg(9)*sin(getArg(10)));
  }
}

void EvtSVSNONCPEIGEN::initProbMax() {
  double theProbMax = 
    abs(_A_f) * abs(_A_f) +
    abs(_Abar_f) * abs(_Abar_f) +
    abs(_A_fbar) * abs(_A_fbar) +
    abs(_Abar_fbar) * abs(_Abar_fbar);
  
  setProbMax(theProbMax);
}

void EvtSVSNONCPEIGEN::decay( EvtParticle *p){

  //added by Lange Jan4,2000
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

  EvtParticle *v;
  p->initializePhaseSpace(2, daugs);
  v= p->getDaug(0);
  EvtVector4R momv = v->getP4();
  EvtVector4R moms = p->getDaug(1)->getP4();
  
  EvtComplex amp;

  EvtIncoherentMixing::OtherB(p,t,other_b,0.5);
  
   if (!flip) {
     if (other_b==B0B){

       amp=_A_f*cos(_dm*t/(2*EvtConst::c))+
	 EvtComplex(cos(-2.0*_phickm),sin(-2.0*_phickm))*
	 EvtComplex(0.0,1.0)*_Abar_f*sin(_dm*t/(2*EvtConst::c));
     }
     if (other_b==B0){
            
       amp=_A_f*EvtComplex(cos(2.0*_phickm),sin(2.0*_phickm))*
	 EvtComplex(0.0,1.0)*sin(_dm*t/(2*EvtConst::c))+       
	 _Abar_f*cos(_dm*t/(2*EvtConst::c));
     }
   }
   else{
     if (other_b==B0B){

       amp=_A_fbar*cos(_dm*t/(2*EvtConst::c))+
	 EvtComplex(cos(-2.0*_phickm),sin(-2.0*_phickm))*
	 EvtComplex(0.0,1.0)*_Abar_fbar*sin(_dm*t/(2*EvtConst::c));
     }
     if (other_b==B0){

       amp=_A_fbar*EvtComplex(cos(2.0*_phickm),sin(2.0*_phickm))*
	 EvtComplex(0.0,1.0)*sin(_dm*t/(2*EvtConst::c))+       
	 _Abar_fbar*cos(_dm*t/(2*EvtConst::c));
     }
   }

  EvtVector4R p4_parent;

  p4_parent=momv+moms;

  double norm=momv.mass()/(momv.d3mag()*p->mass());

  vertex(0,amp*norm*p4_parent*(v->epsParent(0)));
  vertex(1,amp*norm*p4_parent*(v->epsParent(1)));
  vertex(2,amp*norm*p4_parent*(v->epsParent(2)));

  return ;
}

