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

#ifndef EVTRELBREITWIGNERBARRIERFACT_HH
#define EVTRELBREITWIGNERBARRIERFACT_HH

#include "EvtGen/EvtAbsLineShape.hh"

class EvtRelBreitWignerBarrierFact :public EvtAbsLineShape {

public:

  EvtRelBreitWignerBarrierFact(); 
  EvtRelBreitWignerBarrierFact(double mass, double width, double maxRange,
			       double mDaug1, double mDaug2, int l); 
  ~EvtRelBreitWignerBarrierFact();
  EvtRelBreitWignerBarrierFact& operator=(const EvtRelBreitWignerBarrierFact& x);
  EvtRelBreitWignerBarrierFact(const EvtRelBreitWignerBarrierFact& x); 

  double rollMass();
  double getBlattWeisskof(double p, double pAB);
  EvtAbsLineShape* clone();

protected:

  double _m1;
  double _m2;
  int _l;
}; 

#endif

