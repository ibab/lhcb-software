// $Id: IRichDetParameters.h,v 1.3 2004-07-15 15:36:53 jonrob Exp $
#ifndef RICHKERNEL_IRICHDETPARAMETERS_H
#define RICHKERNEL_IRICHDETPARAMETERS_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// LHCbKernel
#include "Kernel/RichRadiatorType.h"

/** @class IRichDetParameters IRichDetParameters.h RichDetTools/IRichDetParameters.h
 *
 *  Interface for tools providing access to useful detector parameters
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */

static const InterfaceID IID_IRichDetParameters( "IRichDetParameters", 1, 0 );

class IRichDetParameters : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichDetParameters; }

  /** Calculates the maximum observable photon energy for a given radiator medium
   *
   *  @param rad  The radiator type
   *
   *  @return The value of the maximum photon energy for the given radiator
   */
  virtual double maxPhotonEnergy ( const Rich::RadiatorType rad ) const = 0;

  /** Calculates the minimum observable photon energy for a given radiator medium
   *
   *  @param rad  The radiator type
   *
   *  @return The value of the minimum photon energy for the given radiator
   */
  virtual double minPhotonEnergy ( const Rich::RadiatorType rad ) const = 0;
  
  /** Calculate the mean observable photon energy for a given radiator medium
   *
   *  @param rad  The radiator type
   *
   * @return The value of the mean photon energy for the given radiator
   */
  virtual double meanPhotonEnergy ( const Rich::RadiatorType rad ) const = 0;
  
};

#endif // RICHKERNEL_IRICHDETPARAMETERS_H
