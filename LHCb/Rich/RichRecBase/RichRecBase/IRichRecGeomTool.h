
//-----------------------------------------------------------------------------
/** @file IRichRecGeomTool.h
 *
 *  Header file for RICH reconstruction tool interface : IRichRecGeomTool
 *
 *  CVS Log :-
 *  $Id: IRichRecGeomTool.h,v 1.2 2004-07-26 18:00:57 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHRECGEOMTOOL_H
#define RICHRECTOOLS_IRICHRECGEOMTOOL_H 1

// Event
class RichRecPixel;
class RichRecSegment;

/// Static Interface Identification
static const InterfaceID IID_IRichRecGeomTool( "IRichRecGeomTool" , 1 , 0 );

/** @class IRichRecGeomTool IRichRecGeomTool.h
 *
 *  Interface for a tool to answer simple geometrical questions
 *  using the reconstruction event model
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class IRichRecGeomTool : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichRecGeomTool; }

  /** Computes the square of distance separating the pixel hit and hit position extrapolated
   *  using the RichRecSegment direction in global coordinates
   *  
   *  @param segment Pointer to the RichRecSegment
   *  @param pixel   Pointer to the RichRecPixel
   *
   *  @return The separation squared in global coordinates
   */
  virtual double trackPixelHitSep2Global( const RichRecSegment * segment,
                                          const RichRecPixel * pixel ) const = 0;

  /** Returns square of distance separating the pixel hit and hit position extrapolated
   *  using the RichRecSegment direction in local HPD panel coordinates
   *  
   *  @param segment Pointer to the RichRecSegment
   *  @param pixel   Pointer to the RichRecPixel
   *
   *  @return The separation squared in local HPD panel coordinates
   */
  virtual double trackPixelHitSep2Local( const RichRecSegment * segment,
                                         const RichRecPixel * pixel ) const = 0;

};

#endif // RICHRECTOOLS_IRICHRECGEOMTOOL_H
