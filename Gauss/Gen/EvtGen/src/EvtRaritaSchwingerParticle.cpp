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
// Module: EvtRaritaSchwingerParticle.cc
//
// Description: Class to describe spin 1/2 particles.
//
// Modification history:
//
//   RYD     August 9, 2000         Module created
//
//------------------------------------------------------------------------
// 
#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtRaritaSchwingerParticle.hh"
#include "EvtGenBase/EvtDiracSpinor.hh"
#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtReport.hh"

EvtRaritaSchwingerParticle::~EvtRaritaSchwingerParticle(){}


EvtRaritaSchwingerParticle::EvtRaritaSchwingerParticle(){

  return;
  
}

void EvtRaritaSchwingerParticle::init(EvtId id,const EvtVector4R& p4){

  _validP4=true;
  setp(p4);
  setpart_num(id);

  if (EvtPDL::getStdHep(id)==0){
    report(ERROR,"EvtGen") << "Error in EvtRaritaSchwingerParticle::init, part_n="
                           << id.getId()<<std::endl;
    ::abort();
  }

  static EvtVector4R e1(0.0,1.0,0.0,0.0);
  static EvtVector4R e2(0.0,0.0,1.0,0.0);
  static EvtVector4R e3(0.0,0.0,0.0,1.0);
  

  if (EvtPDL::getStdHep(id)>0){  

    EvtDiracSpinor u1,u2;

    u1.set(EvtComplex(sqrt(2.0*mass()),0.0),EvtComplex(0.0,0.0),
	   EvtComplex(0.0,0.0),EvtComplex(0.0,0.0));
    u2.set(EvtComplex(0.0,0.0),EvtComplex(sqrt(2.0*mass()),0.0),
	   EvtComplex(0.0,0.0),EvtComplex(0.0,0.0));

    

    _spinorRest[0]=dirProd(e3,u1+u2);
    _spinorRest[1]=dirProd(e1+EvtComplex(0.0,1.0)*e2,u1);
    _spinorRest[2]=dirProd(e1-EvtComplex(0.0,1.0)*e2,u2);
    _spinorRest[3]=dirProd(e1,(u1+u2))+dirProd(EvtComplex(0.0,1.0)*e2,(u1-u2));
    
    _spinor[0]=boostTo(_spinorRest[0],p4);
    _spinor[1]=boostTo(_spinorRest[1],p4);
    _spinor[2]=boostTo(_spinorRest[2],p4);
    _spinor[3]=boostTo(_spinorRest[3],p4);


  }
  else{

    EvtDiracSpinor u1,u2;

    u1.set(EvtComplex(0.0,0.0),EvtComplex(0.0,0.0),
	   EvtComplex(sqrt(2.0*mass()),0.0),EvtComplex(0.0,0.0));
    u2.set(EvtComplex(0.0,0.0),EvtComplex(0.0,0.0),
	   EvtComplex(0.0,0.0),EvtComplex(sqrt(2.0*mass()),0.0));

    

    _spinorRest[0]=dirProd(e3,(u1+u2));
    _spinorRest[1]=dirProd(e1+EvtComplex(0.0,1.0)*e2,u1);
    _spinorRest[2]=dirProd(e1-EvtComplex(0.0,1.0)*e2,u2);
    _spinorRest[3]=dirProd(e1,(u1+u2))+dirProd(EvtComplex(0.0,1.0)*e2,(u1-u2));
    
    _spinor[0]=boostTo(_spinorRest[0],p4);
    _spinor[1]=boostTo(_spinorRest[1],p4);
    _spinor[2]=boostTo(_spinorRest[2],p4);
    _spinor[3]=boostTo(_spinorRest[3],p4);

  }

  setLifetime();
}


EvtRaritaSchwinger EvtRaritaSchwingerParticle::spRSParent(int i) const {

  return _spinor[i];

}

EvtRaritaSchwinger EvtRaritaSchwingerParticle::spRS(int i) const {

  return _spinorRest[i];

}



