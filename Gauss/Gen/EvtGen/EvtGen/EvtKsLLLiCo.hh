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
// Module: EvtGen/EvtKsLLLiCo.hh
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTKSLLLC_HH
#define EVTKSLLLC_HH

#include "EvtGen/EvtDecayAmp.hh"
#include "EvtGen/EvtParticle.hh"

class EvtKsLLLC:public  EvtDecayAmp  {

public:

  EvtKsLLLC() {}
  virtual ~EvtKsLLLC();

  void getName(EvtString& name);
  EvtDecayBase* clone();

  void init();
  void decay(EvtParticle *p);
  void initProbMax();

  // declaration of oneLoop(x,s)
  static EvtComplex oneLoopFourQ2(double, double);
  //declaration of omega(s)
  static double omega2(double);
};

#endif
