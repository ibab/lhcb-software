//--------------------------------------------------------------------------
//
//
// Copyright Information: See EvtGen/COPYRIGHT
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
//
// Module: EvtGen/EvtBtoXsll.hh
//
// Description:
// Class to generate inclusive non-resonant B -> Xs l+ l- decays.
//
// Modification history:
//
//    Stephane Willocq       Jan  17, 2001       Module created
//
//------------------------------------------------------------------------

#ifndef EVTBTOXSLL_HH
#define EVTBTOXSLL_HH

#include "EvtGenBase/EvtDecayIncoherent.hh"
#include "EvtGenBase/EvtParticle.hh"

class EvtBtoXsllUtil;

class EvtBtoXsll:public  EvtDecayIncoherent  {

public:
  
  EvtBtoXsll() {}
  virtual ~EvtBtoXsll();

  void getName(std::string& name);

  EvtDecayBase* clone();

  void initProbMax();

  void init();

  void decay(EvtParticle *p); 

private:

  EvtBtoXsllUtil *_calcprob;
  double         _dGdsProbMax;
  double         _dGdsdupProbMax;
};

#endif

