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
// Module: EvtGen/EvtHQETFF.hh
//
// Description:
//
// Modification history:
//
//    DJL     April 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTHQETFF_HH
#define EVTHQETFF_HH

#include "EvtGen/EvtSemiLeptonicFF.hh"
#include "EvtGen/EvtId.hh"

class EvtHQETFF : public EvtSemiLeptonicFF {

public:
  EvtHQETFF(double hqetrho2, double hqetr1, double hqetr2);
  void getvectorff(EvtId parent,EvtId daught,
                       double t, double mass, double *a1f,
                       double *a2f, double *vf, double *a0f );

private:
  double r1;
  double rho2;
  double r2;
};

#endif

