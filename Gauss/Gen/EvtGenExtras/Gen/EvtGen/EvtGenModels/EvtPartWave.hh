//--------------------------------------------------------------------------
// $Id: EvtPartWave.hh,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtGen/EvtPartWave.hh
//
// Description:Decay model for implementation of generic 2 body
//             decay specified by the partial waves amplitudes
//
// Modification history:
//
//    RYD      September 7, 1999         Module created
//
//------------------------------------------------------------------------

#ifndef EVTPARTWAVE_HH
#define EVTPARTWAVE_HH

#include "EvtGenBase/EvtDecayAmp.hh"
#include "EvtGenBase/EvtId.hh"

class EvtParticle;
class EvtEvalHelAmp;

class EvtPartWave:public  EvtDecayAmp{

public:

  EvtPartWave() : _evalHelAmp( 0 ) , _HBC( 0 ) {}
  virtual ~EvtPartWave();

  void getName(std::string& name);
  EvtDecayBase* clone();

  void init();
  void initProbMax();

  void decay(EvtParticle *p); 


private:

  void fillHelicity(int* lambda2,int n,int J2);

  EvtEvalHelAmp* _evalHelAmp;
  EvtComplexPtr * _HBC ;
  int _nB ;
  

};

#endif






