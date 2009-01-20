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

#include "EvtGenBase/EvtId.hh"

class EvtSemiLeptonicFF{

 public:

  virtual void getscalarff( EvtId , EvtId , double , double , double *,
                            double * ) {return;}
  virtual void getvectorff( EvtId , EvtId ,double , double , double *,
                            double *, double *, double * ) {return;}
  virtual void gettensorff( EvtId , EvtId ,double , double , double *,
                            double *, double *, double * ) {return;}

};

#endif