EvtSpinDensity EvtRaritaSchwingerParticle::rotateToHelicityBasis() const{
  
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

  EvtVector4C eplus(0.0,-1.0/sqrt(2.0),EvtComplex(0.0,-1.0/sqrt(2.0)),0.0);
  EvtVector4C ezero(0.0,0.0,0.0,1.0);
  EvtVector4C eminus(0.0,1.0/sqrt(2.0),EvtComplex(0.0,-1.0/sqrt(2.0)),0.0);

  EvtRaritaSchwinger sppp=dirProd(eplus,spplus);
  EvtRaritaSchwinger spp=dirProd(sqrt(2.0/3.0)*ezero,spplus)+
    dirProd(sqrt(1.0/3.0)*eplus,spminus);
  EvtRaritaSchwinger spm=dirProd(sqrt(2.0/3.0)*ezero,spminus)+
    dirProd(sqrt(1.0/3.0)*eminus,spplus);
  EvtRaritaSchwinger spmm=dirProd(eminus,spminus);

    
  EvtRaritaSchwinger sp0=spRS(0);
  EvtRaritaSchwinger sp1=spRS(1);
  EvtRaritaSchwinger sp2=spRS(2);
  EvtRaritaSchwinger sp3=spRS(3);

  EvtSpinDensity R;
  R.SetDim(4);
      
  R.Set(0,0,(sppp*sp0)/sqrt(2*m));
  R.Set(0,1,(sppp*sp1)/sqrt(2*m));
  R.Set(0,2,(sppp*sp2)/sqrt(2*m));
  R.Set(0,3,(sppp*sp3)/sqrt(2*m));

  R.Set(1,0,(spp*sp0)/sqrt(2*m));
  R.Set(1,1,(spp*sp1)/sqrt(2*m));
  R.Set(1,2,(spp*sp2)/sqrt(2*m));
  R.Set(1,3,(spp*sp3)/sqrt(2*m));

  R.Set(2,0,(spm*sp0)/sqrt(2*m));
  R.Set(2,1,(spm*sp1)/sqrt(2*m));
  R.Set(2,2,(spm*sp2)/sqrt(2*m));
  R.Set(2,3,(spm*sp3)/sqrt(2*m));

  R.Set(3,0,(spmm*sp0)/sqrt(2*m));
  R.Set(3,1,(spmm*sp1)/sqrt(2*m));
  R.Set(3,2,(spmm*sp2)/sqrt(2*m));
  R.Set(3,3,(spmm*sp3)/sqrt(2*m));

  return R;

}


EvtSpinDensity EvtRaritaSchwingerParticle::rotateToHelicityBasis(double alpha,
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

  EvtVector4C eplus(0.0,-1.0/sqrt(2.0),EvtComplex(0.0,-1.0/sqrt(2.0)),0.0);
  EvtVector4C ezero(0.0,0.0,0.0,1.0);
  EvtVector4C eminus(0.0,1.0/sqrt(2.0),EvtComplex(0.0,-1.0/sqrt(2.0)),0.0);

  EvtRaritaSchwinger sppp=dirProd(eplus,spplus);
  EvtRaritaSchwinger spp=dirProd(sqrt(2.0/3.0)*ezero,spplus)+
    dirProd(sqrt(1.0/3.0)*eplus,spminus);
  EvtRaritaSchwinger spm=dirProd(sqrt(2.0/3.0)*ezero,spminus)+
    dirProd(sqrt(1.0/3.0)*eminus,spplus);
  EvtRaritaSchwinger spmm=dirProd(eminus,spminus);

  sppp.applyRotateEuler(alpha,beta,gamma);
  spp.applyRotateEuler(alpha,beta,gamma);
  spm.applyRotateEuler(alpha,beta,gamma);
  spmm.applyRotateEuler(alpha,beta,gamma);

    
  EvtRaritaSchwinger sp0=spRS(0);
  EvtRaritaSchwinger sp1=spRS(1);
  EvtRaritaSchwinger sp2=spRS(2);
  EvtRaritaSchwinger sp3=spRS(3);

  EvtSpinDensity R;
  R.SetDim(4);
      
  R.Set(0,0,(sppp*sp0)/sqrt(2*m));
  R.Set(0,1,(sppp*sp1)/sqrt(2*m));
  R.Set(0,2,(sppp*sp2)/sqrt(2*m));
  R.Set(0,3,(sppp*sp3)/sqrt(2*m));

  R.Set(1,0,(spp*sp0)/sqrt(2*m));
  R.Set(1,1,(spp*sp1)/sqrt(2*m));
  R.Set(1,2,(spp*sp2)/sqrt(2*m));
  R.Set(1,3,(spp*sp3)/sqrt(2*m));

  R.Set(2,0,(spm*sp0)/sqrt(2*m));
  R.Set(2,1,(spm*sp1)/sqrt(2*m));
  R.Set(2,2,(spm*sp2)/sqrt(2*m));
  R.Set(2,3,(spm*sp3)/sqrt(2*m));

  R.Set(3,0,(spmm*sp0)/sqrt(2*m));
  R.Set(3,1,(spmm*sp1)/sqrt(2*m));
  R.Set(3,2,(spmm*sp2)/sqrt(2*m));
  R.Set(3,3,(spmm*sp3)/sqrt(2*m));

  return R;

}

