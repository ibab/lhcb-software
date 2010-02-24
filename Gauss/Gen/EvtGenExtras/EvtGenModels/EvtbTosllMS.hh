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
// Module: EvtGen/EvtbTosllMS.hh
//
// Description: Implementation of the B -> K* ell^+ ell^- decay according
//              to the paper: D.Melikhov, B.Stech, PRD62, 014006 (2000).
//
//              This is the derived class of the base class "EvtDecayAmp", 
//              but the decay amplitude will be calculated in the class 
//              "EvtbTosllAmpNew" (see file EvtbTosllAmpNew.hh).
//
// Modification history:
//
//  N.Nikitin (nnikit@mail.cern.ch)  Avril 21, 2008    Module created
//
//------------------------------------------------------------------------

#ifndef EVTBTOSLLMS_HH
#define EVTBTOSLLMS_HH

#include "EvtGenBase/EvtDecayAmp.hh"

class EvtParticle;
class EvtbTosllFFNew;        // my class with ff for rare semileptonic B-decays
class EvtbTosllAmpNew;       // my class with amplitudes for rare semileptonic B-decays
class EvtbTosllWilsCoeffNLO; // my class with Wilson coefficients in NLO

class EvtbTosllMS:public  EvtDecayAmp{

public:

  EvtbTosllMS() {} ;
  virtual ~EvtbTosllMS();

  virtual std::string getName() ;
  virtual EvtDecayBase* clone();

  virtual void init();
  virtual void initProbMax();
  virtual void decay(EvtParticle *p);

private:
  EvtbTosllFFNew *_msffmodel;
  EvtbTosllAmpNew *_calcamp;
  EvtbTosllWilsCoeffNLO *_wilscoeff;
};

#endif
