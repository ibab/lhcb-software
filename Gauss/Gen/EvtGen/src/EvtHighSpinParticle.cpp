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
// Module: EvtHighSpinParticle.cc
//
// Description: Class to describe particles with spin>2.
//
// Modification history:
//
//    RYD   August 8, 2000           Module created
//
//------------------------------------------------------------------------
// 
#include <iostream.h>
#include <math.h>
#include <assert.h>
#include "EvtGen/EvtHighSpinParticle.hh"
#include "EvtGen/EvtVector4R.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtSpinDensity.hh"
#include "EvtGen/EvtdFunction.hh"


EvtHighSpinParticle::~EvtHighSpinParticle() {}


void EvtHighSpinParticle::init(EvtId id,const EvtVector4R& p4){

  setp(p4);
  setpart_num(id);

  setLifetime();

}

EvtSpinDensity EvtHighSpinParticle::rotateToHelicityBasis() const{

  int n=EvtSpinType::getSpinStates(EvtPDL::getSpinType(getId()));

  EvtSpinDensity R;
  R.SetDiag(n);

  return R;

}



EvtSpinDensity EvtHighSpinParticle::rotateToHelicityBasis(double alpha,
							  double beta,
							  double gamma) const{

  int i,j;
  
  int n=EvtSpinType::getSpinStates(EvtPDL::getSpinType(getId()));

  EvtSpinDensity R;
  R.SetDim(n);

  int J2=EvtSpinType::getSpin2(EvtPDL::getSpinType(getId()));

  assert(n==J2+1);

  int *lambda2;

  lambda2=new int[J2+1];

  for(i=0;i<J2+1;i++){
    lambda2[i]=J2-i*2;
  }


  for(i=0;i<n;i++){
    for(j=0;j<n;j++){
      R.Set(i,j,EvtdFunction::d(J2,lambda2[j],lambda2[i],-beta)*
	exp(EvtComplex(0.0,-0.5*(alpha*lambda2[i]-gamma*lambda2[j]))));
    }
  }

  delete [] lambda2;

  return R;


}


