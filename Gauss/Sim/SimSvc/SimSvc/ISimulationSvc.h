// $ID:  $

#ifndef SIMSVC_ISIMULATIONSVC_H
#define SIMSVC_ISIMULATIONSVC_H

// Include files
#include "GaudiKernel/IInterface.h"

#include "SimSvc/SimAttribute.h"
#include "SimSvc/RegionCuts.h"
#include <map>

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

  // typedefs used  
  typedef std::map<int, const SimAttribute*> PartAttr;
  typedef std::vector<RegionCuts> VectOfRegCuts;

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ISimulationSvc; }

  /**
   * This method returns the simulation attribute associated to a given
   * logical volume.
   * @param vol the logical volume
   * @return the simulation attribute that should be used to simulate
   * this logical volume
   */
  virtual const PartAttr* simAttribute (const ILVolume* vol) const = 0;

  /**
   * This method returns the simulation attribute associated to a given
   * logical volume.
   * @param vol the logical volume
   * @return the simulation attribute that should be used to simulate
   * this logical volume
   */
  virtual const PartAttr* simAttribute (const std::string volname) const = 0;

  /**
   * This method tells whether a simulation attribute is associated to a given
   * logical volume or not
   * @param vol the logical volume
   */
  virtual bool hasSimAttribute (const ILVolume* vol) const = 0;

  /**
   * This method tells whether a simulation attribute is associated to a given
   * logical volume or not
   * @param vol the logical volume
   */
  virtual bool hasSimAttribute (const std::string volname) const = 0;

  /**
   * This method returns the pointer to the vector of region definitions.
   * @param 
   * @return std::vector<RegionCuts>
   */
  virtual const std::vector<RegionCuts>* regionsDefs () const = 0;

  /**
   * this method erases the current set of attributes and loads a new set
   */
  virtual void reload () = 0;

};

#endif // SIMSVC_ISIMULATIONSVC_H
