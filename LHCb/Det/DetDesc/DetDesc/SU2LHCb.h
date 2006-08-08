// $Id: SU2LHCb.h,v 1.1 2006-08-08 12:48:29 jpalac Exp $
#ifndef V16R7_SU2LHCB_H 
#define V16R7_SU2LHCB_H 1

// Include files
#include "Kernel/Transform3DTypes.h"
#include "Kernel/Point3DTypes.h"
#include "GaudiKernel/PhysicalConstants.h"
/** @namespace DetDesc SU2LHCb.h v16r7/SU2LHCb.h
 *  
 *  Methods for transformations from LHCb cavern surveying frame of
 *  reference to LHCb frame. Accounts for X <-> Y axis shift and tilt wrt.
 *  beam. 
 *
 *  @author Juan PALACIOS
 *  @date   2006-08-08
 *
 *
 */
namespace DetDesc {

  /** Get the SU -> LHCb 3D transformation
   *  
   *  @code
   *  Gaudi::Transform3D su2lhcb = DetDesc::SU2LHCb();
   *  @endcode
   *  @return SU -> LHCb 3D transformation
   *  @author Juan PALACIOS
   *  @date   2006-08-08
   *
   */
  const Gaudi::Transform3D SU2LHCb() 
  {
    return Gaudi::Transform3D( Gaudi::RotationY( -0.5*Gaudi::Units::pi) *
                               Gaudi::RotationZ( -0.5*Gaudi::Units::pi) *
                               Gaudi::RotationX( -3.601*Gaudi::Units::mrad) );
  }

  /** Helper to transform any type of MathCore point
   *  
   *  @code
   *  Gaudi::XYZPoint pSU( 0.0, 0.0, 9450);
   *  Gaudi::XYZPoint pLHCb = DetDesc::SU2LHCb( pSU );
   *  @endcode
   *  @param 3D point in SU reference frame
   *  @return 3D point transformed into LHCb frame
   *
   *  @author Juan PALACIOS
   *  @date   2006-08-08
   *
   */
  template <typename Point>
  const Point SU2LHCb(const Point& point) 
  {
    return SU2LHCb()*point;
  }

};
#endif // V16R7_SU2LHCB_H
