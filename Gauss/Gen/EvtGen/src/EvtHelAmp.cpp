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
#include <stdlib.h>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtVector4C.hh"
#include "EvtGen/EvtTensor4C.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtHelAmp.hh"
#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtConst.hh"
#include "EvtGen/EvtdFunction.hh"
#include "EvtGen/EvtDiracSpinor.hh"
#include "EvtGen/EvtGammaMatrix.hh"


EvtHelAmp::~EvtHelAmp() {

  //free memory
  delete [] _lambdaA2;
  delete [] _lambdaB2;
  delete [] _lambdaC2;

  int ia,ib,ic;
  for(ib=0;ib<_nB;ib++){
    delete [] _HBC[ib];
  }

  delete [] _HBC;

  for(ia=0;ia<_nA;ia++){
    delete [] _RA[ia];
  }
  delete [] _RA;


  for(ib=0;ib<_nB;ib++){
    delete [] _RB[ib];
  }
  delete [] _RB;

  for(ic=0;ic<_nC;ic++){
    delete [] _RC[ic];
  }
  delete [] _RC;


  
  for(ia=0;ia<_nA;ia++){
    for(ib=0;ib<_nB;ib++){
      delete [] _amp[ia][ib];
      delete [] _amp1[ia][ib];
      delete [] _amp3[ia][ib];
    }
    delete [] _amp[ia];
    delete [] _amp1[ia];
    delete [] _amp3[ia];
  }  

  delete [] _amp;
  delete [] _amp1;
  delete [] _amp3;


}

void EvtHelAmp::getName(EvtString& model_name){

  model_name="HELAMP";     

}


EvtDecayBase* EvtHelAmp::clone(){

  return new EvtHelAmp;

}

void EvtHelAmp::init(){

  checkNDaug(2);


  //find out how many states each particle have
  _nA=EvtSpinType::getSpinStates(EvtPDL::getSpinType(getParentId()));
  _nB=EvtSpinType::getSpinStates(EvtPDL::getSpinType(getDaug(0)));
  _nC=EvtSpinType::getSpinStates(EvtPDL::getSpinType(getDaug(1)));

  _idA=getParentId();
  _idB=getDaug(0);
  _idC=getDaug(1);

  if (verbose()){
    report(INFO,"EvtGen")<<"_nA,_nB,_nC:"
			 <<_nA<<","<<_nB<<","<<_nC<<endl;
  }

  //find out what 2 times the spin is
  _JA2=EvtSpinType::getSpin2(EvtPDL::getSpinType(getParentId()));
  _JB2=EvtSpinType::getSpin2(EvtPDL::getSpinType(getDaug(0)));
  _JC2=EvtSpinType::getSpin2(EvtPDL::getSpinType(getDaug(1)));

  if (verbose()){
    report(INFO,"EvtGen")<<"_JA2,_JB2,_JC2:"
			 <<_JA2<<","<<_JB2<<","<<_JC2<<endl;
  }

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

  if (verbose()){
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
			       <<_HBC[ib][ic]<<endl;
	}
      }
    }
  }

}


void EvtHelAmp::initProbMax(){

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

  //report(INFO,"EvtGen")<<"Calculated probmax"<<maxprob<<endl;
  
  setProbMax(maxprob);

}


void EvtHelAmp::decay( EvtParticle *p){

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


void EvtHelAmp::setUpRotationMatrices(EvtParticle* p,double theta, double phi){

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


void EvtHelAmp::applyRotationMatrices(){

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








