// $Id: RichInterpCKResVthetaForTrStoredTracks.h,v 1.1 2004-04-19 23:06:11 jonesc Exp $
#ifndef RICHRECTOOLS_RICHINTERPCKRESVTHETAFORTRSTOREDTRACKS_H
#define RICHRECTOOLS_RICHINTERPCKRESVTHETAFORTRSTOREDTRACKS_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// RichUtils
#include "RichUtils/Rich1DTabFunc.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// interfaces
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichCherenkovAngle.h"

/** @class RichInterpCKResVthetaForTrStoredTracks RichInterpCKResVthetaForTrStoredTracks.h
 *
 *  Tool to calculate the Cherenkov angle resolution. This implementation is
 *  for TrStoredTracks and uses an interpolation of the error against the
 *  cherenkov angle theta
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichInterpCKResVthetaForTrStoredTracks : public RichRecToolBase,
                                               virtual public IRichCherenkovResolution {

public:

  /// Standard constructor
  RichInterpCKResVthetaForTrStoredTracks( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent );

  /// Destructor
  virtual ~RichInterpCKResVthetaForTrStoredTracks() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Photon resolution
  double ckThetaResolution( RichRecSegment * segment,
                            const Rich::ParticleIDType id = Rich::Pion ) const;

private:  // Private data

  /// Pointer to RichCherenkovAngle interface
  IRichCherenkovAngle * m_ckAngle;

  // data containers from job options
  std::vector<double> m_theerr[Rich::NRadiatorTypes][Rich::Track::NTrTypes];
  std::vector<double> m_thebin[Rich::NRadiatorTypes];

  /// Interpolators
  Rich1DTabFunc * m_ckRes[Rich::NRadiatorTypes][Rich::Track::NTrTypes];

};

#endif // RICHRECTOOLS_RICHINTERPCKRESVTHETAFORTRSTOREDTRACKS_H
