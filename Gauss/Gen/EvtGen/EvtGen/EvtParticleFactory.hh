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
// Module: EvtGen/EvtParticleFactory.hh
//
// Description:Class to describe all particles
//
// Modification history:
//
//    DJL December 27, 1999 Module created.
//
//------------------------------------------------------------------------

#ifndef EVTPARTICLEFACTORY_HH
#define EVTPARTICLEFACTORY_HH

#include <iostream.h>
#include "EvtGen/EvtVector4R.hh"
#include "EvtGen/EvtId.hh"
#include "EvtGen/EvtSpinType.hh"


class EvtParticleFactory{

public:


static EvtParticle* particleFactory(EvtSpinType::spintype spinType);

static EvtParticle* particleFactory(EvtId id, 
				    EvtVector4R p4);

static EvtParticle* particleFactory(EvtId id, 
				    EvtVector4R p4,
				    EvtSpinDensity rho);

};


#endif
