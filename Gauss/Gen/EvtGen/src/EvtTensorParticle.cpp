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
// Module: EvtTensorParticle.cc
//
// Description: Class to describe spin 2 particles.
//
// Modification history:
//
//    DJL/RYD   September 25,1996           Module created
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
#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtTensor4C.hh"
#include "EvtGenBase/EvtVector4C.hh"
#include "EvtGenBase/EvtTensorParticle.hh"
#include "EvtGenBase/EvtReport.hh"

EvtTensorParticle::~EvtTensorParticle(){}

void EvtTensorParticle::init(EvtId part_n,const EvtVector4R& p4){
 
  init(part_n,p4.get(0),p4.get(1)
       ,p4.get(2),p4.get(3));

  setLifetime();

  
}

void EvtTensorParticle::init(EvtId part_n,double e,double px,double py,double pz){

  _validP4=true;
  setp(e,px,py,pz);
  setpart_num(part_n);
  
  eps1.setdiag(0.0,-1.0/sqrt(6.0),-1.0/sqrt(6.0),
	       2.0/sqrt(6.0));
  eps2.setdiag(0.0,1.0/sqrt(2.0),-1.0/sqrt(2.0),0.0);
  eps3.setdiag(0.0,0.0,0.0,0.0);
  eps4.setdiag(0.0,0.0,0.0,0.0);
  eps5.setdiag(0.0,0.0,0.0,0.0);
  eps3.set(0,1,EvtComplex(0.0,0.0));
  eps3.set(0,2,EvtComplex(0.0,0.0));
  eps3.set(0,3,EvtComplex(0.0,0.0));
  eps3.set(1,0,EvtComplex(0.0,0.0));
  eps3.set(2,0,EvtComplex(0.0,0.0));
  eps3.set(3,0,EvtComplex(0.0,0.0));
  eps4.set(0,1,EvtComplex(0.0,0.0));
  eps4.set(0,2,EvtComplex(0.0,0.0));
  eps4.set(0,3,EvtComplex(0.0,0.0));
  eps4.set(1,0,EvtComplex(0.0,0.0));
  eps4.set(2,0,EvtComplex(0.0,0.0));
  eps4.set(3,0,EvtComplex(0.0,0.0));
  eps5.set(0,1,EvtComplex(0.0,0.0));
  eps5.set(0,2,EvtComplex(0.0,0.0));
  eps5.set(0,3,EvtComplex(0.0,0.0));
  eps5.set(1,0,EvtComplex(0.0,0.0));
  eps5.set(2,0,EvtComplex(0.0,0.0));
  eps5.set(3,0,EvtComplex(0.0,0.0));
  eps3.set(1,2,EvtComplex(1.0/sqrt(2.0),0.0));
  eps3.set(2,1,EvtComplex(1.0/sqrt(2.0),0.0));
  eps4.set(1,3,EvtComplex(1.0/sqrt(2.0),0.0));
  eps4.set(3,1,EvtComplex(1.0/sqrt(2.0),0.0));
  eps5.set(2,3,EvtComplex(1.0/sqrt(2.0),0.0));
  eps5.set(3,2,EvtComplex(1.0/sqrt(2.0),0.0));
  eps3.set(1,3,EvtComplex(0.0,0.0));
  eps3.set(3,1,EvtComplex(0.0,0.0));
  eps3.set(2,3,EvtComplex(0.0,0.0));
  eps3.set(3,2,EvtComplex(0.0,0.0));
  eps4.set(1,2,EvtComplex(0.0,0.0));
  eps4.set(2,1,EvtComplex(0.0,0.0));
  eps4.set(2,3,EvtComplex(0.0,0.0));
  eps4.set(3,2,EvtComplex(0.0,0.0));
  eps5.set(1,3,EvtComplex(0.0,0.0));
  eps5.set(3,1,EvtComplex(0.0,0.0));
  eps5.set(2,1,EvtComplex(0.0,0.0));
  eps5.set(1,2,EvtComplex(0.0,0.0));

  setLifetime();
  
}

