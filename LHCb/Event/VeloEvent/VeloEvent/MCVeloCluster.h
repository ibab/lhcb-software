#ifndef LHCBEVENT_MCVELOCLUSTER_H
#define LHCBEVENT_MCVELOCLUSTER_H 1

/// Includes
#include "GaudiKernel/SmartRefVector.h"

/// Forward declarations
class MCParticle;

/// Externals 
static const CLID CLID_MCVeloCluster = 4041;

/**
 * Vertex detector Monte Carlo cluster extension class.
 */

class MCVeloCluster {

public:

  /// Constructor
  MCVeloCluster() {

  }

  /// Destructor
  virtual ~MCVeloCluster() {

  }

  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return MCVeloCluster::classID(); 
  }
  static const CLID& classID() { 
    return CLID_MCVeloCluster;
  }

  /// Access to references to Monte Carlo particles contributing to this cluster
  SmartRefVector<MCParticle>& particles();
  /// Access to references to Monte Carlo particles contributing to this cluster
  const SmartRefVector<MCParticle>& particles() const;
  
private:

  /// References to Monte Carlo particles contributing to this cluster
  SmartRefVector<MCParticle> m_particles;

};

#include "LHCbEvent/MCParticle.h"

/// Access to references to Monte Carlo particles contributing to this cluster
inline SmartRefVector<MCParticle>& MCVeloCluster::particles() {
  return m_particles;
}

/// Access to references to Monte Carlo particles contributing to this cluster
inline const SmartRefVector<MCParticle>& MCVeloCluster::particles() const {
  return m_particles;
}

#endif // LHCBEVENT_MCVELOCLUSTER_H






