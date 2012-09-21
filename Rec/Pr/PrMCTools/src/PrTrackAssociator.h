#ifndef PRTRACKASSOCIATOR_H 
#define PRTRACKASSOCIATOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from Event/LinkerEvent
#include "Linker/LinkedTo.h"

// Forward declarations

namespace LHCb {
  class MCParticle;
}

class TruthCounter {
public:
  TruthCounter( ){ reset(); }
  ~TruthCounter() {};

  void reset() {
    particle = NULL;
    nVelo    = 0.;
    nTT      = 0.;
    nT       = 0.;
  }

  const LHCb::MCParticle* particle;
  float nVelo;
  float nTT;
  float nT;
};


/** @class TrackAssociator TrackAssociator.h
 *
 *  This algorithm computes the link between a Track and a MCParticle.
 *  The requirement is a match of both the Velo/VP and the T part of the
 *  Track. If there are not enough coordinates, the match is assumed so that
 *  a Velo only or a T only are matched properly.
 *  The required fraction of hits is a jobOption 'FractionOK', default 0.70.
 *
 *  Rewritten for the upgrade, handles all containers in one instance
 *
 *  @author Olivier Callot
 *  @date   2012-04-04
 */

class PrTrackAssociator : public GaudiAlgorithm {
public:

  // Standard constructor
  PrTrackAssociator( const std::string& name, ISvcLocator* pSvcLocator );

  // Destructor
  virtual ~PrTrackAssociator();

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute();     ///< Algorithm execution

private:
  void incrementVelo( const LHCb::MCParticle* part );
  void incrementTT  ( const LHCb::MCParticle* part );
  void incrementT   ( const LHCb::MCParticle* part );

  // jobOptions
  std::string m_rootOfContainers;   ///< root in TES of all the track containers to process
  std::string m_singleContainer;
  double      m_fractionOK;         ///< minimal good matching fraction

  std::vector<TruthCounter> m_truthCounters; ///< vector of counters of associated MC particles
  TruthCounter m_total;                      ///< total number of measurements
};
#endif // PRTRACKASSOCIATOR_H
