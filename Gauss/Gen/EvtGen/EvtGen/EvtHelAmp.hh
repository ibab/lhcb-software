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

#include "EvtGen/EvtDecayAmp.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtId.hh"

class EvtHelAmp:public  EvtDecayAmp{

public:

  EvtHelAmp() {}
  virtual ~EvtHelAmp();

  void getName(EvtString& name);
  EvtDecayBase* clone();

  void init();
  void initProbMax();

  void decay(EvtParticle *p); 


private:

  void fillHelicity(int* lambda2,int n,int J2);
  void setUpRotationMatrices(EvtParticle *p,double theta, double phi);
  void applyRotationMatrices();

  //spins states available for particle A, B, and C.
  int _nA,_nB,_nC;

  //helicity amplitudes
  EvtComplexPtrPtr _HBC;

  //2 times spin for each of the particles
  int _JA2,_JB2,_JC2;

  //2 times the helicity for the states
  int *_lambdaA2,*_lambdaB2,*_lambdaC2;

  //Rotation matrices
  EvtComplexPtrPtr _RA,_RB,_RC;

  //temporary array for amplitudes
  EvtComplexPtrPtrPtr _amp,_amp1,_amp3;

  EvtId _idA,_idB,_idC;

};

#endif






