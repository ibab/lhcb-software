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
// Module: EvtGen/EvtAbsLineShape.hh
//
// Description: Class to keep the particle properties for
//              one particle
//
// Modification history:
//
//    Lange     March 10, 2001         Module created
//
//------------------------------------------------------------------------

#ifndef EVTABSLINESHAPE_HH
#define EVTABSLINESHAPE_HH

#include "EvtGenBase/EvtSpinType.hh"

class EvtId;

class EvtAbsLineShape {

public:

  EvtAbsLineShape(); 
  EvtAbsLineShape(double mass, double width, double maxRange, EvtSpinType::spintype sp); 
  virtual ~EvtAbsLineShape();
  EvtAbsLineShape& operator=(const EvtAbsLineShape& x);
  EvtAbsLineShape(const EvtAbsLineShape& x); 

  double getMass() {return _mass;}
  double getMassMin() {return _massMin;} 
  double getMassMax() {return _massMax;}
  double getMaxRange() {return _maxRange;}
  double getWidth() {return _width;} 
  EvtSpinType::spintype getSpinType() {return _spin;}
  virtual double rollMass();
  virtual EvtAbsLineShape* clone();

  void reSetMass(double mass) { _mass=mass;}
  void reSetWidth(double width) { _width=width;}
  void reSetMassMin(double mass) { _massMin=mass;}
  void reSetMassMax(double mass) { _massMax=mass;}
  virtual void reSetBlatt(double /*  blatt  */ ) {};
  void includeBirthFactor(bool yesno) { _includeBirthFact = yesno; }
  void includeDecayFactor(bool yesno) { _includeDecayFact = yesno; }

  virtual double getRandMass(EvtId *parId, int nDaug, EvtId *dauId, EvtId *othDaugId,double maxMass, double *dauMasses);
  virtual double getMassProb(double mass, double massPar, int nDaug, double *massDau);

protected:

  bool _includeDecayFact;
  bool _includeBirthFact;
  double _mass;
  double _massMin;
  double _massMax;
  double _width;
  double _maxRange;
  EvtSpinType::spintype _spin;
  
}; 

#endif

