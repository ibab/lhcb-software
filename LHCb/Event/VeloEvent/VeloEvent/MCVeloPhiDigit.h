#ifndef LHCBEVENT_MCVELOPHIDIGIT_H
#define LHCBEVENT_MCVELOPHIDIGIT_H 1

/// Includes
#include "VeloEvent/MCVeloDigit.h"
#include "VeloEvent/VeloPhiDigit.h"

/// Externals
static const CLID CLID_MCVeloPhiDigit = 4003;

/**
 * Vertex detector Monte Carlo Phi digitisation.
 * This class extends the Phi digitisation by adding Monte Carlo information to
 * the cluster. Links are implemented to the MCParticle(s) which contribute
 * to this digitisation.
 */

class MCVeloPhiDigit : public VeloPhiDigit, public MCVeloDigit {

public:

  /// Constructor
  MCVeloPhiDigit() {

  }

  /// Destructor
  virtual ~MCVeloPhiDigit() {

  }

  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return MCVeloPhiDigit::classID(); 
  }
  static const CLID& classID() { 
    return CLID_MCVeloPhiDigit; 
  }

private:

};

#endif /// LHCBEVENT_MCVELOPHIDIGIT_H







