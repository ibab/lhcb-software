// $Id: RichInterpCherenkovResolution.h,v 1.2 2003-11-25 14:06:40 jonrob Exp $
#ifndef RICHRECTOOLS_RICHINTERPCHERENKOVRESOLUTION_H
#define RICHRECTOOLS_RICHINTERPCHERENKOVRESOLUTION_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// RichUtils
#include "RichUtils/Rich1DTabFunc.h"

// interfaces
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichCherenkovAngle.h"

/** @class RichInterpCherenkovResolution RichInterpCherenkovResolution.h
 *
 *  Tool providing Cherenkova angle calulcations
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichInterpCherenkovResolution : public RichRecToolBase,
                                      virtual public IRichCherenkovResolution {

public:

  /// Standard constructor
  RichInterpCherenkovResolution( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

  /// Destructor
  virtual ~RichInterpCherenkovResolution() {};

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

  // data containers from job options
  std::vector<double> m_theerr[Rich::NRadiatorTypes][Rich::Track::NTrTypes];
  std::vector<double> m_thebin[Rich::NRadiatorTypes];

  // Interpolators
  Rich1DTabFunc * m_ckRes[Rich::NRadiatorTypes][Rich::Track::NTrTypes];

};

#endif // RICHRECTOOLS_RICHINTERPCHERENKOVRESOLUTION_H
