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
// Module: EvtGen/EvtSTS.hh
//
// Description:Performs the decay of a scalar -> tensor scalar
//
// Modification history:
//
//    RYD        August 21, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTSTS_HH
#define EVTSTS_HH

#include "EvtGen/EvtDecayAmp.hh"

class EvtParticle;

class EvtSTS:public  EvtDecayAmp  {

public:

  EvtSTS() {}
  virtual ~EvtSTS();

  void getName(EvtString& name);
  EvtDecayBase* clone();

  void init();
  void initProbMax();

  void decay(EvtParticle *p); 

};

#endif
