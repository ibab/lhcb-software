// $Id: RichBinnedCherenkovResolution.h,v 1.2 2003-11-25 14:06:39 jonrob Exp $
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
                            const Rich::ParticleIDType id = Rich::Pion );

private:  // Private data

  /// Pointer to RichCherenkovAngle interface
  IRichCherenkovAngle * m_ckAngle;

  std::vector<double> m_theerr[Rich::NRadiatorTypes][Rich::Track::NTrTypes];
  std::vector<double> m_thebin[Rich::NRadiatorTypes];

};

#endif // RICHRECTOOLS_RICHBINNEDCHERENKOVRESOLUTION_H
