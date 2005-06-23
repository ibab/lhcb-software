
//----------------------------------------------------------------------------------------
/** @file RichFunctionalCKResVpForTrStoredTracks.h
 *
 *  Header file for tool : RichFunctionalCKResVpForTrStoredTracks
 *
 *  CVS Log :-
 *  $Id: RichFunctionalCKResVpForTrStoredTracks.h,v 1.4 2005-06-23 15:17:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/10/2004
 */
//----------------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHFUNCTIONALCKRESVPFORTRSTOREDTRACKS_H
#define RICHRECTOOLS_RICHFUNCTIONALCKRESVPFORTRSTOREDTRACKS_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichCherenkovResolution.h"

//----------------------------------------------------------------------------------------
/** @class RichFunctionalCKResVpForTrStoredTracks RichFunctionalCKResVpForTrStoredTracks.h
 *
 *  Provides an analytical calculation of the expected Cherenkoc angle resolution.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-04-07
 *
 *  @todo Finish this implementation using Results from Ring Refitting work.
 */
//----------------------------------------------------------------------------------------

class RichFunctionalCKResVpForTrStoredTracks : public RichRecToolBase,
                                               virtual public IRichCherenkovResolution {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichFunctionalCKResVpForTrStoredTracks( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent);

  /// Destructor
  virtual ~RichFunctionalCKResVpForTrStoredTracks() {};

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Photon resolution
  double ckThetaResolution( RichRecSegment * segment,
                            const Rich::ParticleIDType id = Rich::Pion ) const;

private:

};

#endif // RICHRECTOOLS_RICHFUNCTIONALCKRESVPFORTRSTOREDTRACKS_H
