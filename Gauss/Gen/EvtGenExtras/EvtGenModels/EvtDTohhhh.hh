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
// Module: EvtGen/EvtToDhhhh.hh
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#ifndef EvtDHHHH_HH
#define EvtDHHHH_HH

#include "EvtGenBase/EvtDecayIncoherent.hh"


// MINT
#include "Mint/IMintGen.h"
#include "Mint/D_hhhh.h"


class EvtParticle;

class EvtDTohhhh:public  EvtDecayIncoherent  {

public:
  EvtDTohhhh() {}
  virtual ~EvtDTohhhh();

  std::string getName();
  EvtDecayBase* clone();

  void init();
  void decay(EvtParticle *p); 

private:
  MINT::IMintGen* m_MintGen;

};

#endif
