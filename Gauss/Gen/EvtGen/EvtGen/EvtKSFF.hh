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
// Module: EvtGen/EvtKSFF.hh
//
// Description: Form factors for EvtKS model
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------

#ifndef EVTKSFF_HH
#define EVTKSFF_HH

#include "EvtGen/EvtSemiLeptonicFF.hh"
#include "EvtGen/EvtId.hh"

class EvtKSFF : public EvtSemiLeptonicFF {

  void getscalarff( EvtId parent, EvtId daught,
                       double t, double mass, double *fpf,
                       double *f0f );
  void getvectorff( EvtId parent, EvtId daught,
                       double t, double mass, double *a1f,
                       double *a2f, double *vf, double *a0f );


};

#endif

