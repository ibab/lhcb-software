// $Id: IRichSignalDetectionEff.h,v 1.1 2003-08-06 09:55:31 jonrob Exp $
#ifndef RICHRECTOOLS_IRICHSIGNALDETECTIONEFF_H
#define RICHRECTOOLS_IRICHSIGNALDETECTIONEFF_H 1

/** @class IRichSignalDetectionEff IRichSignalDetectionEff.h
 *
 *  Interface for tool providing calculations for the photon detection efficiencies
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichSignalDetectionEff( "IRichSignalDetectionEff", 1, 0 );

class IRichSignalDetectionEff : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() {return IID_IRichSignalDetectionEff;}

  /// Returns the detection efficiency for a cherenkov photon of a given energy
  virtual double photonDetEfficiency( double energy ) = 0;
  
};

#endif // RICHRECTOOLS_IRICHSIGNALDETECTIONEFF_H
