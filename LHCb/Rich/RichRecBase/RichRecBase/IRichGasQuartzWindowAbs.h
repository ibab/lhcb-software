// $Id: IRichGasQuartzWindowAbs.h,v 1.1 2004-04-17 09:28:02 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHGASQUARTZWINDOWABS_H
#define RICHRECTOOLS_IRICHGASQUARTZWINDOWABS_H 1

// Event model
class RichRecSegment;

/** @class IRichGasQuartzWindowAbs IRichGasQuartzWindowAbs.h
 *
 *  Interface for tool to calculate transmission properties for the
 *  gas quartz window
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichGasQuartzWindowAbs( "IRichGasQuartzWindowAbs", 1, 0 );

class IRichGasQuartzWindowAbs : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() {return IID_IRichGasQuartzWindowAbs;}

  /// Transmission probability for given pathlength (segment) and photon energy
  virtual double photonTransProb( const RichRecSegment * segment,
                                  const double energy ) const = 0;

};

#endif // RICHRECTOOLS_IRICHGASQUARTZWINDOWABS_H
