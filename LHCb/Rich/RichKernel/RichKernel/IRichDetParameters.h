// $Id: IRichDetParameters.h,v 1.1 2004-06-17 12:00:45 cattanem Exp $
#ifndef RICHDETTOOLS_IRICHDETPARAMETERS_H
#define RICHDETTOOLS_IRICHDETPARAMETERS_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// LHCbKernel
#include "Kernel/RichRadiatorType.h"

/** @class IRichDetParameters IRichDetParameters.h RichDetTools/IRichDetParameters.h
 *
 * Interface to Tool to provide access to useful detector parameters
 *
 *  @author Chris Jones
 *  @date   2004-03-29
 */

static const InterfaceID IID_IRichDetParameters( "IRichDetParameters", 1, 0 );

class IRichDetParameters : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichDetParameters; }

  /// Returns the maximum photon energy for a given radiator
  virtual double maxPhotonEnergy( const Rich::RadiatorType rad ) const = 0;

  /// Returns the minimum photon energy for a given radiator
  virtual double minPhotonEnergy( const Rich::RadiatorType rad ) const = 0;

  /// Returns the mean photon energy for a given radiator
  virtual double meanPhotonEnergy( const Rich::RadiatorType rad ) const = 0;

};

#endif // RICHDETTOOLS_IRICHDETPARAMETERS_H
