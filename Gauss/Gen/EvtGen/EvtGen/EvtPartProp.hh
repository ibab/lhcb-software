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
// Module: EvtGen/EvtPartProp.hh
//
// Description: Class to keep the particle properties for
//              one particle
//
// Modification history:
//
//    RYD     April 4, 1997         Module created
//
//------------------------------------------------------------------------

#ifndef EVTPARTPROP_HH
#define EVTPARTPROP_HH

#include "EvtGen/EvtSpinType.hh"
#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtString.hh"
#include "EvtGen/EvtAbsLineShape.hh"

class EvtPartProp {

public:

  EvtPartProp(); 
  EvtPartProp(const EvtPartProp& x); 

  ~EvtPartProp(); 

  double getMass() {return _lineShape->getMass();} 
  double getMassMin() {return _lineShape->getMassMin();} 
  double getMassMax() {return _lineShape->getMassMax();} 
  double getWidth() {return _lineShape->getWidth();} 

  double getctau() {return _ctau; } 
  void   setctau(double tau) { _ctau=tau; }

  int    getChg3() {return _chg3; } 
  void   setChg3(int c3) { _chg3=c3; }

  EvtSpinType::spintype  getSpinType() {return _spintype; }
  void   setSpinType(EvtSpinType::spintype stype ) { _spintype=stype; }

  const EvtString&  getName() {return _name;}
  void   setName(EvtString pname);

  EvtId  getId() {return _id;}
  void   setId(EvtId id) {_id=id;}

  EvtId  getIdChgConj() {return _idchgconj;}
  void   setIdChgConj(EvtId idchgconj) {_idchgconj=idchgconj;}

  int  getStdHep() {return _stdhep;}
  void   setStdHep(int stdhep) {_stdhep=stdhep;}

  int  getLundKC() {return _lundkc;}
  void   setLundKC(int lundkc) {_lundkc=lundkc;}

  EvtAbsLineShape* getLineShape() {return _lineShape;}
  void initLineShape(double mass, double width, double maxRange);
  void initLineShape(double mass, double width, double maxRange, double mDaug1, double mDaug2, int l);

  double rollMass(){return _lineShape->rollMass();}

  EvtPartProp& operator=(const EvtPartProp& x);

  void reSetMass(double mass);
  void reSetWidth(double width);


private:

  EvtAbsLineShape *_lineShape;

  double _ctau;
  EvtId  _id;
  EvtId  _idchgconj;
  EvtSpinType::spintype  _spintype;
  int _chg3;
  int _stdhep;
  int _lundkc;
  EvtString _name;
  
}; 

#endif

