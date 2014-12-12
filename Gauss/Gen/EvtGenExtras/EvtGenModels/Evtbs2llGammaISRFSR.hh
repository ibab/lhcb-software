//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 2003      Caltech
//
// Module: Evtbs2llGammaISRFSR.hh
//
// Description: See the Internal LHCb Note LHCb-INT-2011-011.
//
// Modification history:
//
//   N.Nikitin    December   09, 2014     Module created
//
//------------------------------------------------------------------------

#ifndef EVTBS2LLGAMMAISRFSR_HH
#define EVTBS2LLGAMMAISRFSR_HH

#include "EvtGenBase/EvtDecayAmp.hh"

class EvtParticle;
class Evtbs2llGammaFF;          // my class with ff for rare semileptonic B-decays
class Evtbs2llGammaISRFSRAmp;   // my class with amplitudes for rare radiative leptonic B-decays
class EvtbTosllWilsCoeffNLO;    // my class with Wilson coefficients in NLO

class Evtbs2llGammaISRFSR:public  EvtDecayAmp{

public:

  Evtbs2llGammaISRFSR() {}
  virtual ~Evtbs2llGammaISRFSR();

  virtual std::string getName();
  virtual EvtDecayBase* clone();

  virtual void init();
  virtual void initProbMax();
  virtual void decay(EvtParticle *p);

private:
  Evtbs2llGammaFF *_mntffmodel;
  Evtbs2llGammaISRFSRAmp *_calcamp;
  EvtbTosllWilsCoeffNLO *_wilscoeff;
};

#endif

