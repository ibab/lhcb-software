#ifndef LHCBEVENT_MCVELORCLUSTER_H
#define LHCBEVENT_MCVELORCLUSTER_H 1

/// Includes
#include "VeloEvent/MCVeloCluster.h"
#include "VeloEvent/VeloRCluster.h"
#include "VeloEvent/MCVeloRDigit.h"

/// Externals 
static const CLID CLID_MCVeloRCluster = 4042;

/**
 * Vertex detector Monte Carlo R clusters.
 * This class extends the R cluster by adding Monte Carlo information to
 * the cluster. Links are implemented to the MCParticle(s) which contribute
 * to this cluster. To access the MCHits you need to go via the digits.
 */

class MCVeloRCluster : public VeloRCluster, public MCVeloCluster {

public:

  /// Constructor
  MCVeloRCluster() {

  }

  /// Destructor
  virtual ~MCVeloRCluster() {

  }

  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return MCVeloRCluster::classID(); 
  }
  static const CLID& classID() { 
    return CLID_MCVeloRCluster;
  }

  /// Access the Monte Carlo digitized hits used to build this cluster
  SmartRefVector<MCVeloRDigit>& digits();
  const SmartRefVector<MCVeloRDigit>& digits() const;

private:

  /// References to Monte Carlo digitised hits
  SmartRefVector<MCVeloRDigit> m_digits;

};

#endif //LHCBEVENT_MCVELORCLUSTER_H 
