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
// Module: EvtGen/EvtSVPHelAmp.hh
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EVTSVPHELAMP_HH
#define EVTSVPHELAMP_HH

#include "EvtGen/EvtDecayAmp.hh"
#include "EvtGen/EvtParticle.hh"

//Class to handle decays of the form SCALAR ->VECTOR PHOTON
//where the helicity amplitudes must be specified.  The
//first and third arguements are the magnetudes of the H+
//and H- helicity amplitudes respectively.  The second and
//fourth arguements are the phases.
//Calls EvtSVPHel.

class EvtSVPHelAmp:public  EvtDecayAmp  {

public:

  EvtSVPHelAmp() {}
  virtual ~EvtSVPHelAmp();

  void getName(EvtString& name);
  EvtDecayBase* clone();

  void init();
  void initProbMax();
  void decay(EvtParticle *p); 

};

#endif
