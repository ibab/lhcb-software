// $Id: IMagneticFieldSvc.h,v 1.1 2005-12-08 15:19:23 cattanem Exp $
#ifndef KERNEL_IMAGNETICFIELDSVC_H
#define KERNEL_IMAGNETICFIELDSVC_H

// Include files
#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"

// from LHCbDefinitions
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"


// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IMagneticFieldSvc("IMagneticFieldSvc", 2 , 0); 


/** @class IMagneticFieldSvc IMagneticFieldSvc.h GaudiKernel/IMagneticFieldSvc.h

    The interface to the MagneticFieldSvc

    @author Iain Last
*/
class IMagneticFieldSvc : virtual public IService   {

public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMagneticFieldSvc; }

  // Get the magnetic field vector at a given point in space.
  // Input: Point3D - Point at which magnetic field vector is to be given.
  // Output: Vector3D - Magnectic field vector.
  // Return: StatusCode SUCCESS if calculation was performed.
  virtual StatusCode fieldVector( const Gaudi::XYZPoint&  xyz, 
                                        Gaudi::XYZVector& fvec ) const = 0;

};

#endif  // KERNEL_IMAGNETICFIELDSVC_H
