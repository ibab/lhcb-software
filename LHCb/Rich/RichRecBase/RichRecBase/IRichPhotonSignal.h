// $Id: IRichPhotonSignal.h,v 1.5 2004-02-02 14:23:02 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHPHOTONSIGNAL_H
#define RICHRECTOOLS_IRICHPHOTONSIGNAL_H 1

// Event
class RichRecPhoton;

/** @class IRichPhotonSignal IRichPhotonSignal.h
 *
 *  Interface to the tool to calculate for a given photon the probabilities of it 
 *  being a signal or scattered photon, and its predicted contribution
 *  to its associated RichRecPixel, under a certain mass hypothesis
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichPhotonSignal( "IRichPhotonSignal" , 1 , 0 );

class IRichPhotonSignal : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichPhotonSignal; }

  /// Predicted pixel signal for a given reconstructed photon under a given mass hypothesis
  virtual double predictedPixelSignal( RichRecPhoton * photon,
                                       const Rich::ParticleIDType id ) const = 0;

  /// Returns the value of the signal probablity function for the given photon at its cherenkov theta, for the given mass hypothesis
  virtual double signalProb( RichRecPhoton * photon,
                             const Rich::ParticleIDType id ) const = 0;

  /// Returns the value of the scatter probablity function for the given photon at its cherenkov theta, for the given mass hypothesis
  virtual double scatterProb( RichRecPhoton * photon,
                              const Rich::ParticleIDType id ) const = 0;

};

#endif // RICHRECTOOLS_IRICHPHOTONSIGNAL_H
