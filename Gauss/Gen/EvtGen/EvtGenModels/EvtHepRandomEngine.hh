//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 2001      SLAC
//
// Module: EvtGen/EvtHepRandomEngine.hh
//
// Description:
// this is an EvtRandomEngine and a HepRandomEngine.
// It uses EvtRandom::Flat() and can be passed to RandGeneral as a 
// random engine
//
// Modification history:
//
//   Sven Menke     February 14, 2001         Module created
//
//------------------------------------------------------------------------

#ifndef EVTHEPRANDOMENGINE_HH
#define EVTHEPRANDOMENGINE_HH

#include "CLHEP/config/CLHEP.h"
#include "CLHEP/Random/RandomEngine.h"
#include "EvtGenBase/EvtRandomEngine.hh"

class EvtHepRandomEngine:public  HepRandomEngine, public EvtRandomEngine  {

public:
  
  EvtHepRandomEngine();

  virtual ~EvtHepRandomEngine();

  virtual double random();

  virtual HepDouble flat();

  void showStatus() const;
  void restoreStatus(const char filename[]) ;
  void saveStatus(const char filename[]) const;
  void setSeeds(const long* a, int b);
  void setSeed(long a, int b);
  void flatArray(const HepInt s, HepDouble* vect);

};

#endif

