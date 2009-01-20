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
// Module: EvtGen/EvtHepRandomEngine.cc
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
//
#include "CLHEP/Random/RandomEngine.h"
#include "EvtGenBase/EvtRandomEngine.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenModels/EvtHepRandomEngine.hh"

EvtHepRandomEngine::EvtHepRandomEngine() {}

EvtHepRandomEngine::~EvtHepRandomEngine() {}

double EvtHepRandomEngine::random()
{
  return EvtRandom::Flat();
}

HepDouble EvtHepRandomEngine::flat()
{
  return random();
}

void EvtHepRandomEngine::showStatus() const {}
void EvtHepRandomEngine::restoreStatus(const char /*filename*/[]) {}
void EvtHepRandomEngine::saveStatus(const char /*filename*/[]) const {}
void EvtHepRandomEngine::setSeeds(const long* /*a*/, int /*b*/) {}
void EvtHepRandomEngine::setSeed(long /*a*/, int /*b*/) {}
void EvtHepRandomEngine::flatArray(const HepInt s, HepDouble* vect) 
{
  for (int i=0;i<s;i++)
    vect[i] = flat();
}

std::string EvtHepRandomEngine::name( ) const {
  return std::string( "EvtHepRandomEngine" ) ;
}
