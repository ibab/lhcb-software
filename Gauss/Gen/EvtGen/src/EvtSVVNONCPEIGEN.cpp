//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 2001      Royal Holloway, University of London
//
// Module: EvtSVVNONCPEIGEN.cc
//
// Description: Routine to decay scalar -> vector vector
//              and has CP violation.
//
//              There are 27 arguments !  The first three are dm, phase1
//              and phase2.  dm is the B0-B0bar mass difference. Phases 1
//              and 2 are the CKM phases relevant for the particular mode, 
//              eg for B-->DstRho phase1 is beta and phase2 is gamma.
//              The other 24 aruguments are the 4 amplitudes in the order:
//              A_f, Abar_f, A_fbar, Abar_fbar.  Each amplitude has its 3 
//              helicity states in the order +, 0, -, which are each 
//              specified by a magnitude and a phase.
//
//              
// Modification history:
//    Ajit Kurup 9 March 2001        Module created (from EvtSVSNONCPEIGEN)
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtGenKine.hh"
#include "EvtGenBase/EvtIncoherentMixing.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtVector4C.hh"
#include "EvtGenModels/EvtSVVNONCPEIGEN.hh"
#include <string>
#include "EvtGenModels/EvtSVVHelAmp.hh"
#include "EvtGenBase/EvtConst.hh"

EvtSVVNONCPEIGEN::~EvtSVVNONCPEIGEN() {}

void EvtSVVNONCPEIGEN::getName(std::string& model_name){

  model_name="SVV_NONCPEIGEN";     

}


EvtDecayBase* EvtSVVNONCPEIGEN::clone(){

  return new EvtSVVNONCPEIGEN;

}

void EvtSVVNONCPEIGEN::init(){

  // check that there are 27 arguments
  checkNArg(27,15);
  checkNDaug(2);

  EvtSpinType::spintype parenttype = EvtPDL::getSpinType(getParentId());

  if ( getParentId() != EvtPDL::getId("B0") ) {
    report(ERROR,"EvtGen") << "EvtSVVNONCPEIGEN decay "
                           << EvtPDL::name(getParentId()).c_str()
			   << ".  Must be specified to decay "
			   << "only B0 or a B0 alias. " << std::endl;
    report(ERROR,"EvtGen") << "Will terminate execution!"<<std::endl;
    ::abort();
  }

  checkSpinDaughter(0,EvtSpinType::VECTOR);
  checkSpinDaughter(1,EvtSpinType::VECTOR);

  // The ordering of A_f is :
  // A_f[0:2] = A_f
  // A_f[3:5] = Abar_f
  // A_f[6:8] = A_fbar 
  // A_f[9:11] = Abar_fbar
  //
  // Each of the 4 amplitudes include the 3 different helicity states in 
  // the order +, 0, -.

  int i=0;
  int j=(getNArg()-3)/2;

  for(i=0; i<j; ++i){
    _A_f[i] = EvtComplex(getArg((2*i)+3)*cos(getArg((2*i)+4)),
			getArg((2*i)+3)*sin(getArg((2*i)+4)));
  }

  // If only 6 amplitudes are specified, calculate the last 6 from the first 6:
  if(6 == j){
    for(i = 0; i < 3; ++i){
      _A_f[6+i] = _A_f[3+i];
      _A_f[9+i] = _A_f[i];
    }
  }
}

void EvtSVVNONCPEIGEN::initProbMax() {
  double probMax = 0;
  for (int i = 0; i < 12; ++i){
    double amp = abs(_A_f[i]);
    probMax += amp * amp;
  }

  setProbMax(probMax); 
}

void EvtSVVNONCPEIGEN::decay( EvtParticle *p){

  //added by Lange Jan4,2000
  static EvtId B0=EvtPDL::getId("B0");
  static EvtId B0B=EvtPDL::getId("anti-B0");

  double t;
  EvtId other_b;
  EvtId daugs[2];

  int flip=0;
  if (EvtRandom::Flat(0.0,1.0)<0.5) flip=1;
  
  if (!flip) {
    daugs[0]=getDaug(0);
    daugs[1]=getDaug(1);
  }
  else{
    daugs[0]=EvtPDL::chargeConj(getDaug(0));
    daugs[1]=EvtPDL::chargeConj(getDaug(1));
  }

  p->initializePhaseSpace(2,daugs);

  EvtIncoherentMixing::OtherB(p,t,other_b,0.5);

  EvtComplex amp[3];

  double dmt2 = getArg(0) * t / (2 * EvtConst::c);
  double phiCKM = 2.0 * getArg(1) + getArg(2);
  EvtComplex ePlusIPhi(cos(phiCKM), sin(phiCKM));
  EvtComplex eMinusIPhi(cos(-phiCKM), sin(-phiCKM));

  if (!flip) {
    if (other_b==B0B){
      for (int i=0; i<3; ++i){
	amp[i] = _A_f[i] * cos(dmt2)+
	  eMinusIPhi * EvtComplex(0.0,1.0) * _A_f[i+3] * sin(dmt2);
      }
    }
    if (other_b==B0){
      for(int i=0; i<3; ++i){
	amp[i] = _A_f[i] * ePlusIPhi * EvtComplex(0.0,1.0) * sin(dmt2) +       
	  _A_f[i+3] * cos(dmt2);
      }
    }
  }
  else{
    if (other_b==B0B){
      for(int i=0; i<3; ++i){
	amp[i] = _A_f[i+6] * cos(dmt2) +
	  eMinusIPhi * EvtComplex(0.0,1.0) * _A_f[i+9] * sin(dmt2);
      }
    }
    if (other_b==B0){
      for(int i=0; i<3; ++i){
	amp[i] = _A_f[i+6] * ePlusIPhi * EvtComplex(0.0,1.0) * sin(dmt2)+ 
	  _A_f[i+9]*cos(dmt2);
      }
    }
  }
  
  EvtSVVHelAmp::SVVHel(p,_amp2,daugs[0],daugs[1],amp[0],amp[1],amp[2]);

  return ;
}

