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
// Module: EvtGen/EvtbTosllAmp.hh
//
// Description:
//
// Modification history:
//
//    RYD     January 30 2000         Module created
//
//------------------------------------------------------------------------

#ifndef EVTBTOSLLAMP_HH
#define EVTBTOSLLAMP_HH

class EvtAmp;
class EvtId;
class EvtbTosllFF;
class EvtParticle;

class EvtbTosllAmp{

 public:

  //Daughters are initialized and have been added to the parent.
  //No need to carry around the daughters seperately!

  virtual void CalcAmp( EvtParticle *parent, EvtAmp& amp,
			EvtbTosllFF *formFactors )=0;

  double CalcMaxProb( EvtId parent, EvtId meson, EvtId lepton,
		      EvtId nudaug, EvtbTosllFF *formFactors,
		      double& poleSize);


};

#endif
