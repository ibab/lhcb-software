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
// Module: EvtGen/EvtbTosllMSExt.hh
//
// Description: Implementation of the rare semileptonic B-decays according
//              to the paper: D.Melikhov, B.Stech, PRD62, 014006 (2000)
//              with the nonstandart models simuliakr..
//
//              This is the derived class of the base class "EvtDecayAmp", 
//              but the decay amplitude will be calculated in the class 
//              "EvtbTosllAmpNew" (see file EvtbTosllAmpNew.hh).
//
// Modification history:
//
//  N.Nikitin (nnikit@mail.cern.ch)  July 07, 2011    Module created
//
//------------------------------------------------------------------------

#ifndef EVTBTOSLLMSEXT_HH
#define EVTBTOSLLMSEXT_HH

#include "EvtGenBase/EvtDecayAmp.hh"

class EvtParticle;
class EvtbTosllFFNew;        // my class with ff for rare semileptonic B-decays
class EvtbTosllAmpNewExt;    // my class with amplitudes for rare semileptonic B-decays
class EvtbTosllWilsCoeffNLO; // my class with Wilson coefficients in NLO

class EvtbTosllMSExt:public  EvtDecayAmp{

public:

  EvtbTosllMSExt() {} ;
  virtual ~EvtbTosllMSExt();

  virtual std::string getName() ;
  virtual EvtDecayBase* clone();

  virtual void init();
  virtual void initProbMax();
  virtual void decay(EvtParticle *p);

private:
  EvtbTosllFFNew        *_msffmodel;
  EvtbTosllAmpNewExt    *_calcamp;
  EvtbTosllWilsCoeffNLO *_wilscoeff;
};

#endif
