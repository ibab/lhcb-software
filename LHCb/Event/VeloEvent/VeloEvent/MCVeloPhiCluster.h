#ifndef LHCBEVENT_MCVELOPHICLUSTER_H
#define LHCBEVENT_MCVELOPHICLUSTER_H 1

/// Includes
#include "VeloEvent/MCVeloCluster.h"
#include "VeloEvent/VeloPhiCluster.h"
#include "VeloEvent/MCVeloPhiDigit.h"

/// Externals 
static const CLID CLID_MCVeloPhiCluster = 4043;

/**
 * Vertex detector Monte Carlo Phi clusters.
 * This class extends the Phi cluster by adding Monte Carlo information to
 * the cluster. Links are implemented to the MCParticle(s) which contribute
 * to this cluster. To access the MCHits you need to go via the digits.
 */

class MCVeloPhiCluster : public VeloPhiCluster, public MCVeloCluster {

public:

  /// Constructor
  MCVeloPhiCluster() {

  }

  /// Destructor
  virtual ~MCVeloPhiCluster() {

  }

  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return MCVeloPhiCluster::classID(); 
  }
  static const CLID& classID() { 
    return CLID_MCVeloPhiCluster;
  }

  /// Access the Monte Carlo digitized hits used to build this cluster
  SmartRefVector<MCVeloPhiDigit>& digits();
  const SmartRefVector<MCVeloPhiDigit>& digits() const;

private:

  /// References to Monte Carlo digitised hits
  SmartRefVector<MCVeloPhiDigit> m_digits;

};

#endif //LHCBEVENT_MCVELOPHICLUSTER_H 











