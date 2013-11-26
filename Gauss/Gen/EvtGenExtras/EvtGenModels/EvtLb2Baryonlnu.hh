//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information:
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtGen/EvtLb2Baryonlnu.hh
//
// Description:Implementation of the Lb2Baryonlnu model
// Class to handle semileptonic Lb -> p l nu decays using the using form factor predictions based on the quark model.  Here baryon can be Lc+, p, Lc*+, N*+.
// 
//
// Modification history:
//
//    William Sutcliffe     July 27, 2013     Module created
//
//------------------------------------------------------------------------

#ifndef EVTLB2BARYONLNU_HH
#define EVTLB2BARYONLNU_HH

#include "EvtGenBase/EvtDecayAmp.hh"
#include "EvtGenBase/EvtSemiLeptonicFF.hh"
#include "EvtGenBase/EvtSLBaryonAmp.hh"

class EvtParticle;

class EvtLb2Baryonlnu:public  EvtDecayAmp  {

public:

  EvtLb2Baryonlnu();
  virtual ~EvtLb2Baryonlnu();

  std::string getName();
  EvtDecayBase* clone();

  void decay(EvtParticle *p);
  void initProbMax();
  void init();

private:
  EvtSemiLeptonicFF *ffmodel;
  EvtSLBaryonAmp *calcamp;
};

#endif

