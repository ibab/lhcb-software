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
// Module: EvtGen/EvtTSS.hh
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTTSS_HH
#define EVTTSS_HH

#include "EvtGen/EvtDecayAmp.hh"
#include "EvtGen/EvtParticle.hh"

//Class to handle decays of the form TENSOR -> SCALAR SCALAR.
//Calls EvtTensorToScalarScalar

class EvtTSS:public  EvtDecayAmp  {

public:

  EvtTSS() {}
  virtual ~EvtTSS();

  void getName(EvtString& name);
  EvtDecayBase* clone();

  void init();
  void decay(EvtParticle *p); 
  void initProbMax();

};

#endif
