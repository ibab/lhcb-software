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
// Module: Evtbs2llGammaMNT.hh
//
// Description: the B -> Gamma ell^+ ell^- decay channel description
//		based on the papers:
//		D.Melikhov, N.Nikitin, K.Toms, Yad. Fiz. 62, No 11
//              and 
//              I.Balakireva, D.Melikhov, N.Nikitin, D.Tlisov, 
//                                           e-Print: arXiv:0911.0605 [hep-ph]. 
//
// Modification history:
//
//  A.Popov                October  30, 2008    Module created
//  A.Popov and N.Nikitin  February 25, 2010    New description of CP-violation
//
//------------------------------------------------------------------------

#ifndef EVTBS2LLGAMMAMNT_HH
#define EVTBS2LLGAMMAMNT_HH

#include "EvtGenBase/EvtDecayAmp.hh"

class EvtParticle;
class Evtbs2llGammaFF;          // my class with ff for rare semileptonic B-decays
class Evtbs2llGammaAmp;         // my class with amplitudes for rare semileptonic B-decays
class EvtbTosllWilsCoeffNLO;    // my class with Wilson coefficients in NLO

class Evtbs2llGammaMNT:public  EvtDecayAmp{

public:

  Evtbs2llGammaMNT() {}
  virtual ~Evtbs2llGammaMNT();

  virtual std::string getName();
  virtual EvtDecayBase* clone();

  virtual void init();
  virtual void initProbMax();
  virtual void decay(EvtParticle *p);

private:
  Evtbs2llGammaFF *_mntffmodel;
  Evtbs2llGammaAmp *_calcamp;
  EvtbTosllWilsCoeffNLO *_wilscoeff;
};

#endif

