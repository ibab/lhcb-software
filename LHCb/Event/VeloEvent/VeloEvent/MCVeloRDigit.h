#ifndef LHCBEVENT_MCVELORDIGIT_H
#define LHCBEVENT_MCVELORDIGIT_H 1

/// Includes
#include "VeloEvent/MCVeloDigit.h"
#include "VeloEvent/VeloRDigit.h"

/// Externals
static const CLID CLID_MCVeloRDigit = 4012;

/**
 * Vertex detector Monte Carlo R digitisation.
 * This class extends the R digitisation by adding Monte Carlo information to
 * the cluster. Links are implemented to the MCParticle(s) which contribute
 * to this digitisation.
 */

class MCVeloRDigit : public VeloRDigit, public MCVeloDigit {

public:

  /// Constructor
  MCVeloRDigit() {

  }

  /// Destructor
  virtual ~MCVeloRDigit() {

  }

  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return MCVeloRDigit::classID(); 
  }
  static const CLID& classID() { 
    return CLID_MCVeloRDigit; 
  }

private:

};

#endif /// LHCBEVENT_MCVELORDIGIT_H
