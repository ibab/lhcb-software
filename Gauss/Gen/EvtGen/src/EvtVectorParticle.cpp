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
// Module: EvtVectorParticle.cc
//
// Description: Class to describe spin 1 particles
//
// Modification history:
//
//    DJL/RYD   September 25, 1996           Module created
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "EvtGen/EvtComplex.hh"
#include "EvtGen/EvtVectorParticle.hh"
#include "EvtGen/EvtVector4C.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"

EvtVectorParticle::~EvtVectorParticle(){}


void EvtVectorParticle::init(EvtId part_n,double e,double px,double py,double pz){

  setp(e,px,py,pz);
  setpart_num(part_n);
  
  eps1.set(0.0,1.0,0.0,0.0);
  eps2.set(0.0,0.0,1.0,0.0);
  eps3.set(0.0,0.0,0.0,1.0);

  setLifetime();
}

void EvtVectorParticle::init(EvtId part_n,const EvtVector4R& p4){

  setp(p4);
  setpart_num(part_n);
  
  eps1.set(0.0,1.0,0.0,0.0);
  eps2.set(0.0,0.0,1.0,0.0);
  eps3.set(0.0,0.0,0.0,1.0);

  setLifetime();
}

EvtVector4C EvtVectorParticle::epsParent(int i) const  {

  EvtVector4C temp;
  
  switch(i) {
  case 0:
    temp=eps1;
    break;
  case 1:
    temp=eps2;
    break;
  case 2:
    temp=eps3;
    break;
  default:
    report(ERROR,"EvtGen") <<"Error invalid vector number"
			   <<i<<std::endl;
    ::abort();
  }

  return boostTo(temp,this->getP4());
}

EvtVector4C EvtVectorParticle::eps(int i) const {
  
  switch(i) {
  case 0:
    return eps1;
  case 1:
    return eps2;
  case 2:
    return eps3;
  default:
    report(ERROR,"EvtGen") << "Error invalid number vector number:"
			   <<i<<std::endl;
    ::abort();
  }

  return eps1;
}


EvtSpinDensity EvtVectorParticle::rotateToHelicityBasis() const{

  EvtVector4C eplus(0.0,-1.0/sqrt(2.0),EvtComplex(0.0,-1.0/sqrt(2.0)),0.0);
  EvtVector4C ezero(0.0,0.0,0.0,1.0);
  EvtVector4C eminus(0.0,1.0/sqrt(2.0),EvtComplex(0.0,-1.0/sqrt(2.0)),0.0);
  
  EvtVector4C e1=eps(0);
  EvtVector4C e2=eps(1);
  EvtVector4C e3=eps(2);
  
  EvtSpinDensity R;
  R.SetDim(3);

  R.Set(0,0,(eplus.conj())*e1);
  R.Set(0,1,(eplus.conj())*e2);
  R.Set(0,2,(eplus.conj())*e3);
  
  R.Set(1,0,(ezero.conj())*e1);
  R.Set(1,1,(ezero.conj())*e2);
  R.Set(1,2,(ezero.conj())*e3);
  
  R.Set(2,0,(eminus.conj())*e1);
  R.Set(2,1,(eminus.conj())*e2);
  R.Set(2,2,(eminus.conj())*e3);

  return R;

}


EvtSpinDensity EvtVectorParticle::rotateToHelicityBasis(double alpha,
							double beta,
							double gamma) const{

  EvtVector4C eplus(0.0,-1.0/sqrt(2.0),EvtComplex(0.0,-1.0/sqrt(2.0)),0.0);
  EvtVector4C ezero(0.0,0.0,0.0,1.0);
  EvtVector4C eminus(0.0,1.0/sqrt(2.0),EvtComplex(0.0,-1.0/sqrt(2.0)),0.0);
  
  eplus.applyRotateEuler(alpha,beta,gamma);
  ezero.applyRotateEuler(alpha,beta,gamma);
  eminus.applyRotateEuler(alpha,beta,gamma);

  EvtVector4C e1=eps(0);
  EvtVector4C e2=eps(1);
  EvtVector4C e3=eps(2);
  
  EvtSpinDensity R;
  R.SetDim(3);

  R.Set(0,0,(eplus.conj())*e1);
  R.Set(0,1,(eplus.conj())*e2);
  R.Set(0,2,(eplus.conj())*e3);
  
  R.Set(1,0,(ezero.conj())*e1);
  R.Set(1,1,(ezero.conj())*e2);
  R.Set(1,2,(ezero.conj())*e3);
  
  R.Set(2,0,(eminus.conj())*e1);
  R.Set(2,1,(eminus.conj())*e2);
  R.Set(2,2,(eminus.conj())*e3);

  return R;

}


