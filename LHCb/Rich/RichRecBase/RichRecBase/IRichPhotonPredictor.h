// $Id: IRichPhotonPredictor.h,v 1.3 2004-04-17 09:28:02 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHPHOTONPREDICTOR_H
#define RICHRECTOOLS_IRICHPHOTONPREDICTOR_H 1

// Event
class RichRecPixel;
class RichRecSegment;

/** @class IRichPhotonPredictor IRichPhotonPredictor.h
 *
 *  Interface for a simple tool to perform a fast determination of whether
 *  a valid RichRecPhoton candidate can be produced using a given RichRecSegment
 *  and RichRecPixel
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichPhotonPredictor( "IRichPhotonPredictor" , 1 , 0 );

class IRichPhotonPredictor : public virtual IAlgTool {

public:

  /// static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichPhotonPredictor; }

  /// Returns square of distance seperating the pixel hit and hit position extrapolated
  /// using the RichRecSegment direction in global corrdinates
  virtual double trackPixelHitSep2Global( const RichRecSegment * segment,
                                          const RichRecPixel * pixel ) const = 0;

  /// Returns square of distance seperating the pixel hit and hit position extrapolated
  /// using the RichRecSegment direction in local corrdinates
  virtual double trackPixelHitSep2Local( const RichRecSegment * segment,
                                         const RichRecPixel * pixel ) const = 0;

  /// Is it possible to make a photon candidate using this segment and pixel.
  virtual bool photonPossible( RichRecSegment * segment,
                               RichRecPixel * pixel ) const = 0;

};

#endif // RICHRECTOOLS_IRICHPHOTONPREDICTOR_H
