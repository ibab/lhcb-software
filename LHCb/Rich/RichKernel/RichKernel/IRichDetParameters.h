/** @file IRichDetParameters.h
 *
 *  Header file for tool interface : IRichDetParameters
 *
 *  CVS Log :-
 *  $Id: IRichDetParameters.h,v 1.5 2004-07-29 09:30:32 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.4  2004/07/26 17:53:16  jonrob
 *  Various improvements to the doxygen comments
 *
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */

#ifndef RICHKERNEL_IRICHDETPARAMETERS_H
#define RICHKERNEL_IRICHDETPARAMETERS_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// LHCbKernel
#include "Kernel/RichRadiatorType.h"

/// Static Interface Identification
static const InterfaceID IID_IRichDetParameters( "IRichDetParameters", 1, 0 );

/** @class IRichDetParameters IRichDetParameters.h RichKernel/IRichDetParameters.h
 *
 *  Interface for tools providing access to useful detector parameters
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */

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
