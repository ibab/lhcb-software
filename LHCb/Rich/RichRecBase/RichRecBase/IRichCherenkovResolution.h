// $Id: IRichCherenkovResolution.h,v 1.2 2004-02-02 14:23:01 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHCHERENKOVRESOLUTION_H
#define RICHRECTOOLS_IRICHCHERENKOVRESOLUTION_H 1

// Event model
class RichRecSegment;

/** @class IRichCherenkovResolution IRichCherenkovResolution.h
 *
 *  Interface for tool providing Cherenkova angle calulcations
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichCherenkovResolution( "IRichCherenkovResolution", 1, 0 );

class IRichCherenkovResolution : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() {return IID_IRichCherenkovResolution;}

  /// Cherenkov theta resolution for given particle hypothesis
  virtual double ckThetaResolution( RichRecSegment * segment,
                                    const Rich::ParticleIDType id = Rich::Pion ) const = 0;

};

#endif // RICHRECTOOLS_IRICHCHERENKOVRESOLUTION_H
