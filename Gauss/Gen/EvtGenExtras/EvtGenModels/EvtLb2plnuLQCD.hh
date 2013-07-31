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
// Module: EvtGen/EvtLb2plnuLQCD.hh
//
// Description:Implementation of the Lb2plnuLQCD model
// Class to handle semileptonic Lb -> p l nu decays using the using form factor predictions from Lattice QCD.
// 
//
// Modification history:
//
//    William Sutcliffe     July 27, 2013     Module created
//
//------------------------------------------------------------------------

#ifndef EVTLB2PMUNULQCD_HH
#define EVTLB2PMUNULQCD_HH

#include "EvtGenBase/EvtDecayAmp.hh"
#include "EvtGenBase/EvtSemiLeptonicFF.hh"
#include "EvtGenBase/EvtSLBaryonAmp.hh"

class EvtParticle;

class EvtLb2plnuLQCD:public  EvtDecayAmp  {

public:

  EvtLb2plnuLQCD();
  virtual ~EvtLb2plnuLQCD();

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

