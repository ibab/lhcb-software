
//-----------------------------------------------------------------------------
/** @file IRichGasQuartzWindowAbs.h
 *
 *  Header file for RICH reconstruction tool interface : IRichGasQuartzWindowAbs
 *
 *  CVS Log :-
 *  $Id: IRichGasQuartzWindowAbs.h,v 1.2 2004-07-26 18:00:57 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHGASQUARTZWINDOWABS_H
#define RICHRECTOOLS_IRICHGASQUARTZWINDOWABS_H 1

// Event model
class RichRecSegment;

/// Static Interface Identification
static const InterfaceID IID_IRichGasQuartzWindowAbs( "IRichGasQuartzWindowAbs", 1, 0 );

/** @class IRichGasQuartzWindowAbs IRichGasQuartzWindowAbs.h
 *
 *  Interface for tool to calculate transmission properties for the
 *  gas quartz window
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class IRichGasQuartzWindowAbs : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() {return IID_IRichGasQuartzWindowAbs;}

  /** Calculates the transmission probability through the gas quartz window for
   *  a given pathlength (segment) and photon energy
   *
   *  @param segment Pointer to a RichRecSegment
   *  @param energy photon energy
   *
   *  @return The transmission probability through the quartz window
   */
  virtual double photonTransProb( const RichRecSegment * segment,
                                  const double energy ) const = 0;

};

#endif // RICHRECTOOLS_IRICHGASQUARTZWINDOWABS_H
