// $Id: EvtSSD_DirectCP.hh,v 1.1 2006-11-08 23:16:11 robbep Exp $
// Generation of direct CP violation in hadronic environment
// Patrick Robbe, LHCb,  08 Nov 2006
// 

#ifndef EVTSSD_DirectCP_HH
#define EVTSSD_DirectCP_HH

#include "EvtGenBase/EvtDecayAmp.hh"

class EvtParticle;

class EvtSSD_DirectCP : public  EvtDecayAmp  {

public:

  EvtSSD_DirectCP() {}
  virtual ~EvtSSD_DirectCP();
  
  void getName(std::string& name);
  EvtDecayBase* clone();

  void initProbMax();
  void init();
  void decay(EvtParticle *p); 

private:

  //Arguments

  double _acp;
};

#endif