EvtTensor4C EvtTensorParticle::epsTensorParent(int i) const {

  EvtTensor4C temp;
  //  EvtVector4R p4_temp;

  switch (i){
  case 0:
    temp = eps1;
    break;
  case 1:
    temp = eps2;
    break;
  case 2:
    temp = eps3;
    break;
  case 3:
    temp = eps4;
    break;
  case 4:
    temp = eps5;
    break;
  default:
    report(ERROR,"EvtGen") << "only 5 componets in EvtTensorParticle. ERROR \n";
    ::abort();
  } // switch

  temp.applyBoostTo(this->getP4());
  return temp;
  
} //epsParent


EvtTensor4C EvtTensorParticle::epsTensor(int i) const {
   
  EvtTensor4C temp;

  switch (i){
  case 0:
    return eps1;
  case 1:
    return eps2;
  case 2:
    return eps3;
  case 3:
    return eps4;
  case 4:
    return eps5;
  default:
    report(ERROR,"EvtGen") << "only 5 componets in EvtTensorParticle. ERROR \n";
    ::abort();
  } // switch

  return temp;

} //eps



EvtSpinDensity EvtTensorParticle::rotateToHelicityBasis() const{


  EvtTensor4C epp,ep,ez,em,emm;

  EvtVector4C eplus(0.0,-1.0/sqrt(2.0),EvtComplex(0.0,-1.0/sqrt(2.0)),0.0);
  EvtVector4C ezero(0.0,0.0,0.0,1.0);
  EvtVector4C eminus(0.0,1.0/sqrt(2.0),EvtComplex(0.0,-1.0/sqrt(2.0)),0.0);
      
  epp.zero();
  ep.zero();
  ez.zero();
  em.zero();
  emm.zero();
  
  epp=directProd(eplus,eplus);
  ep =(1/sqrt(2.0))*directProd(eplus,ezero)
    +(1/sqrt(2.0))*directProd(ezero,eplus);
  ez =(1/sqrt(6.0))*directProd(eplus,eminus)
    +(2/sqrt(6.0))*directProd(ezero,ezero)
    +(1/sqrt(6.0))*directProd(eminus,eplus);
  em =(1/sqrt(2.0))*directProd(eminus,ezero)
    +(1/sqrt(2.0))*directProd(ezero,eminus);
  emm=directProd(eminus,eminus);
      
  epp=conj(epp);
  ep=conj(ep);
  ez=conj(ez);
  em=conj(em);
  emm=conj(emm);

  EvtTensor4C e1=epsTensor(0);
  EvtTensor4C e2=epsTensor(1);
  EvtTensor4C e3=epsTensor(2);
  EvtTensor4C e4=epsTensor(3);
  EvtTensor4C e5=epsTensor(4);

  EvtSpinDensity R;
  R.SetDim(5);

  R.Set(0,0,cont(epp,e1));      
  R.Set(0,1,cont(epp,e2));      
  R.Set(0,2,cont(epp,e3));      
  R.Set(0,3,cont(epp,e4));      
  R.Set(0,4,cont(epp,e5));      

  R.Set(1,0,cont(ep,e1));      
  R.Set(1,1,cont(ep,e2));      
  R.Set(1,2,cont(ep,e3));      
  R.Set(1,3,cont(ep,e4));      
  R.Set(1,4,cont(ep,e5));      

  R.Set(2,0,cont(ez,e1));      
  R.Set(2,1,cont(ez,e2));      
  R.Set(2,2,cont(ez,e3));      
  R.Set(2,3,cont(ez,e4));      
  R.Set(2,4,cont(ez,e5));      

  R.Set(3,0,cont(em,e1));      
  R.Set(3,1,cont(em,e2));      
  R.Set(3,2,cont(em,e3));      
  R.Set(3,3,cont(em,e4));      
  R.Set(3,4,cont(em,e5));      

  R.Set(4,0,cont(emm,e1));      
  R.Set(4,1,cont(emm,e2));      
  R.Set(4,2,cont(emm,e3));      
  R.Set(4,3,cont(emm,e4));      
  R.Set(4,4,cont(emm,e5));      

  return R;

}


