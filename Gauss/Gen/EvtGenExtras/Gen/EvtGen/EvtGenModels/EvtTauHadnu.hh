//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998     Caltech, UCSB
//
// Module: EvtGen/EvtTauHadnu.hh
//
// Description:
//
// Modification history:
//
//  Lange Oct 26, 2002 Created
//
//------------------------------------------------------------------------

#ifndef EVTTAUHADNU_HH
#define EVTTAUHADNU_HH

#include "EvtGenBase/EvtDecayAmp.hh"

class EvtParticle;

class EvtTauHadnu : public  EvtDecayAmp  {

public:
  
  EvtTauHadnu() {}
  virtual ~EvtTauHadnu();

  void getName(std::string& name);
  EvtDecayBase* clone();

  void initProbMax();
  void init();
  void decay(EvtParticle *p); 

private:
  double _beta;
  double  _mRho;
  double  _gammaRho;
  double  _mRhopr;
  double  _gammaRhopr;
  double _mA1;
  double _gammaA1;

  EvtComplex Fpi( EvtVector4R q1, EvtVector4R q2);
  double pi3G(double m2,int dupD);

};

#endif
