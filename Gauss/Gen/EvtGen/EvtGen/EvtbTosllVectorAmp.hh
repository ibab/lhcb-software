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
// Module: EvtGen/EvtbTosllVectorAmp.hh
//
// Description:
//
// Modification history:
//
//    RYD     January 5, 2000         Module created
//
//------------------------------------------------------------------------

#ifndef EVTBTOSLLVECTORAMP_HH
#define EVTBTOSLLVECTORAMP_HH


#include <assert.h>
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtbTosllAmp.hh"
#include "EvtGen/EvtbTosllFF.hh"

class EvtAmp;

class EvtbTosllVectorAmp : public EvtbTosllAmp {

  // public:

  //Daughters are initialized and have been added to the parent.
  //No need to carry around the daughters seperately!
  void CalcAmp( EvtParticle *parent, EvtAmp& amp,
		EvtbTosllFF *formFactors );

};

#endif

