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
// Module: EvtPartProp.cc
//
// Description: Store particle properties for one particle.
//
// Modification history:
//
//    RYD     April 4, 1997        Module created
//
//------------------------------------------------------------------------
//
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <ctype.h>
#include "EvtGen/EvtPartProp.hh"
#include "EvtGen/EvtAbsLineShape.hh"
#include "EvtGen/EvtRelBreitWignerBarrierFact.hh"


EvtPartProp::EvtPartProp():
  _id(-1,-1)
  ,_idchgconj(-1,-1)
  ,_chg3(0)
  ,_stdhep(0)
  ,_lundkc(0)
{
  _lineShape=0;
  _ctau=0.0;
  _name="*******";
  _spintype=EvtSpinType::SCALAR;
}

EvtPartProp::EvtPartProp(const EvtPartProp& x){

  _lineShape=x._lineShape->clone();
  _ctau=x._ctau;
  _name=x._name;
  _spintype=x._spintype;
  _id=x._id;
  _idchgconj=x._idchgconj;
  _chg3=x._chg3;
  _stdhep=x._stdhep;
  _lundkc=x._lundkc;

}

EvtPartProp::~EvtPartProp() {
  if ( _lineShape ) delete _lineShape;
  _lineShape=0;
}


void EvtPartProp::setName(EvtString pname) {

  _name=pname;

}


EvtPartProp& EvtPartProp::operator=(const EvtPartProp& x){

  _lineShape=x._lineShape->clone();

  _ctau=x._ctau;
  _name=x._name;
  _chg3=x._chg3;
  _spintype=x._spintype;
  return *this;
}

void EvtPartProp::initLineShape(double mass, double width, double maxRange){

  _lineShape=new EvtAbsLineShape(mass,width,maxRange);

}

void EvtPartProp::initLineShape(double mass, double width, double maxRange,
				double mDaug1, double mDaug2, int l){
  _lineShape=new EvtRelBreitWignerBarrierFact(mass,width,maxRange,mDaug1,mDaug2,l);

}

void EvtPartProp::reSetMass(double mass) {
  if (!_lineShape) ::abort();
  _lineShape->reSetMass(mass);
}
void EvtPartProp::reSetWidth(double width){
  if (!_lineShape) ::abort();
  _lineShape->reSetWidth(width);
}








