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
// Module: EvtGen/EvtSLPole.hh
//
// Description:Semileptonic decays with pole form form factors
//
// Modification history:
//
//    DJL     April 23, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTSLPOLE_HH
#define EVTSLPOLE_HH

#include <stdio.h>
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtDiracSpinor.hh"
#include "EvtGen/EvtTensor4C.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtDecayAmp.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtSemiLeptonicScalarAmp.hh"
#include "EvtGen/EvtSemiLeptonicVectorAmp.hh"
#include "EvtGen/EvtSemiLeptonicTensorAmp.hh"
#include "EvtGen/EvtSLPoleFF.hh"

class EvtSLPole:public  EvtDecayAmp  {

public:

  EvtSLPole() {}
  virtual ~EvtSLPole();

  void getName(EvtString& name);
  EvtDecayBase* clone();

  void decay(EvtParticle *p);
  void initProbMax();
  void init();

private:
  EvtSemiLeptonicFF *SLPoleffmodel;
  EvtSemiLeptonicAmp *calcamp;
};

#endif

