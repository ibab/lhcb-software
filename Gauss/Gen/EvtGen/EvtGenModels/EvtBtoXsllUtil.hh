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
// Module: EvtGen/EvtBtoXsllUtil.hh
//
// Description:
// Class to generate inclusive non-resonant B -> Xs l+ l- decays.
//
// Modification history:
//
//    Stephane Willocq       Jan  19, 2001       Module created
//
//------------------------------------------------------------------------

#ifndef EVTBTOXSLLUTIL_HH
#define EVTBTOXSLLUTIL_HH

#include "EvtGenBase/EvtComplex.hh"
#include "EvtGenBase/EvtDecayIncoherent.hh"
#include "EvtGenBase/EvtParticle.hh"

class EvtBtoXsllUtil{

public:

  EvtComplex coeff9(double mb, double sh);

  double dGdsProb(double mb, double ms, double ml,
                                  double s);

  double dGdsdupProb(double mb, double ms, double ml,
                                     double s,  double u);
  
  double FermiMomentum( double pf );
  
  double FermiMomentumProb( double pb, double pf );

};

#endif

