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
// Module: EvtGen/EvtISGW.hh
//
// Description:Implementation of the ISGW model
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------

#ifndef EVTISGW_HH
#define EVTISGW_HH

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
#include "EvtGen/EvtSemiLeptonicTensorAmp.hh"
#include "EvtGen/EvtISGWFF.hh"


class EvtISGW:public  EvtDecayAmp  {

public:

  EvtISGW() {}
  virtual ~EvtISGW();

  void getName(EvtString& name);
  EvtDecayBase* clone();

  void decay(EvtParticle *p);
  void init();

private:
  EvtSemiLeptonicFF *isgwffmodel;
  EvtSemiLeptonicAmp *calcamp;
};



#endif

