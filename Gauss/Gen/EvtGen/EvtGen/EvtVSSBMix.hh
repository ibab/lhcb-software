//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1999    Stanford University
//
// Module: EvtGen/EvtVSSBMix.hh
//
// Description:
//    Routine to decay vector-> scalar scalar with coherent BB-like mixing
//
// Modification history:
//
//    D. Kirkby   October 1, 1999    Module created (adapted from VSSMix)
//
//------------------------------------------------------------------------

#ifndef EVTVSSBMIX_HH
#define EVTVSSBMIX_HH

#include "EvtGen/EvtDecayAmp.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtComplex.hh"

class EvtVSSBMix : public EvtDecayAmp  {
public:
  EvtVSSBMix() {}
  virtual ~EvtVSSBMix();

  void getName(EvtString& name);
  EvtDecayBase* clone();

  void decay(EvtParticle *p); 
  void init();
  void initProbMax();

private:
  double _freq;   // mixing frequency in hbar/mm
  double _dGamma;
  EvtComplex _qoverp;
};

#endif
