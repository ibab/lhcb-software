// $Id: HltLHCbIDMCParticleRelation.h,v 1.2 2009-10-08 19:39:00 graven Exp $
#ifndef HLTLHCBIDMCPARTICLERELATION_H 
#define HLTLHCBIDMCPARTICLERELATION_H 1


// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from LinkerEvent
#include "Linker/LinkedTo.h"

#include "Event/MCParticle.h"
#include "Kernel/LHCbID.h"


#include "Event/VeloCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"

/** @class HltLHCbIDMCParticleRelation HltLHCbIDMCParticleRelation.h
 *  Build the linker table relating LHCbID used in pattern to MCParticles
 *
 *  @date   2005-06-01
 */
class HltLHCbIDMCParticleRelation : public GaudiAlgorithm {
public: 

  HltLHCbIDMCParticleRelation( const std::string& name, 
                               ISvcLocator* pSvcLocator );

  virtual ~HltLHCbIDMCParticleRelation( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::string m_relationLocation;
};

#endif // PATLHCBID2MCPARTICLE_H
