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
// Module: EvtGen/include/EvtVectorT.hh 
// 
// Description:Class to keep track of particle properties.  
// 
// Modification history: 
//
// Lange Aug 11, 2000 Created
//
//------------------------------------------------------------------------

#ifndef EVTCGVECTOR_HH
#define EVTCGVECTOR_HH

#include <assert.h>
#include <iostream>
template <class T>
class EvtVectorT {

public:

  EvtVectorT();
  EvtVectorT(int n);

  EvtVectorT(int n,T *vectors);

  EvtVectorT( const EvtVectorT &copy);

  ~EvtVectorT();

  void resize(int n);
  void append(T value);
  void init(T value, int position=-1);
  int length() { return _dim;}

  T& operator[](int i) {assert(_dim>0); assert(i>=0); assert(i<_dim); return _vector[i];}

private:
  int _dim;
  T *_vector;
  
};

template <class T>
EvtVectorT<T>::EvtVectorT() {
  _vector=0;
  _dim=0;
  return;

}

template <class T>
EvtVectorT<T>::EvtVectorT(int n, T *vectors) {

  assert(n>0);
  _vector=new T[n];
  _dim=n;
  int i;
  for (i=0; i<n; i++ ) {_vector[i]=vectors[i];}
}

template <class T>
EvtVectorT<T>::EvtVectorT(int n) {

  assert(n>0);
  _dim=n;
  _vector=new T[n];
}

template <class T>
EvtVectorT<T>::EvtVectorT( const EvtVectorT &copy) {

  _dim=copy._dim;
  if (_dim==0){
    _vector=0;
    return;
  }
  _vector=new T[_dim];
  int i;
  for (i=0; i<_dim; i++ ) {_vector[i]=copy._vector[i];}

}


template <class T>
EvtVectorT<T>::~EvtVectorT() {
  if ( _vector) delete [] _vector;
}


template <class T>
void EvtVectorT<T>::init(T value, int position) {

  int i;
  if ( position >=0 ) {
    assert(position<_dim);
    _vector[position]=value;
  }
  else{
    for (i=0; i<_dim; i++ ) {_vector[i]=value;}
  }
}

template <class T>
void EvtVectorT<T>::append(T value) {

  int temp=_dim+1;
  this->resize(temp);
  _vector[(_dim-1)]=value;
}


template <class T>
void EvtVectorT<T>::resize(int n) {

  assert(n>=0);
  T *temp=0;

  int i;
  int olddim=_dim;
  if ( n>_dim&&_dim>0) {
    temp=new T[_dim];
    for (i=0; i<_dim; i++ ) { temp[i]=_vector[i];}
  }

  if ( _vector!=0) delete [] _vector;

  _dim=n;

  if (_dim==0){
    _vector=0;
    return;
  }

  _vector = new T[n];

  if ( temp ) {
    for (i=0; i<olddim; i++ ) { _vector[i]=temp[i];}
    delete [] temp;
  }

  return;
}

#endif
