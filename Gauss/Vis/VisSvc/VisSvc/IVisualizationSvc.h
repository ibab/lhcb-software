// $ID:  $

#ifndef VISDESC_IVISUALIZATIONSVC_H
#define VISDESC_IVISUALIZATIONSVC_H

// Include files
#include "GaudiKernel/IInterface.h"

#include "VisSvc/VisAttribute.h"

// Forward and external declarations
class Material;
class ILVolume;

/// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IVisualizationSvc(948, 1 , 0);


/** @class IVisualizationSvc IVisualizationSvc.h VisDesc/IVisualizationSvc.h
 *
 *  this interface defines a Visualization service that is able to answer
 *  many question concerning the visualization of the detector. It
 *  is able to associate visualization attributes to materials and logical
 *  volumes. These describe the way these materials and volumes should be
 *  displayed
 *
 *  @author Sebastien Ponce
 */

class IVisualizationSvc : virtual public IInterface {

 public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IVisualizationSvc; }

  /**
   * This method returns the visualization attribute associated to a given
   * material or 0 if there is no attribute associated to it.
   * @param mat the material
   * @return the visualization attribute that should be used to display
   * this material
   */
  virtual const VisAttribute visAttribute (const Material* mat) const = 0;

  /**
   * This method returns the visualization attribute associated to a given
   * logical volume or 0 if there is no attribute associated to it.
   * @param vol the logical volume
   * @return the visualization attribute that should be used to display
   * this logical volume
   */
  virtual const VisAttribute visAttribute (const ILVolume* vol) const = 0;

  /**
   * this method erases the current set of attributes and loads a new set
   */
  virtual void reload () = 0;

};

#endif // VISDESC_IVISUALIZATIONSVC_H
