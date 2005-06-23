
/** @file RichBinnedCKResVthetaForTrStoredTracks.h
 *
 *  Header file for tool : RichBinnedCKResVthetaForTrStoredTracks
 *
 *  CVS Log :-
 *  $Id: RichBinnedCKResVthetaForTrStoredTracks.h,v 1.3 2005-06-23 15:17:41 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.2  2004/07/27 20:15:29  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

#ifndef RICHRECTOOLS_RICHBINNEDCKRESVTHETAFORTRSTOREDTRACKS_H
#define RICHRECTOOLS_RICHBINNEDCKRESVTHETAFORTRSTOREDTRACKS_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichCherenkovAngle.h"

/** @class RichBinnedCKResVthetaForTrStoredTracks RichBinnedCKResVthetaForTrStoredTracks.h
 *
 *  Tool to calculate the Cherenkov angle resolution. This version is
 *  for TrStoredTracks and implements an approach that uses simple bins in the
 *  cherenkov angle theta space and also varies according to the track algorithm type.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichBinnedCKResVthetaForTrStoredTracks : public RichRecToolBase,
                                               virtual public IRichCherenkovResolution {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichBinnedCKResVthetaForTrStoredTracks( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent );

  /// Destructor
  virtual ~RichBinnedCKResVthetaForTrStoredTracks() {};

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Photon resolution
  double ckThetaResolution( RichRecSegment * segment,
                            const Rich::ParticleIDType id = Rich::Pion ) const;

private:  // Private data

  /// Pointer to RichCherenkovAngle interface
  const IRichCherenkovAngle * m_ckAngle;

  /// The averged resolutions in each bin, for each track type
  std::vector<double> m_theerr[Rich::NRadiatorTypes][Rich::Track::NTrTypes];

  /// The boundaries for the resolution bins
  std::vector<double> m_binEdges[Rich::NRadiatorTypes];

};

#endif // RICHRECTOOLS_RICHBINNEDCKRESVTHETAFORTRSTOREDTRACKS_H
