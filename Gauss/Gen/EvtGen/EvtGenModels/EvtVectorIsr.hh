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
// Module: EvtGen/EvtVectorIsr.hh
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTVECTORISR_HH
#define EVTVECTORISR_HH

#include "EvtGenBase/EvtDecayIncoherent.hh"

class EvtParticle;

//This is a special decay model to generate e+e- -> phi gamma

class EvtVectorIsr:public  EvtDecayIncoherent  {

public:

  EvtVectorIsr() {}
  virtual ~EvtVectorIsr();


  void getName(std::string& name);

  EvtDecayBase* clone();

  void decay(EvtParticle *p); 

  void init();

  void initProbMax();

private:

  double csfrmn,csbkmn;
  

};

#endif











