// $Id: IRichPhotonPredictor.h,v 1.5 2004-07-09 17:40:04 jonrob Exp $
#ifndef RICHRECTOOLS_IRICHPHOTONPREDICTOR_H
#define RICHRECTOOLS_IRICHPHOTONPREDICTOR_H 1

// Event
class RichRecPixel;
class RichRecSegment;

/** @class IRichPhotonPredictor IRichPhotonPredictor.h
 *
 *  Interface for a simple tool to perform a relatively fast determination of whether
 *  a valid RichRecPhoton candidate can be produced using a given RichRecSegment
 *  and RichRecPixel. The means by which this is determined depends on the particular 
 *  instance of the tool used.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichPhotonPredictor( "IRichPhotonPredictor" , 1 , 0 );

class IRichPhotonPredictor : public virtual IAlgTool {

public:

  /// static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichPhotonPredictor; }

  /// Is it possible to make a photon candidate using this segment and pixel.
  virtual bool photonPossible( RichRecSegment * segment,
                               RichRecPixel * pixel ) const = 0;

};

#endif // RICHRECTOOLS_IRICHPHOTONPREDICTOR_H
