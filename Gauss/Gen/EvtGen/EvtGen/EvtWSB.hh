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
// Module: EvtGen/EvtWSB.hh
//
// Description:Implementation of WSB semileptonic model
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------

#ifndef EVTWSB_HH
#define EVTWSB_HH

#include <fstream.h>
#include <stdio.h>
#include "EvtGen/EvtGenKine.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtDiracSpinor.hh"
#include "EvtGen/EvtTensor4C.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtDecayAmp.hh"
#include "EvtGen/EvtSemiLeptonicScalarAmp.hh"
#include "EvtGen/EvtSemiLeptonicVectorAmp.hh"
#include "EvtGen/EvtWSBFF.hh"

class EvtWSB:public  EvtDecayAmp  {

public:

  EvtWSB() {}
  virtual ~EvtWSB();

  void getName(EvtString& name);
  EvtDecayBase* clone();

  void decay(EvtParticle *p);
  void init();

private:
  EvtSemiLeptonicFF *wsbffmodel;
  EvtSemiLeptonicAmp *calcamp;
};

#endif

