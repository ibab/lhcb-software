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
// Module: EvtHelAmp.cc
//
// Description: Decay model for implementation of generic 2 body
//              decay specified by the helicity amplitudes
//
//
// Modification history:
//
//    RYD       March 14, 1999       Module created
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
#include "EvtGenModels/EvtHelAmp.hh"
#include "EvtGenBase/EvtId.hh"
#include <string>
#include "EvtGenBase/EvtConst.hh"
#include "EvtGenBase/EvtEvalHelAmp.hh"


EvtHelAmp::~EvtHelAmp() {

  delete _evalHelAmp;

}

void EvtHelAmp::getName(std::string& model_name){

  model_name="HELAMP";     

}


EvtDecayBase* EvtHelAmp::clone(){

  return new EvtHelAmp;

}

void EvtHelAmp::init(){

  checkNDaug(2);


  //find out how many states each particle have
  int _nA=EvtSpinType::getSpinStates(EvtPDL::getSpinType(getParentId()));
  int _nB=EvtSpinType::getSpinStates(EvtPDL::getSpinType(getDaug(0)));
  int _nC=EvtSpinType::getSpinStates(EvtPDL::getSpinType(getDaug(1)));

  EvtId _idA=getParentId();
  EvtId _idB=getDaug(0);
  EvtId _idC=getDaug(1);

  if (verbose()){
    report(INFO,"EvtGen")<<"_nA,_nB,_nC:"
			 <<_nA<<","<<_nB<<","<<_nC<<std::endl;
  }

  //find out what 2 times the spin is
  int _JA2=EvtSpinType::getSpin2(EvtPDL::getSpinType(getParentId()));
  int _JB2=EvtSpinType::getSpin2(EvtPDL::getSpinType(getDaug(0)));
  int _JC2=EvtSpinType::getSpin2(EvtPDL::getSpinType(getDaug(1)));

  if (verbose()){
    report(INFO,"EvtGen")<<"_JA2,_JB2,_JC2:"
			 <<_JA2<<","<<_JB2<<","<<_JC2<<std::endl;
  }

  //allocate memory
  int* _lambdaA2=new int[_nA];
  int* _lambdaB2=new int[_nB];
  int* _lambdaC2=new int[_nC];

  EvtComplexPtr* _HBC=new EvtComplexPtr[_nB];
  int /*ia,*/ib,ic;
  for(ib=0;ib<_nB;ib++){
    _HBC[ib]=new EvtComplex[_nC];
  }

  int i;
  //find the allowed helicities (actually 2*times the helicity!)

  fillHelicity(_lambdaA2,_nA,_JA2);
  fillHelicity(_lambdaB2,_nB,_JB2);
  fillHelicity(_lambdaC2,_nC,_JC2);

  if (verbose()){
    report(INFO,"EvtGen")<<"Helicity states of particle A:"<<std::endl;
    for(i=0;i<_nA;i++){
      report(INFO,"EvtGen")<<_lambdaA2[i]<<std::endl;
    }

    report(INFO,"EvtGen")<<"Helicity states of particle B:"<<std::endl;
    for(i=0;i<_nB;i++){
      report(INFO,"EvtGen")<<_lambdaB2[i]<<std::endl;
    }

    report(INFO,"EvtGen")<<"Helicity states of particle C:"<<std::endl;
    for(i=0;i<_nC;i++){
      report(INFO,"EvtGen")<<_lambdaC2[i]<<std::endl;
    }
  }

  //now read in the helicity amplitudes

  int argcounter=0;

  for(ib=0;ib<_nB;ib++){
    for(ic=0;ic<_nC;ic++){
      _HBC[ib][ic]=0.0;
      if (abs(_lambdaB2[ib]-_lambdaC2[ic])<=_JA2) argcounter+=2;
    }
  }

  checkNArg(argcounter);

  argcounter=0;

  for(ib=0;ib<_nB;ib++){
    for(ic=0;ic<_nC;ic++){
      if (abs(_lambdaB2[ib]-_lambdaC2[ic])<=_JA2) {
	_HBC[ib][ic]=getArg(argcounter)*exp(EvtComplex(0.0,getArg(argcounter+1)));;
	argcounter+=2;
	if (verbose()){
	  report(INFO,"EvtGen")<<"_HBC["<<ib<<"]["<<ic<<"]="
			       <<_HBC[ib][ic]<<std::endl;
	}
      }
    }
  }

  _evalHelAmp=new EvtEvalHelAmp(EvtPDL::getSpinType(getParentId()),
				EvtPDL::getSpinType(getDaug(0)),
				EvtPDL::getSpinType(getDaug(1)),
				_HBC);
  

}


void EvtHelAmp::initProbMax(){

  double maxprob=_evalHelAmp->probMax();

  if (verbose()){
    report(INFO,"EvtGen")<<"Calculated probmax"<<maxprob<<std::endl;
  }

  setProbMax(maxprob);

}


void EvtHelAmp::decay( EvtParticle *p){

  //first generate simple phase space
  p->initializePhaseSpace(getNDaug(),getDaugs());

  _evalHelAmp->evalAmp(p,_amp2);
    
  return ;

}


void EvtHelAmp::fillHelicity(int* lambda2,int n,int J2){
  
  int i;
  
  //photon is special case!
  if (n==2&&J2==2) {
    lambda2[0]=2;
    lambda2[1]=-2;
    return;
  }
  
  assert(n==J2+1);

  for(i=0;i<n;i++){
    lambda2[i]=n-i*2-1;
  }

  return;

}







