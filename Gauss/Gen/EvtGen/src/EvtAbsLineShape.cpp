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
// Module: EvtLineShape.cc
//
// Description: Store particle properties for one particle.
//
// Modification history:
//
//    Lange     March 10, 2001        Module created
//
//------------------------------------------------------------------------
//
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "EvtGen/EvtAbsLineShape.hh"
#include "EvtGen/EvtRandom.hh"


EvtAbsLineShape::EvtAbsLineShape() {
}

EvtAbsLineShape::~EvtAbsLineShape() {
}

EvtAbsLineShape::EvtAbsLineShape(double mass, double width, double maxRange) {

  _mass=mass;
  _width=width;

  double maxdelta=5.0*width;
  if ( width>0.001 ) {
    if ( 5.0*width < 0.6 ) maxdelta = 0.6;
  }
  if ( maxRange > 0.00001 ) {
    _massMax=mass+maxdelta;
    _massMin=mass-maxRange;
  }
  else{
    _massMax=mass+maxdelta;
    _massMin=mass-5.0*width;
  }
 
}

EvtAbsLineShape::EvtAbsLineShape(const EvtAbsLineShape& x){
  _mass=x._mass;
  _massMax=x._massMax;
  _massMin=x._massMin;
  _width=x._width;
}

EvtAbsLineShape& EvtAbsLineShape::operator=(const EvtAbsLineShape& x){
  _mass=x._mass;
  _massMax=x._massMax;
  _massMin=x._massMin;
  _width=x._width;

  return *this; 
}

EvtAbsLineShape* EvtAbsLineShape::clone() {

  return new EvtAbsLineShape(*this);
}


double EvtAbsLineShape::rollMass() {

  double ymin, ymax;
  double temp;

  if ( _width < 0.0001 ) {
    return _mass;
  }
  else{
    ymin = atan( 2.0*(_massMin-_mass)/_width);
    ymax = atan( 2.0*(_massMax-_mass)/_width);

    temp= ( _mass + ((_width/2.0)*tan(EvtRandom::Flat(ymin,ymax))));

    return temp;
  }
}

