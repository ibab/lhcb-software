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


class EvtAbsLineShape {

public:

  EvtAbsLineShape(); 
  EvtAbsLineShape(double mass, double width=0., double maxRange=0.); 
  ~EvtAbsLineShape();
  EvtAbsLineShape& operator=(const EvtAbsLineShape& x);
  EvtAbsLineShape(const EvtAbsLineShape& x); 

  double getMass() {return _mass;}
  double getMassMin() {return _massMin;} 
  double getMassMax() {return _massMax;}
  double getWidth() {return _width;} 

  virtual double rollMass();
  virtual EvtAbsLineShape* clone();

  void reSetMass(double mass) { _mass=mass;}
  void reSetWidth(double width) { _width=width;}

protected:

  double _mass;
  double _massMin;
  double _massMax;
  double _width;
  
}; 

#endif

