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
// Module: EvtGen/EvtPhspFlatLifetime.hh
//
// Description:
//   Class to handle generic phase space decays not done
//   in other decay models, with a flat lifetime
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//    PR          July 11, 2013           Add flat lifetime generation
//------------------------------------------------------------------------

#ifndef EVTPHSPFLATLIFETIME_HH
#define EVTPHSPFLATLIFETIME_HH

#include "EvtGenBase/EvtDecayIncoherent.hh"

class EvtParticle;

class EvtPhspFlatLifetime : public  EvtDecayIncoherent  {
public:
  /// Constructor
  EvtPhspFlatLifetime() : m_maxLifetime( 0. ) {} ;

  /// Destructor
  virtual ~EvtPhspFlatLifetime() {} ;

  /// return name of the model
  virtual std::string getName();

  /// Clone
  virtual EvtDecayBase* clone();

  /// Compute maximum weight
  virtual void initProbMax();

  /// Initialize the model
  virtual void init();

  /// Perform the decay
  virtual  void decay(EvtParticle *p);

private:
  /// parameter of the model: maximum of the generated lifetime (in ps)
  double m_maxLifetime ;
};

#endif

