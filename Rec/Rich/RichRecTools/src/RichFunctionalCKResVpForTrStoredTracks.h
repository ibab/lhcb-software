// $Id: RichFunctionalCKResVpForTrStoredTracks.h,v 1.1 2004-04-19 23:06:09 jonesc Exp $
#ifndef RICHRECTOOLS_RICHFUNCTIONALCKRESVPFORTRSTOREDTRACKS_H
#define RICHRECTOOLS_RICHFUNCTIONALCKRESVPFORTRSTOREDTRACKS_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichCherenkovResolution.h"

/** @class RichFunctionalCKResVpForTrStoredTracks RichFunctionalCKResVpForTrStoredTracks.h
 *
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-04-07
 */

class RichFunctionalCKResVpForTrStoredTracks : public RichRecToolBase,
                                               virtual public IRichCherenkovResolution {

public:

  /// Standard constructor
  RichFunctionalCKResVpForTrStoredTracks( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent);

  /// Destructor
  virtual ~RichFunctionalCKResVpForTrStoredTracks() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Photon resolution
  double ckThetaResolution( RichRecSegment * segment,
                            const Rich::ParticleIDType id = Rich::Pion ) const;

private:

};

#endif // RICHRECTOOLS_RICHFUNCTIONALCKRESVPFORTRSTOREDTRACKS_H
