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
// Module: EvtGen/EvtDecayIncoherent.hh
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EvtDecayIncoherent_HH
#define EvtDecayIncoherent_HH

#include "EvtGen/EvtDecayBase.hh"
#include "EvtGen/EvtParticle.hh"


class EvtParticle;

class EvtDecayIncoherent : public EvtDecayBase{

public:

  void makeDecay(EvtParticle* p);

  virtual ~EvtDecayIncoherent() {}

  void setDaughterSpinDensity(int daughter)
  { spinDensitySet[daughter]=1; return;}

  int isDaughterSpinDensitySet(int daughter) 
  {return spinDensitySet[daughter];}

private:

  int spinDensitySet[MAX_DAUG];

};




#endif



