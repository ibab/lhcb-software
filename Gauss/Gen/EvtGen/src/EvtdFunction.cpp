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
// Module: EvtKine.cc
//
// Description: Evaluates the Wigner d-Functions.
//
// Modification history:
//
//    RYD            March 14, 1999         Module created
//
//------------------------------------------------------------------------
// 
#include <stdlib.h>
#include <math.h>
#include <iostream.h>
#include <assert.h>
#include "EvtGen/EvtdFunction.hh"
#include "EvtGen/EvtdFunctionSingle.hh"


double EvtdFunction::d(int j,int m1,int m2, double theta){

  //cout << "j,m1,m2:"<<j<<","<<m1<<","<<m2<<endl;

  double c=cos(theta);
  double c2=cos(0.5*theta);
  double s=sin(theta);
  double s2=sin(0.5*theta);

  int m1p=m1;
  int m2p=m2;

  //cout << "here10"<<endl;

  int signp=1;
  //make |m2p|>|m1p|
  if (abs(m2p)<abs(m1p)) {
    int tmp=m1p;
    m1p=m2p;
    m2p=tmp;
    if ((m1p-m2p)%4!=0) signp=-signp;
  } 

  //cout << "here11"<<endl;

  //make m2p non-negative
  if (m2p<0) {
    m1p=-m1p;
    m2p=-m2p;
    if ((m1p-m2p)%4!=0) signp=-signp;
  }

  //cout << "here1"<<endl;

  EvtdFunctionSingle df;

  //cout << "here2"<<endl;

  df.init(j,m1p,m2p);

  //cout << "here3"<<endl;
  

  double dtmp=df.d(j,m1p,m2p,theta)*signp;
  //cout << "new:"<<dtmp<<endl;


  int sign=1;
  
  //make |m1|>|m2|
  if (abs(m1)<abs(m2)) {
    int tmp=m1;
    m1=m2;
    m2=tmp;
    if ((m1-m2)%4!=0) sign=-sign;
  } 

  //make m1 non-negative
  if (m1<0) {
    m1=-m1;
    m2=-m2;
    if ((m1-m2)%4!=0) sign=-sign;
  }


  double d=0.0;

  switch (j){
    
  case 0:  // j=0
    d=1.0;
    break;
  case 1:  // j=1/2

    switch (m1){

    case 1: //m1=1/2

      switch (m2) {
	
      case 1: //m2=1/2

	d=c2;
	break;

      case -1: //m2=-1/2
	
	d=-s2;
	break;

      default:
	::abort();
	
      }

      break;

    default:
      ::abort();
      
    }
    break;

  case 2: //j=1

    switch (m1){
      
    case 0: //m1=0

      d=c;
      break;
      
    case 2: //m1=1

      switch (m2){
	
      case 2:  //m2=1
	
	d=0.5*(1+c);
	break;

      case 0:  //m2=0
	
	d=-s/sqrt(2.0);
	break;

      case -2:  //m2=1
	
	d=0.5*(1-c);
	break;
	
      }
      break;
      
    default:
      ::abort();

    }
    break;

  case 4: //j=2

    switch (m1){

    case 4: //m1=2

      switch (m2){

      case 4: //m2=2
	d=0.25*(1+c)*(1+c);
	break;

      case 2: //m2=1
	d=-0.5*(1+c)*s;
	break;

      case 0: //m2=0
	d=0.25*sqrt(6.0)*s*s;
	break;

      case -2: //m2=-1
	d=-0.5*(1-c)*s;
	break;

      case -4: //m2=-2
	d=0.25*(1-c)*(1-c);
	break;

      default:
	::abort();

      }
      break;

    case 2: //m1=2

      switch (m2){

      case 2: //m2=1;
	d=0.5*(1+c)*(2*c-1);
	break;

      case 0: //m2=0;
	d=-sqrt(3.0/2.0)*s*c;
	break;
	
      case -2: //m2=-1;
	d=0.5*(1-c)*(2*c+1);
	break;

      default:
	::abort();
	
      }
      break;

    case 0: //m1=2
      d=0.5*(3*c*c-1);
      break;

    default:
      ::abort();

    }
    break;
    

  default:

    //Ok here we use the new method as we have 
    //no old method.
    d=dtmp*sign;
    
  }

  if (sign==-1) d=-d;

  //cout << "old:"<<d<<endl<<endl;

  assert(fabs(dtmp-d)<0.0001);

  return d;
  
}