EvtSpinDensity EvtTensorParticle::rotateToHelicityBasis(double alpha,
							double beta,
							double gamma) const{

  EvtTensor4C epp,ep,ez,em,emm;

  EvtVector4C eplus(0.0,-1.0/sqrt(2.0),EvtComplex(0.0,-1.0/sqrt(2.0)),0.0);
  EvtVector4C ezero(0.0,0.0,0.0,1.0);
  EvtVector4C eminus(0.0,1.0/sqrt(2.0),EvtComplex(0.0,-1.0/sqrt(2.0)),0.0);

  eplus.applyRotateEuler(alpha,beta,gamma);
  ezero.applyRotateEuler(alpha,beta,gamma);
  eminus.applyRotateEuler(alpha,beta,gamma);

      
  epp.zero();
  ep.zero();
  ez.zero();
  em.zero();
  emm.zero();
  
  epp=directProd(eplus,eplus);
  ep =(1/sqrt(2.0))*directProd(eplus,ezero)
    +(1/sqrt(2.0))*directProd(ezero,eplus);
  ez =(1/sqrt(6.0))*directProd(eplus,eminus)
    +(2/sqrt(6.0))*directProd(ezero,ezero)
    +(1/sqrt(6.0))*directProd(eminus,eplus);
  em =(1/sqrt(2.0))*directProd(eminus,ezero)
    +(1/sqrt(2.0))*directProd(ezero,eminus);
  emm=directProd(eminus,eminus);


      
  //epp.applyRotateEuler(alpha,beta,gamma);
  //ep.applyRotateEuler(alpha,beta,gamma);
  //ez.applyRotateEuler(alpha,beta,gamma);
  //em.applyRotateEuler(alpha,beta,gamma);
  //emm.applyRotateEuler(alpha,beta,gamma);
      
  epp=conj(epp);
  ep=conj(ep);
  ez=conj(ez);
  em=conj(em);
  emm=conj(emm);

  EvtTensor4C e1=epsTensor(0);
  EvtTensor4C e2=epsTensor(1);
  EvtTensor4C e3=epsTensor(2);
  EvtTensor4C e4=epsTensor(3);
  EvtTensor4C e5=epsTensor(4);

  EvtSpinDensity R;
  R.SetDim(5);

  R.Set(0,0,cont(epp,e1));      
  R.Set(0,1,cont(epp,e2));      
  R.Set(0,2,cont(epp,e3));      
  R.Set(0,3,cont(epp,e4));      
  R.Set(0,4,cont(epp,e5));      

  R.Set(1,0,cont(ep,e1));      
  R.Set(1,1,cont(ep,e2));      
  R.Set(1,2,cont(ep,e3));      
  R.Set(1,3,cont(ep,e4));      
  R.Set(1,4,cont(ep,e5));      

  R.Set(2,0,cont(ez,e1));      
  R.Set(2,1,cont(ez,e2));      
  R.Set(2,2,cont(ez,e3));      
  R.Set(2,3,cont(ez,e4));      
  R.Set(2,4,cont(ez,e5));      

  R.Set(3,0,cont(em,e1));      
  R.Set(3,1,cont(em,e2));      
  R.Set(3,2,cont(em,e3));      
  R.Set(3,3,cont(em,e4));      
  R.Set(3,4,cont(em,e5));      

  R.Set(4,0,cont(emm,e1));      
  R.Set(4,1,cont(emm,e2));      
  R.Set(4,2,cont(emm,e3));      
  R.Set(4,3,cont(emm,e4));      
  R.Set(4,4,cont(emm,e5));      

  return R;

}

  






