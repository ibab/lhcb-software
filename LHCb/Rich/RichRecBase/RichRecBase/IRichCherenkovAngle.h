// $Id: IRichCherenkovAngle.h,v 1.2 2004-02-02 14:23:01 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHCHERENKOVANGLE_H
#define RICHRECTOOLS_IRICHCHERENKOVANGLE_H 1

// Event model
class RichRecSegment;

/** @class IRichCherenkovAngle IRichCherenkovAngle.h
 *
 *  Interface for tool calculating the expected Cherenkov angle
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichCherenkovAngle( "IRichCherenkovAngle", 1, 0 );

class IRichCherenkovAngle : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() {return IID_IRichCherenkovAngle;}

  /// Returns average Cherenkov angle for given particle hypothesis
  virtual double avgCherenkovTheta( RichRecSegment * segment,
                                    const Rich::ParticleIDType id ) const = 0;

  /// Returns average Cherenkov angle for the current mass hypothesis
  /// Assigned to the segment
  virtual double avgCherenkovTheta( RichRecSegment * segment ) const = 0;

};

#endif // RICHRECTOOLS_IRICHCHERENKOVANGLE_H
