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
// Module: EvtGen/EvtHQET.hh
//
// Description:Implementation of the HQET model
//
// Modification history:
//
//    DJL     April 20, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTHQET_HH
#define EVTHQET_HH

#include <stdio.h>
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtDiracSpinor.hh"
#include "EvtGen/EvtTensor4C.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtDecayAmp.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtSemiLeptonicVectorAmp.hh"
#include "EvtGen/EvtHQETFF.hh"

class EvtHQET:public  EvtDecayAmp  {

public:

  EvtHQET();
  virtual ~EvtHQET();

  void getName(EvtString& name);
  EvtDecayBase* clone();

  void decay(EvtParticle *p);
  void initProbMax();
  void init();

private:
  EvtSemiLeptonicFF *hqetffmodel;
  EvtSemiLeptonicAmp *calcamp;
};
#endif

