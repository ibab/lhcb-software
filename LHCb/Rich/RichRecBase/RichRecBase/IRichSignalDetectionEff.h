
//-----------------------------------------------------------------------------
/** @file IRichSignalDetectionEff.h
 *
 *  Header file for RICH reconstruction tool interface : IRichSignalDetectionEff
 *
 *  CVS Log :-
 *  $Id: IRichSignalDetectionEff.h,v 1.4 2004-07-26 18:00:58 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_IRICHSIGNALDETECTIONEFF_H
#define RICHRECBASE_IRICHSIGNALDETECTIONEFF_H 1

// Event model
class RichRecSegment;

/// Static Interface Identification
static const InterfaceID IID_IRichSignalDetectionEff( "IRichSignalDetectionEff", 1, 0 );

/** @class IRichSignalDetectionEff IRichSignalDetectionEff.h
 *
 *  Interface for tools providing calculations for the photon
 *  detection efficiencies.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class IRichSignalDetectionEff : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() {return IID_IRichSignalDetectionEff;}

  /** Computes the detection efficiency for average cherenkov photon of a given energy,
   *  that are emitted from a given RichRecSegment.
   *
   *  @param segment  The RichRecSegment for which the efficiency should be computed
   *  @param  energy  The photon energy 
   *
   *  @return The average detection efficiency for photons of the given energy
   */
  virtual double photonDetEfficiency( RichRecSegment * segment,
                                      const double energy ) const = 0;

};

#endif // RICHRECBASE_IRICHSIGNALDETECTIONEFF_H
