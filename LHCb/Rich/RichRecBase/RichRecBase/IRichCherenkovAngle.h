
//-----------------------------------------------------------------------------
/** @file IRichCherenkovAngle.h
 *
 *  Header file for RICH reconstruction tool interface : IRichCherenkovAngle
 *
 *  CVS Log :-
 *  $Id: IRichCherenkovAngle.h,v 1.3 2004-07-26 18:00:57 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHCHERENKOVANGLE_H
#define RICHRECTOOLS_IRICHCHERENKOVANGLE_H 1

// Event model
class RichRecSegment;

/// Static Interface Identification
static const InterfaceID IID_IRichCherenkovAngle( "IRichCherenkovAngle", 1, 0 );

/** @class IRichCherenkovAngle IRichCherenkovAngle.h
 *
 *  Interface for tool calculating the expected Cherenkov angle
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class IRichCherenkovAngle : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() {return IID_IRichCherenkovAngle;}

  /** Computes the average Cherenkov angle for given particle hypothesis
   * 
   *  @param segment The RichRecSegment for which to coumpute the expected CK angle
   *  @param id      The mass hypothesis for which the angle should be calculated
   *
   *  @return The expected average cherenkov angle
   */
  virtual double avgCherenkovTheta( RichRecSegment * segment,
                                    const Rich::ParticleIDType id ) const = 0;

  /** Computes the average Cherenkov angle for the current mass hypothesis
   *  assigned to the given segment
   *
   *  @param segment The RichRecSegment for which to coumpute the expected CK angle
   *
   *  @return The expected average cherenkov angle
   */
  virtual double avgCherenkovTheta( RichRecSegment * segment ) const = 0;

};

#endif // RICHRECTOOLS_IRICHCHERENKOVANGLE_H
