
//-----------------------------------------------------------------------------
/** @file IRichPhotonSignal.h
 *
 *  Header file for RICH reconstruction tool interface : IRichPhotonSignal
 *
 *  CVS Log :-
 *  $Id: IRichPhotonSignal.h,v 1.6 2004-07-26 18:00:57 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHPHOTONSIGNAL_H
#define RICHRECTOOLS_IRICHPHOTONSIGNAL_H 1

// Event
class RichRecPhoton;

/// Static Interface Identification
static const InterfaceID IID_IRichPhotonSignal( "IRichPhotonSignal" , 1 , 0 );

/** @class IRichPhotonSignal IRichPhotonSignal.h
 *
 *  Interface to the tool to calculate for a given photon the probabilities of it
 *  being a signal or scattered photon, and its predicted contribution
 *  to its associated RichRecPixel, under a certain mass hypothesis
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class IRichPhotonSignal : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichPhotonSignal; }

  /** Calculate the predicted pixel signal for a given reconstructed photon 
   *  under a given mass hypothesis
   *
   *  @param photon Pointer to the reconstructed photon
   *  @param id The mass hypothesis to assume
   *
   *  @return The predicted signal contribution to the associated RichRecPixel
   *          for the photon assuming the given mass hypothesis
   */
  virtual double predictedPixelSignal( RichRecPhoton * photon,
                                       const Rich::ParticleIDType id ) const = 0;

  /** Calculates the value of the signal probablity function for the given photon
   *  at its cherenkov theta, for the given mass hypothesis
   *
   *  @param photon Pointer to the reconstructed photon
   *  @param id The mass hypothesis to assume
   *
   *  @return The signal probability
   */
  virtual double signalProb( RichRecPhoton * photon,
                             const Rich::ParticleIDType id ) const = 0;

  /** Calculates the value of the scatter probablity function for the given photon 
   *  at its cherenkov theta, for the given mass hypothesis
   *
   *  @param photon Pointer to the reconstructed photon
   *  @param id The mass hypothesis to assume
   *
   *  @return The scattering probability
   */
  virtual double scatterProb( RichRecPhoton * photon,
                              const Rich::ParticleIDType id ) const = 0;

};

#endif // RICHRECTOOLS_IRICHPHOTONSIGNAL_H
