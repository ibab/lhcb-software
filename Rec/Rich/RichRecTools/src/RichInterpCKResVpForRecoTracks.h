
//---------------------------------------------------------------------------------------
/** @file RichInterpCKResVpForRecoTracks.h
 *
 *  Header file for tool : RichInterpCKResVpForRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichInterpCKResVpForRecoTracks.h,v 1.3 2006-03-02 15:29:19 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//---------------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHINTERPCKRESVPFORRECOTRACKS_H
#define RICHRECTOOLS_RICHINTERPCKRESVPFORRECOTRACKS_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// RichDet
#include "RichDet/Rich1DTabFunc.h"

// CLHEP
#include "Kernel/PhysicalConstants.h"

// interfaces
#include "RichRecBase/IRichCherenkovResolution.h"

//---------------------------------------------------------------------------------------
/** @class RichInterpCKResVpForRecoTracks RichInterpCKResVpForRecoTracks.h
 *
 *  Tool to calculate the Cherenkov angle resolution. This implementation is
 *  for reconstructed Tracks and uses an interpolation of the measured Cherenkov resolution
 *  against the RichRecSegment momentum.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//---------------------------------------------------------------------------------------

class RichInterpCKResVpForRecoTracks : public RichRecToolBase,
                                       virtual public IRichCherenkovResolution
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichInterpCKResVpForRecoTracks( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent );

  /// Destructor
  virtual ~RichInterpCKResVpForRecoTracks() {};

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Photon resolution
  double ckThetaResolution( LHCb::RichRecSegment * segment,
                            const Rich::ParticleIDType id = Rich::Pion ) const;

private:  // Private data

  // data containers from job options
  std::vector<double> m_theerr[Rich::NRadiatorTypes][Rich::Track::NTrTypes];
  std::vector<double> m_thebin[Rich::NRadiatorTypes];

  /// Interpolator for the CK resolution
  Rich1DTabFunc * m_ckRes[Rich::NRadiatorTypes][Rich::Track::NTrTypes];

};

#endif // RICHRECTOOLS_RICHINTERPCKRESVPFORRECOTRACKS_H
