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
// Module: EvtGen/EvtDecayParm.hh
//
// Description: Class to keep the arguments and daughters of a decay
//
// Modification history:
//
//    RYD     April 5, 1997         Module created
//
//------------------------------------------------------------------------

#ifndef EVTDECAYPARM_HH
#define EVTDECAYPARM_HH

#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtString.hh"

typedef void ( *fcnPtr ) ( EvtParticle *,int,int *,double* );

class EvtDecayParm {

public:
  
  EvtDecayParm(); 
  ~EvtDecayParm(); 

  void    init(fcnPtr pfcn, int ndaug, int *daugs, int narg,
	       double *args, EvtString name); 

  int     GetNDaug() {return itsndaug; }
  int     GetNArg() {return itsnarg; }
  int*    GetDaugs() {return itsdaugs; }
  double* GetArgs() {return itsargs; }
  fcnPtr  GetfcnPtr() {return itsfcn; }
  EvtString   GetModelName() {return modelname; }

private:


  fcnPtr  itsfcn;
  int     itsndaug;
  int*    itsdaugs;
  int     itsnarg;
  double* itsargs;
  EvtString    modelname;

  
}; 

#endif

