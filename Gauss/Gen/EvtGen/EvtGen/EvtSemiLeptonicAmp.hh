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
// Module: EvtGen/EvtSemiLeptonicAmp.hh
//
// Description:Store decay parameters for one decay.
//
// Modification history:
//
//    RYD     September 30 1997         Module created
//
//------------------------------------------------------------------------

#ifndef EVTSEMILEPTONICAMP_HH
#define EVTSEMILEPTONICAMP_HH

#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtSemiLeptonicFF.hh"

class EvtAmp;

class EvtSemiLeptonicAmp{

 public:

  //Daughters are initialized and have been added to the parent.
  //No need to carry around the daughters seperately!

  virtual void CalcAmp( EvtParticle *parent, EvtAmp& amp,
            EvtSemiLeptonicFF *FormFactors  ) {return;}

  double CalcMaxProb( EvtId parent, EvtId meson, EvtId lepton,
		      EvtId nudaug, EvtSemiLeptonicFF *FormFactors );


};

#endif
