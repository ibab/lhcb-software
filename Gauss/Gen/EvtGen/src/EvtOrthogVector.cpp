//--------------------------------------------------------------------------
// 
// Environment: 
// This software is part of the EvtGen package developed jointly 
// for the BaBar and CLEO collaborations.  If you use all or part 
// of it, please give an appropriate acknowledgement.
// 
// Copyright Information: See EvtGen/COPYRIGHT 
// Copyright (C) 2000 Caltech, LLNL
// 
// Module: EvtGen/EvtOrthogVector.hh
// 
// Description:
// 
// Modification history: 
//
// Lange August 11, 2000 Created
//
//------------------------------------------------------------------------

#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "EvtGen/EvtOrthogVector.hh"

EvtOrthogVector::EvtOrthogVector(int n, EvtVectorT<double> *vectors){

  _dimen=n;
  _holder.resize(n);
  _orthogVector.resize(n);
  _orthogVector.init(0.0);


  EvtVectorT<int> temp(n);
  
  int i;
  for (i=0;i<n;i++) {temp[i]=i;}

  findOrthog(_dimen,temp, vectors);

}

EvtOrthogVector::~EvtOrthogVector(){
}

void EvtOrthogVector::findOrthog(int dim, EvtVectorT<int> invect, 
			    EvtVectorT<double> *vectors) {


  if ( dim==2 ) {
    _holder[0]=invect[0];
    _holder[1]=invect[1];
    int sign=findEvenOddSwaps();
    {
      double addition=1;
      int i;
      for (i=1; i<_dimen; i++){
	addition*=vectors[i-1][_holder[i]];
      }
      addition*=sign;
      _orthogVector[_holder[0]]+=addition;
    }
    
    _holder[0]=invect[1];
    _holder[1]=invect[0];
    
    {
      double addition=1;
      int i;
      for (i=1; i<_dimen; i++){
	addition*=vectors[i-1][_holder[i]];
      }
      addition*=sign;
      _orthogVector[_holder[0]]-=addition;
    }
    
    return;
  }
  else{
    EvtVectorT<int> temp((2*dim));

    int i;
    for (i=0; i<dim; i++) temp[i]=invect[i];
    for (i=0; i<dim; i++) temp[i+dim]=invect[i];

    for (i=0; i<dim; i++) {
      _holder[dim-1]=temp[dim-1+i];
      EvtVectorT<int> tempDim((dim-1));

      int j;
      for (j=0; j<(dim-1); j++) tempDim[j]=temp[j+i];
      findOrthog(dim-1, tempDim, vectors); 
    }
  }
 
  return;
}

int EvtOrthogVector::findEvenOddSwaps() {

  EvtVectorT<int> temp(_dimen);

  int i,j,nSwap;
  for (i=0; i<_dimen; i++) temp[i]=_holder[i];

  nSwap=0;
  for (i=0; i<(_dimen-1); i++) {
    for (j=i+1; j<_dimen; j++) {

      if ( temp[i]>temp[j] ) {
	int duh=temp[j];
	temp[j]=temp[i];
	temp[i]=duh;
	nSwap+=1;
      }
    }
  }
  nSwap-= (nSwap/2)*2;

  if ( nSwap ) return -1;
  
  return 1;

}
