// $Id: SU2LHCb.h,v 1.4 2007-01-17 12:10:15 cattanem Exp $
#ifndef V16R7_SU2LHCB_H 
#define V16R7_SU2LHCB_H 1

// Include files
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
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

  /** Get the LHCb -> SU 3D transformation
   *  
   *  @code
   *  Gaudi::Transform3D lhcb2su = DetDesc::LHCb2SU();
   *  @endcode
   *  @return LHCb -> SU 3D transformation
   *  @author Juan PALACIOS
   *  @date   2006-08-08
   *
   */
  const Gaudi::Transform3D LHCb2SU() 
  {
    return Gaudi::Transform3D( Gaudi::RotationY( -0.5*Gaudi::Units::pi) *
                               Gaudi::RotationZ( -0.5*Gaudi::Units::pi) *
                               Gaudi::RotationX( -3.601*Gaudi::Units::mrad) );

                               
  }
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
    return Gaudi::Transform3D( Gaudi::RotationX( 3.601*Gaudi::Units::mrad) *
                               Gaudi::RotationZ( 0.5*Gaudi::Units::pi) *
                               Gaudi::RotationY( 0.5*Gaudi::Units::pi) );
  }

  /** Helper to transform any type of MathCore point
   *  
   *  @code
   *  Gaudi::XYZPoint pLHCb( 0.0, 0.0, 9450);
   *  Gaudi::XYZPoint pSU = DetDesc::LHCb2SU( pLHCb );
   *  @endcode
   *  @param 3D point in LCHb reference frame
   *  @return 3D point transformed into SU frame
   *
   *  @author Juan PALACIOS
   *  @date   2006-08-08
   *
   */
  template <typename Point>
  const Point LHCb2SU(const Point& point) 
  {
    return LHCb2SU()*point;
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
