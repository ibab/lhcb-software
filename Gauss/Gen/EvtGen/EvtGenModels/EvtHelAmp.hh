//
//     This code is still under development, not ready for use!
//     ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
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
// Module: EvtGen/EvtHelAmp.hh
//
// Description:Decay model for implementation of generic 2 body
//             decay specified by the helicity amplitudes
//
// Modification history:
//
//    RYD      March 14, 1999         Module created
//
//------------------------------------------------------------------------

#ifndef EVTHELAMP_HH
#define EVTHELAMP_HH

#include "EvtGenBase/EvtDecayAmp.hh"
#include "EvtGenBase/EvtId.hh"

class EvtParticle;
class EvtEvalHelAmp;

class EvtHelAmp:public  EvtDecayAmp{

public:

  EvtHelAmp() {}
  virtual ~EvtHelAmp();

  void getName(std::string& name);
  EvtDecayBase* clone();

  void init();
  void initProbMax();

  void decay(EvtParticle *p); 


private:

  void fillHelicity(int* lambda2,int n,int J2);

  EvtEvalHelAmp* _evalHelAmp;

};

#endif






