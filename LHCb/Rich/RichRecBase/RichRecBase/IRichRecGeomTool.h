// $Id: IRichRecGeomTool.h,v 1.1 2004-06-10 14:14:48 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHRECGEOMTOOL_H
#define RICHRECTOOLS_IRICHRECGEOMTOOL_H 1

// Event
class RichRecPixel;
class RichRecSegment;

/** @class IRichRecGeomTool IRichRecGeomTool.h
 *
 *  Interface for a simple tool to answer simple geometrical questions
 *  using the reconstruction event model
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRecGeomTool( "IRichRecGeomTool" , 1 , 0 );

class IRichRecGeomTool : public virtual IAlgTool {

public:

  /// static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichRecGeomTool; }

  /// Returns square of distance seperating the pixel hit and hit position extrapolated
  /// using the RichRecSegment direction in global corrdinates
  virtual double trackPixelHitSep2Global( const RichRecSegment * segment,
                                          const RichRecPixel * pixel ) const = 0;

  /// Returns square of distance seperating the pixel hit and hit position extrapolated
  /// using the RichRecSegment direction in local corrdinates
  virtual double trackPixelHitSep2Local( const RichRecSegment * segment,
                                         const RichRecPixel * pixel ) const = 0;

};

#endif // RICHRECTOOLS_IRICHRECGEOMTOOL_H
