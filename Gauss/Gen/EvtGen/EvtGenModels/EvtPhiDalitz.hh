
#ifndef EVTPHIDALITZ_HH
#define EVTPHIDALITZ_HH

#include "EvtGenBase/EvtDecayAmp.hh"

class EvtParticle;

class EvtPhiDalitz:public  EvtDecayAmp  {

public:

  EvtPhiDalitz() {}
  virtual ~EvtPhiDalitz();

  void getName(std::string& name);
  EvtDecayBase* clone();

  void init();

  void decay(EvtParticle *p); 

};

#endif
