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
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtVector4C.hh"
#include "EvtGen/EvtTensor4C.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtPartWave.hh"
#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtConst.hh"
#include "EvtGen/EvtdFunction.hh"
#include "EvtGen/EvtDiracSpinor.hh"
#include "EvtGen/EvtGammaMatrix.hh"
#include "EvtGen/EvtCGCoefSingle.hh"

EvtPartWave::~EvtPartWave() {}

void EvtPartWave::getName(EvtString& model_name){

  model_name="PARTWAVE";     

}


EvtDecayBase* EvtPartWave::clone(){

  return new EvtPartWave;

}

void EvtPartWave::init(){

  checkNDaug(2);

  //find out how many states each particle have
  _nA=EvtSpinType::getSpinStates(EvtPDL::getSpinType(getParentId()));
  _nB=EvtSpinType::getSpinStates(EvtPDL::getSpinType(getDaug(0)));
  _nC=EvtSpinType::getSpinStates(EvtPDL::getSpinType(getDaug(1)));

  _idA=getParentId();
  _idB=getDaug(0);
  _idC=getDaug(1);

  report(INFO,"EvtGen")<<"_nA,_nB,_nC:"
			<<_nA<<","<<_nB<<","<<_nC<<endl;

  //find out what 2 times the spin is
  _JA2=EvtSpinType::getSpin2(EvtPDL::getSpinType(getParentId()));
  _JB2=EvtSpinType::getSpin2(EvtPDL::getSpinType(getDaug(0)));
  _JC2=EvtSpinType::getSpin2(EvtPDL::getSpinType(getDaug(1)));

  report(INFO,"EvtGen")<<"_JA2,_JB2,_JC2:"
			<<_JA2<<","<<_JB2<<","<<_JC2<<endl;

  //allocate memory
  _lambdaA2=new int[_nA];
  _lambdaB2=new int[_nB];
  _lambdaC2=new int[_nC];

  _HBC=new EvtComplexPtr[_nB];
  int ia,ib,ic;
  for(ib=0;ib<_nB;ib++){
    _HBC[ib]=new EvtComplex[_nC];
  }


  _RA=new EvtComplexPtr[_nA];
  for(ia=0;ia<_nA;ia++){
    _RA[ia]=new EvtComplex[_nA];
  }
  _RB=new EvtComplexPtr[_nB];
  for(ib=0;ib<_nB;ib++){
    _RB[ib]=new EvtComplex[_nB];
  }
  _RC=new EvtComplexPtr[_nC];
  for(ic=0;ic<_nC;ic++){
    _RC[ic]=new EvtComplex[_nC];
  }
  
  _amp=new EvtComplexPtrPtr[_nA];
  _amp1=new EvtComplexPtrPtr[_nA];
  _amp3=new EvtComplexPtrPtr[_nA];
  for(ia=0;ia<_nA;ia++){
    _amp[ia]=new EvtComplexPtr[_nB];
    _amp1[ia]=new EvtComplexPtr[_nB];
    _amp3[ia]=new EvtComplexPtr[_nB];
    for(ib=0;ib<_nB;ib++){
      _amp[ia][ib]=new EvtComplex[_nC];
      _amp1[ia][ib]=new EvtComplex[_nC];
      _amp3[ia][ib]=new EvtComplex[_nC];
    }
  }




  int i;
  //find the allowed helicities (actually 2*times the helicity!)

  fillHelicity(_lambdaA2,_nA,_JA2);
  fillHelicity(_lambdaB2,_nB,_JB2);
  fillHelicity(_lambdaC2,_nC,_JC2);

  report(INFO,"EvtGen")<<"Helicity states of particle A:"<<endl;
  for(i=0;i<_nA;i++){
    report(INFO,"EvtGen")<<_lambdaA2[i]<<endl;
  }

  report(INFO,"EvtGen")<<"Helicity states of particle B:"<<endl;
  for(i=0;i<_nB;i++){
    report(INFO,"EvtGen")<<_lambdaB2[i]<<endl;
  }

  report(INFO,"EvtGen")<<"Helicity states of particle C:"<<endl;
  for(i=0;i<_nC;i++){
    report(INFO,"EvtGen")<<_lambdaC2[i]<<endl;
  }

  report(INFO,"EvtGen")<<"Will now figure out the valid (M_LS) states:"<<endl;

  int Lmin=_JA2-_JB2-_JC2;
  if (Lmin<0) Lmin=0;
  int Lmax=_JA2+_JB2+_JC2;

  int L;

  _nPartialWaveAmp=0;

  for (L=Lmin;L<=Lmax;L+=2){
    int Smin=L-_JA2;
    if (Smin<0) Smin=0;
    int Smax=L+_JA2;
    int S;
    for (S=Smin;S<=Smax;S+=2){
      _nL[_nPartialWaveAmp]=L;
      _nS[_nPartialWaveAmp]=S;
      _nPartialWaveAmp++;
      report(INFO,"EvtGen")<<"M["<<L<<"]["<<S<<"]"<<endl;    
    }
  }

  checkNArg(_nPartialWaveAmp*2);

  int argcounter=0;

  for(i=0;i<_nPartialWaveAmp;i++){
    _M[i]=getArg(argcounter)*exp(EvtComplex(0.0,getArg(argcounter+1)));;
    argcounter+=2;
    report(INFO,"EvtGen")<<"M["<<_nL[i]<<"]["<<_nS[i]<<"]="<<_M[i]<<endl;
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
	  cout << "s2,lambda2:"<<s2<<" "<<lambda2<<endl;
	  //fkw changes to satisfy KCC
	  double fkwTmp = (L+1)/(s1+1);
	  EvtComplex tmp=sqrt(fkwTmp)
	    *c1.coef(S,m1,s2,s3,lambda2,-lambda3)
	    *c2.coef(s1,m1,L,S,0,m1)*_M[i];
	  //fkw EvtComplex tmp=sqrt((L+1)/(s1+1))*c1.coef(S,m1,s2,s3,lambda2,-lambda3)*c2.coef(s1,m1,L,S,0,m1)*_M[i];
	  _HBC[ib][ic]+=tmp;
	}
	report(INFO,"EvtGen")<<"_HBC["<<ib<<"]["<<ic<<"]="<<_HBC[ib][ic]<<endl;
      }
    }
  }

  


}


