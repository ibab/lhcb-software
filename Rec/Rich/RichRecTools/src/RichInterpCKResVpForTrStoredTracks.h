// $Id: RichInterpCKResVpForTrStoredTracks.h,v 1.2 2004-06-18 11:21:30 jonesc Exp $
#ifndef RICHRECTOOLS_RICHINTERPCKRESVPFORTRSTOREDTRACKS_H
#define RICHRECTOOLS_RICHINTERPCKRESVPFORTRSTOREDTRACKS_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// RichKernel
#include "RichKernel/Rich1DTabFunc.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// interfaces
#include "RichRecBase/IRichCherenkovResolution.h"

/** @class RichInterpCKResVpForTrStoredTracks RichInterpCKResVpForTrStoredTracks.h
 *
 *  Tool to calculate the Cherenkov angle resolution. This implementation is
 *  for TrStoredTracks and uses an interpolation of the error against the
 *  segment momentum
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichInterpCKResVpForTrStoredTracks : public RichRecToolBase,
                                           virtual public IRichCherenkovResolution {

public:

  /// Standard constructor
  RichInterpCKResVpForTrStoredTracks( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent );

  /// Destructor
  virtual ~RichInterpCKResVpForTrStoredTracks() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Photon resolution
  double ckThetaResolution( RichRecSegment * segment,
                            const Rich::ParticleIDType id = Rich::Pion ) const;

private:  // Private data

  // data containers from job options
  std::vector<double> m_theerr[Rich::NRadiatorTypes][Rich::Track::NTrTypes];
  std::vector<double> m_thebin[Rich::NRadiatorTypes];

  /// Interpolators
  Rich1DTabFunc * m_ckRes[Rich::NRadiatorTypes][Rich::Track::NTrTypes];

};

#endif // RICHRECTOOLS_RICHINTERPCKRESVPFORTRSTOREDTRACKS_H
