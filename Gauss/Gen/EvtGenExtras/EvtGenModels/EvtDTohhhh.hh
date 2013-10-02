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
// #include "Mint/counted_ptr.h"
#include "Mint/EvtTRandom.h"

class EvtParticle;

namespace MINT {
  class EvtRand : virtual public IEvtRandom
  {
   public: 
    /// Standard constructor
    EvtRand(); 
    EvtRand(const MINT::EvtRand& other);
    virtual ~EvtRand( );
    virtual void SetSeed(unsigned int seed);
    virtual unsigned int GetSeed() const;
    virtual double Rndm(int i);
  private:
    unsigned int m_nCalls; // number of random numbers generated
  };
}
    
class EvtDTohhhh : public EvtDecayIncoherent 
{
public:
  EvtDTohhhh() {}
  
  virtual ~EvtDTohhhh();

  std::string getName();
  EvtDecayBase* clone();

  void init();
  void decay(EvtParticle *p); 

private:
  MINT::IMintGen* m_MintGen;
  MINT::EvtTRandom* m_rnd;
};

#endif
