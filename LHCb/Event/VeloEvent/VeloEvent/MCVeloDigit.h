#ifndef LHCBEVENT_MCVELODIGIT_H
#define LHCBEVENT_MCVELODIGIT_H 1

/// Includes
#include "GaudiKernel/SmartRefVector.h"

/// Forward declarations
class MCTrackingHit;

/// Externals
static const CLID CLID_MCVeloDigit = 4011;

/**
 * Vertex detector Monte Carlo digitisation extension class.
 */

class MCVeloDigit {

public:

  /// Constructor
  MCVeloDigit() {

  }

  /// Destructor
  virtual ~MCVeloDigit() {

  }

  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return MCVeloDigit::classID(); 
  }
  static const CLID& classID() { 
    return CLID_MCVeloDigit; 
  }

  /// Access to references to Monte Carlo hits contributing to this digitisation
  SmartRefVector<MCTrackingHit>& hits();
  /// Access to references to Monte Carlo hits contributing to this digitisation
  const SmartRefVector<MCTrackingHit>& hits() const;

private:

  /// References to Monte Carlo hits contributing to this digitisation
  SmartRefVector<MCTrackingHit> m_hits;

};

#include "LHCbEvent/MCTrackingHit.h"

/// Access to references to Monte Carlo hits contributing to this digitisation
inline SmartRefVector<MCTrackingHit>& MCVeloDigit::hits() {
  return m_hits;
}

/// Access to references to Monte Carlo hits contributing to this digitisation
inline const SmartRefVector<MCTrackingHit>& MCVeloDigit::hits() const {
  return m_hits;
}

#endif // LHCBEVENT_MCVELODIGIT_H