void EvtPartWave::initProbMax(){

  double c=1.0/sqrt(4*EvtConst::pi/(2*_JA2+1));

  int ia,ib,ic;


  double theta;
  int itheta;

  double maxprob=0.0;

  for(itheta=-10;itheta<=10;itheta++){
    theta=acos(0.099999*itheta);
    for(ia=0;ia<_nA;ia++){
      double prob=0.0;
      for(ib=0;ib<_nB;ib++){
	for(ic=0;ic<_nC;ic++){
	  _amp[ia][ib][ic]=0.0;
	  if (abs(_lambdaB2[ib]-_lambdaC2[ic])<=_JA2) {
	    _amp[ia][ib][ic]=c*_HBC[ib][ic]*
	      EvtdFunction::d(_JA2,_lambdaA2[ia],
			      _lambdaB2[ib]-_lambdaC2[ic],theta);
	    prob+=real(_amp[ia][ib][ic]*conj(_amp[ia][ib][ic]));
	  }
	}
      }
      
      prob*=sqrt(1.0*_nA);
      
      if (prob>maxprob) maxprob=prob;

    }
  }

  report(INFO,"EvtGen")<<"Calculated probmax"<<maxprob<<endl;
  
  setProbMax(maxprob);

}


void EvtPartWave::decay( EvtParticle *p){

  //first generate simple phase space
  p->initializePhaseSpace(getNDaug(),getDaugs());

  //find theta and phi of the first daughter
  
  EvtVector4R pB=p->getDaug(0)->getP4();

  double theta=acos(pB.get(3)/pB.d3mag());
  double phi=atan2(pB.get(2),pB.get(1));

  //cout <<"Theta:"<<theta<<endl;

  //report(INFO,"EvtGen")<<"pB:"<<pB<<endl;
  //report(INFO,"EvtGen")<<"cos(theta),phi:"<<cos(theta)<<","<<phi<<endl;

  double c=sqrt((2*_JA2+1)/(4*EvtConst::pi));

  int ia,ib,ic;

  double prob1=0.0;

  //cout <<"_nA,_nB,_nC:"<<_nA<<" "<<_nB<<" "<<_nC<<endl;

  for(ia=0;ia<_nA;ia++){
    for(ib=0;ib<_nB;ib++){
      for(ic=0;ic<_nC;ic++){
	_amp[ia][ib][ic]=0.0;
	if (abs(_lambdaB2[ib]-_lambdaC2[ic])<=_JA2) {
	  double dfun=EvtdFunction::d(_JA2,_lambdaA2[ia],
				      _lambdaB2[ib]-_lambdaC2[ic],theta);

	  _amp[ia][ib][ic]=c*_HBC[ib][ic]*
	    exp(EvtComplex(0.0,phi*0.5*(_lambdaA2[ia]-_lambdaB2[ib]+
					_lambdaC2[ic])))*dfun;

	  //cout<<"dfun:"<<dfun<<endl;

	}
	prob1+=real(_amp[ia][ib][ic]*conj(_amp[ia][ib][ic]));
      }
    }
  }

  //cout << "prob1:"<<prob1<<endl;
		 
  setUpRotationMatrices(p,theta,phi);

  applyRotationMatrices();

  double prob2=0.0;

  for(ia=0;ia<_nA;ia++){
    for(ib=0;ib<_nB;ib++){
      for(ic=0;ic<_nC;ic++){
	prob2+=real(_amp[ia][ib][ic]*conj(_amp[ia][ib][ic]));
	if (_nA==1){
	  if (_nB==1){
	    if (_nC==1){
	      vertex(_amp[ia][ib][ic]);
	    }
	    else{
	      vertex(ic,_amp[ia][ib][ic]);
	    }
	  }
	  else{
	    if (_nC==1){
	      vertex(ib,_amp[ia][ib][ic]);
	    }
	    else{
	      vertex(ib,ic,_amp[ia][ib][ic]);
	    }
	  }
	}else{
	  if (_nB==1){
	    if (_nC==1){
	      vertex(ia,_amp[ia][ib][ic]);
	    }
	    else{
	      vertex(ia,ic,_amp[ia][ib][ic]);
	    }
	  }
	  else{
	    if (_nC==1){
	      vertex(ia,ib,_amp[ia][ib][ic]);
	    }
	    else{
	      vertex(ia,ib,ic,_amp[ia][ib][ic]);
	    }
	  }
	}
      }
    }
  }

  if (fabs(prob1-prob2)>0.000001*prob1){
    cout << "prob1,prob2:"<<prob1<<" "<<prob2<<endl;
    ::abort();
  }
    
  return ;

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


void EvtPartWave::setUpRotationMatrices(EvtParticle* p,double theta, double phi){

  switch(_JA2){

  case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:

    {

      EvtSpinDensity R=p->rotateToHelicityBasis();

      
      int i,j,n;
      
      n=R.GetDim();
      
      assert(n==_nA);
	
      
      for(i=0;i<n;i++){
	for(j=0;j<n;j++){
	  _RA[i][j]=R.Get(i,j);
	}
      }

    }

    break;

  default:
    report(ERROR,"EvtGen") << "Spin2(_JA2)="<<_JA2<<" not supported!"<<endl;
    ::abort();
  }
  
  
  switch(_JB2){


  case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:

    {
      
      int i,j,n;

      EvtSpinDensity R=p->getDaug(0)->rotateToHelicityBasis(phi,theta,-phi);
      
      n=R.GetDim();
      
      assert(n==_nB);
	
      
      for(i=0;i<n;i++){
	for(j=0;j<n;j++){
	  _RB[i][j]=R.Get(i,j);
	}
      }

    }

    break;

  default:
    report(ERROR,"EvtGen") << "Spin2(_JB2)="<<_JB2<<" not supported!"<<endl;
    ::abort();
  }
  
  switch(_JC2){

  case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:

    {

      int i,j,n;

      EvtSpinDensity R=p->getDaug(1)->rotateToHelicityBasis(phi,theta,-phi);
            
      n=R.GetDim();

      assert(n==_nC);

      for(i=0;i<n;i++){
	for(j=0;j<n;j++){
	  _RC[i][j]=R.Get(i,j);
	}
      }

    }

    break;

  default:
    report(ERROR,"EvtGen") << "Spin2(_JC2)="<<_JC2<<" not supported!"<<endl;
    ::abort();
  }
  
  

}


void EvtPartWave::applyRotationMatrices(){

  int ia,ib,ic,i;
  
  EvtComplex temp;



  for(ia=0;ia<_nA;ia++){
    for(ib=0;ib<_nB;ib++){
      for(ic=0;ic<_nC;ic++){
	temp=0;
	for(i=0;i<_nC;i++){
	  temp+=_RC[ic][i]*_amp[ia][ib][i];
	}
	_amp1[ia][ib][ic]=temp;
      }
    }
  }



  for(ia=0;ia<_nA;ia++){
    for(ic=0;ic<_nC;ic++){
      for(ib=0;ib<_nB;ib++){
  	temp=0;
  	for(i=0;i<_nB;i++){
  	  temp+=_RB[ib][i]*_amp1[ia][i][ic];
  	}
  	_amp3[ia][ib][ic]=temp;
      }
    }
  }
  


  for(ib=0;ib<_nB;ib++){
    for(ic=0;ic<_nC;ic++){
      for(ia=0;ia<_nA;ia++){
	temp=0;
	for(i=0;i<_nA;i++){
	  temp+=_RA[i][ia]*_amp3[i][ib][ic];
	}
	_amp[ia][ib][ic]=temp;
      }
    }
  }


}








