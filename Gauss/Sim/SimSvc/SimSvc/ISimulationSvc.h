// $ID:  $

#ifndef SIMSVC_ISIMULATIONSVC_H
#define SIMSVC_ISIMULATIONSVC_H

// Include files
#include "GaudiKernel/IInterface.h"

#include "SimSvc/SimAttribute.h"

// Forward and external declarations
class ILVolume;

/// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_ISimulationSvc(948, 1 , 0);


/** @class ISimulationSvc ISimulationSvc.h SimDesc/ISimulationSvc.h
 *
 *  this interface defines a Simulation service that is able to answer
 *  many question concerning the simulation of the detector. It
 *  is able to associate simulation attributes to logical volumes.
 *  These describe the way these volumes should be simulated
 *
 *  @author Sebastien Ponce
 */

class ISimulationSvc : virtual public IInterface {

 public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ISimulationSvc; }

  /**
   * This method returns the simulation attribute associated to a given
   * logical volume or 0 if there is no attribute associated to it.
   * @param vol the logical volume
   * @return the simulation attribute that should be used to simulate
   * this logical volume
   */
  virtual const SimAttribute simAttribute (const ILVolume* vol) const = 0;

  /**
   * this method erases the current set of attributes and loads a new set
   */
  virtual void reload () = 0;

};

#endif // SIMSVC_ISIMULATIONSVC_H
