// $Id: RichBinnedCherenkovResolution.h,v 1.3 2004-02-02 14:26:57 jonesc Exp $
#ifndef RICHRECTOOLS_RICHBINNEDCHERENKOVRESOLUTION_H
#define RICHRECTOOLS_RICHBINNEDCHERENKOVRESOLUTION_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichCherenkovAngle.h"

/** @class RichBinnedCherenkovResolution RichBinnedCherenkovResolution.h
 *
 *  Tool providing Cherenkova angle calulcations
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichBinnedCherenkovResolution : public RichRecToolBase,
                                      virtual public IRichCherenkovResolution {

public:

  /// Standard constructor
  RichBinnedCherenkovResolution( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

  /// Destructor
  virtual ~RichBinnedCherenkovResolution() {};

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

  /// The averged resolutions in each bin, for each track type
  std::vector<double> m_theerr[Rich::NRadiatorTypes][Rich::Track::NTrTypes];

  /// The boundaries for the resolution bins
  std::vector<double> m_binEdges[Rich::NRadiatorTypes];

};

#endif // RICHRECTOOLS_RICHBINNEDCHERENKOVRESOLUTION_H
