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
// Module: EvtGen/EvtKS.hh
//
// Description:Implementation of the KS semileptonic model
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTKS_HH
#define EVTKS_HH

#include <fstream.h>
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
#include "EvtGen/EvtKSFF.hh"

class EvtKS:public  EvtDecayAmp  {

public:

  EvtKS() {}
  virtual ~EvtKS();

  void getName(EvtString& name);
  EvtDecayBase* clone();

  void decay(EvtParticle *p);
  void init();

private:
  EvtSemiLeptonicFF *ksffmodel;
  EvtSemiLeptonicAmp *calcamp;
};

#endif

