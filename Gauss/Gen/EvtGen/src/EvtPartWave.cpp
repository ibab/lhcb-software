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
// Module: EvtHelAmp.cc
//
// Description: Decay model for implementation of generic 2 body
//              decay specified by the partial wave amplitudes
//
//
// Modification history:
//
//    fkw        February 2, 2001     changes to satisfy KCC
//    RYD       September 7, 2000       Module created
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
#include "EvtGenBase/EvtVector4C.hh"
#include "EvtGenBase/EvtTensor4C.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtPartWave.hh"
#include "EvtGenBase/EvtEvalHelAmp.hh"
#include "EvtGenBase/EvtId.hh"
#include <string>
#include "EvtGenBase/EvtConst.hh"
#include "EvtGenBase/EvtKine.hh"
#include "EvtGenBase/EvtCGCoefSingle.hh"
#include <algorithm>
EvtPartWave::~EvtPartWave() {}

void EvtPartWave::getName(std::string& model_name){

  model_name="PARTWAVE";     

}


EvtDecayBase* EvtPartWave::clone(){

  return new EvtPartWave;

}

void EvtPartWave::init(){

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

    report(INFO,"EvtGen")<<"Will now figure out the valid (M_LS) states:"<<std::endl;

  }

#ifdef WIN32
  int Lmin=__max(_JA2-_JB2-_JC2,__max(_JB2-_JA2-_JC2,_JC2-_JA2-_JB2));
#else
  int Lmin=std::max(_JA2-_JB2-_JC2,std::max(_JB2-_JA2-_JC2,_JC2-_JA2-_JB2));
#endif
  if (Lmin<0) Lmin=0;
  //int Lmin=_JA2-_JB2-_JC2;
  int Lmax=_JA2+_JB2+_JC2;

  int L;

  int _nPartialWaveAmp=0;

  int _nL[50];
  int _nS[50];

  for (L=Lmin;L<=Lmax;L+=2){
    int Smin=abs(L-_JA2);
    if (Smin<abs(_JB2-_JC2)) Smin=abs(_JB2-_JC2);
    int Smax=L+_JA2;
    if (Smax>abs(_JB2+_JC2)) Smax=abs(_JB2+_JC2);
    int S;
    for (S=Smin;S<=Smax;S+=2){
      _nL[_nPartialWaveAmp]=L;
      _nS[_nPartialWaveAmp]=S;

      _nPartialWaveAmp++;
      if (verbose()){
	report(INFO,"EvtGen")<<"M["<<L<<"]["<<S<<"]"<<std::endl;    
      }
    }
  }

  checkNArg(_nPartialWaveAmp*2);

  int argcounter=0;

  EvtComplex _M[50];

  for(i=0;i<_nPartialWaveAmp;i++){
    _M[i]=getArg(argcounter)*exp(EvtComplex(0.0,getArg(argcounter+1)));;
    argcounter+=2;
    if (verbose()){
      report(INFO,"EvtGen")<<"M["<<_nL[i]<<"]["<<_nS[i]<<"]="<<_M[i]<<std::endl;
    }
  }

  //Now calculate the helicity amplitudes

  

  for(ib=0;ib<_nB;ib++){
    for(ic=0;ic<_nC;ic++){
      _HBC[ib][ic]=0.0;
      if (abs(_lambdaB2[ib]-_lambdaC2[ic])<=_JA2){
	for(i=0;i<_nPartialWaveAmp;i++){
	  int L=_nL[i];
	  int S=_nS[i];
	  int lambda2=_lambdaB2[ib];
	  int lambda3=_lambdaC2[ic];
	  int s1=_JA2;
	  int s2=_JB2;
	  int s3=_JC2;
	  int m1=lambda2-lambda3;
	  EvtCGCoefSingle c1;
	  c1.init(s2,s3);
	  EvtCGCoefSingle c2;
	  c2.init(L,S);
	  if (verbose()){
	    report(INFO,"EvtGen") << "s2,lambda2:"<<s2<<" "<<lambda2<<std::endl;
	  }
	  //fkw changes to satisfy KCC
	  double fkwTmp = (L+1.0)/(s1+1.0);

	  if (S>=abs(m1)){

	    EvtComplex tmp=sqrt(fkwTmp)
	      *c1.coef(S,m1,s2,s3,lambda2,-lambda3)
	      *c2.coef(s1,m1,L,S,0,m1)*_M[i];
	    //fkw EvtComplex tmp=sqrt((L+1)/(s1+1))*c1.coef(S,m1,s2,s3,lambda2,-lambda3)*c2.coef(s1,m1,L,S,0,m1)*_M[i];
	    _HBC[ib][ic]+=tmp;
	  }
	}
	if (verbose()){
	  report(INFO,"EvtGen")<<"_HBC["<<ib<<"]["<<ic<<"]="<<_HBC[ib][ic]<<std::endl;
	}
      }
    }
  }

  _evalHelAmp=new EvtEvalHelAmp(EvtPDL::getSpinType(getParentId()),
				EvtPDL::getSpinType(getDaug(0)),
				EvtPDL::getSpinType(getDaug(1)),
				_HBC);


}


void EvtPartWave::initProbMax(){

  double maxprob=_evalHelAmp->probMax();

  if (verbose()){
    report(INFO,"EvtGen")<<"Calculated probmax"<<maxprob<<std::endl;
  }

  setProbMax(maxprob);

}


void EvtPartWave::decay( EvtParticle *p){

  //first generate simple phase space
  p->initializePhaseSpace(getNDaug(),getDaugs());

  _evalHelAmp->evalAmp(p,_amp2);

  return;

}



void EvtPartWave::fillHelicity(int* lambda2,int n,int J2){
  
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











