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
// Module: EvtGen/EvtSemiLeptonicFF.hh
// This is the base class for semileptonic form factor calculations.
//
// Description:
//
// Modification history:
//
//    DJL/RYD     September 30, 1997         Module created
//
//------------------------------------------------------------------------

#ifndef EVTSEMILEPTONICFF_HH
#define EVTSEMILEPTONICFF_HH

#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtId.hh"

class EvtSemiLeptonicFF{

 public:

  virtual void getscalarff( EvtId parent, EvtId daught,
                       double t, double mass, double *fpf,
		       double *fmf ) {return;}
  virtual void getvectorff( EvtId parent, EvtId daught,
                       double t, double mass, double *a1f,
                       double *a2f, double *vf, double *a0f ) {return;}
  virtual void gettensorff( EvtId parent, EvtId daught,
                       double t, double mass, double *a1f,
                       double *a2f, double *vf, double *a0f ) {return;}

};

#endif
