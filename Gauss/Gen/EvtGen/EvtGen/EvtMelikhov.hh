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
// Module: EvtGen/EvtMelikhov.hh
//
// Description:Implementation of the Melikhov semileptonic model
//
// Modification history:
//
//    DJL     April 20, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTMELIKHOV_HH
#define EVTMELIKHOV_HH

#include <stdio.h>
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtDiracSpinor.hh"
#include "EvtGen/EvtTensor4C.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtDecayAmp.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtSemiLeptonicVectorAmp.hh"
#include "EvtGen/EvtMelikhovFF.hh"

class EvtMelikhov:public  EvtDecayAmp  {

public:

  EvtMelikhov() {}
  virtual ~EvtMelikhov();

  void getName(EvtString& name);
  EvtDecayBase* clone();

  void decay(EvtParticle *p);
  void init();

private:
  EvtSemiLeptonicFF *Melikhovffmodel;
  EvtSemiLeptonicAmp *calcamp;
};

#endif

