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
// Module: EvtDiracParticle.cc
//
// Description: Class to describe spin 1/2 particles.
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include <iostream.h>
#include <math.h>
#include "EvtGen/EvtComplex.hh"
#include "EvtGen/EvtDiracParticle.hh"
#include "EvtGen/EvtVector4R.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtSpinDensity.hh"
#include "EvtGen/EvtGammaMatrix.hh"

EvtDiracParticle::~EvtDiracParticle(){}


EvtDiracParticle::EvtDiracParticle(){

  return;
  
}

void EvtDiracParticle::init(EvtId part_n,const EvtVector4R& p4){

  setp(p4);
  setpart_num(part_n);

  if (EvtPDL::getStdHep(part_n)==0){
    report(ERROR,"EvtGen") << "Error in EvtDiracParticle::init, part_n="
                           << part_n.getId()<<endl;
    ::abort();
  }

  if (EvtPDL::getStdHep(part_n)>0){  

    spinor1_rest.set(EvtComplex(sqrt(2.0*mass()),0.0),EvtComplex(0.0,0.0),
		     EvtComplex(0.0,0.0),EvtComplex(0.0,0.0));
    spinor2_rest.set(EvtComplex(0.0,0.0),EvtComplex(sqrt(2.0*mass()),0.0),
		     EvtComplex(0.0,0.0),EvtComplex(0.0,0.0));
  
    spinor1_parent=boostTo(spinor1_rest,p4);
    spinor2_parent=boostTo(spinor2_rest,p4);


  }
  else{

    spinor1_rest.set(EvtComplex(0.0,0.0),EvtComplex(0.0,0.0),
		     EvtComplex(sqrt(2.0*mass()),0.0),EvtComplex(0.0,0.0));
    spinor2_rest.set(EvtComplex(0.0,0.0),EvtComplex(0.0,0.0),
		     EvtComplex(0.0,0.0),EvtComplex(sqrt(2.0*mass()),0.0));
  
    spinor1_parent=boostTo(spinor1_rest,p4);
    spinor2_parent=boostTo(spinor2_rest,p4);



  }

  setLifetime();
}


EvtDiracSpinor EvtDiracParticle::spParent(int i) const {

  EvtDiracSpinor temp;

  switch(i) {

  case 0:
    return spinor1_parent;
  case 1:
    return spinor2_parent;
  default:
    report(ERROR,"EvtGen") <<"Error invalid spinor number "
			   <<i<<endl;
    ::abort();
  }

  return temp;
  
}

EvtDiracSpinor EvtDiracParticle::sp(int i) const {

  EvtDiracSpinor temp;
  switch(i) {

  case 0:
    return spinor1_rest;
  case 1:
    return spinor2_rest;
  default:
    report(ERROR,"EvtGen") <<"Error invalid spinor number "
			   <<i<<endl;
    ::abort();
  }
  
  return temp;

}


EvtSpinDensity EvtDiracParticle::rotateToHelicityBasis() const{

  EvtDiracSpinor spplus;
  EvtDiracSpinor spminus;
      
  double m=getP4().mass();
      
  if (EvtPDL::getStdHep(getId())>0){  
    spplus.set(1.0,0.0,0.0,0.0);
    spminus.set(0.0,1.0,0.0,0.0);
  } else {
    spplus.set(0.0,0.0,0.0,1.0);
    spminus.set(0.0,0.0,1.0,0.0);
  }
      
  EvtDiracSpinor sp0=sp(0);
  EvtDiracSpinor sp1=sp(1);

  EvtSpinDensity R;
  R.SetDim(2);
      
  R.Set(0,0,(spplus*sp0)/sqrt(2*m));
  R.Set(0,1,(spplus*sp1)/sqrt(2*m));
  R.Set(1,0,(spminus*sp0)/sqrt(2*m));
  R.Set(1,1,(spminus*sp1)/sqrt(2*m));

  return R;

}


EvtSpinDensity EvtDiracParticle::rotateToHelicityBasis(double alpha,
						       double beta,
						       double gamma) const{

  EvtDiracSpinor spplus;
  EvtDiracSpinor spminus;
      
  double m=getP4().mass();
      
  if (EvtPDL::getStdHep(getId())>0){  
    spplus.set(1.0,0.0,0.0,0.0);
    spminus.set(0.0,1.0,0.0,0.0);
  } else {
    spplus.set(0.0,0.0,0.0,1.0);
    spminus.set(0.0,0.0,1.0,0.0);
  }
      
  EvtDiracSpinor sp0=sp(0);
  EvtDiracSpinor sp1=sp(1);
  
  spplus.applyRotateEuler(alpha,beta,gamma);
  spminus.applyRotateEuler(alpha,beta,gamma);


  EvtSpinDensity R;
  R.SetDim(2);
      
  R.Set(0,0,(spplus*sp0)/sqrt(2*m));
  R.Set(0,1,(spplus*sp1)/sqrt(2*m));
  R.Set(1,0,(spminus*sp0)/sqrt(2*m));
  R.Set(1,1,(spminus*sp1)/sqrt(2*m));

  return R;

}


