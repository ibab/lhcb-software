// $Id: RichCherenkovAngle.h,v 1.3 2004-02-02 14:26:57 jonesc Exp $
#ifndef RICHRECTOOLS_RICHCHERENKOVANGLE_H
#define RICHRECTOOLS_RICHCHERENKOVANGLE_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichDetTools/IRichRefractiveIndex.h"
#include "RichRecBase/IRichParticleProperties.h"

/** @class RichCherenkovAngle RichCherenkovAngle.h
 *
 *  Tool calculating the expected Cherenkov angle
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichCherenkovAngle : public RichRecToolBase,
                           virtual public IRichCherenkovAngle {

public:

  /// Standard constructor
  RichCherenkovAngle( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  /// Destructor
  virtual ~RichCherenkovAngle() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Returns average Cherenkov angle for given particle hypothesis
  double avgCherenkovTheta( RichRecSegment * segment,
                            const Rich::ParticleIDType id ) const;

  /// Returns average Cherenkov angle for the current mass hypothesis
  /// Assigned to the segment
  double avgCherenkovTheta( RichRecSegment * segment ) const;

private:  // Private data

  /// Pointer to RichExpectedTrackSignal interface
  IRichExpectedTrackSignal * m_signal;

  /// Pointer to RichParticleProperties interface
  IRichParticleProperties * m_richPartProp;

  /// Pointer to refractive index tool
  IRichRefractiveIndex * m_refIndex;

};

#endif // RICHRECTOOLS_RICHCHERENKOVANGLE_H
