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
// Module: EvtGen/EvtSSSCPT.hh
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTSSSCPT_HH
#define EVTSSSCPT_HH

#include "EvtGen/EvtDecayAmp.hh"
#include "EvtGen/EvtParticle.hh"

class EvtSSSCPT:public  EvtDecayAmp  {

public:

  EvtSSSCPT() {}
  virtual ~EvtSSSCPT();

  void getName(EvtString& name);
  EvtDecayBase* clone();

  void init();
  void decay(EvtParticle *p); 

};

#endif
