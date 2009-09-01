// $Id: ILHCbMagnetSvc.h,v 1.2 2009-09-01 15:13:58 wouter Exp $
#ifndef KERNEL_ILHCBMAGNETSVC_H 
#define KERNEL_ILHCBMAGNETSVC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

namespace LHCb {
  class MagneticFieldGrid ;
}

static const InterfaceID IID_ILHCbMagnetSvc ( "ILHCbMagnetSvc", 1, 0 );

/** @class ILHCbMagnetSvc ILHCbMagnetSvc.h Kernel/ILHCbMagnetSvc.h
 *  LHCb extensions to the Gaudi IMagneticFieldSvc interface
 *
 *  @author Marco Cattaneo
 *  @date   2008-07-18
 */
class ILHCbMagnetSvc : virtual public IMagneticFieldSvc {

public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ILHCbMagnetSvc; }

  virtual double scaleFactor() const = 0; ///< Return the field scaling factor
  virtual bool   useRealMap()  const = 0; ///< True if using measured map


  /** Implementation of IMagneticFieldSvc interface.
   * @param[in]  xyz Point at which magnetic field vector will be given
   * @param[out] fvec Magnectic field vector.
   * @return StatusCode SUCCESS if calculation was performed.
   */
  virtual StatusCode fieldVector( const ROOT::Math::XYZPoint& xyz,
                                  ROOT::Math::XYZVector& fvec ) const = 0;

  /** Implementation of IMagneticFieldSvc interface.
   * @param[in]  xyz Point at which magnetic field vector will be given
   * @return fvec Magnectic field vector.
   */
  virtual ROOT::Math::XYZVector fieldVector( const ROOT::Math::XYZPoint& xyz ) const = 0;

  /// Get direct access to the field grid
  virtual const LHCb::MagneticFieldGrid* fieldGrid() const = 0 ;

protected:

private:

};
#endif // KERNEL_ILHCBMAGNETSVC_H
