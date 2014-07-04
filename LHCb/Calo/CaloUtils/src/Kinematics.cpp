// $Id: $
// ============================================================================
// $URL$
// ============================================================================
// Include files 
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/MatrixUtils.h"
// ============================================================================
// Local
// ============================================================================
#include "CaloUtils/Kinematics.h"
// ============================================================================
/** @file 
 *  Implementation file for functions form namespace Calo::Kinematics 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08--29
 *
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace{
  // ==========================================================================
  /** evaluate the jacobians
   *  @param calo   (INPUT)  input CaloPosition
   *  @param origin (INPUT)  origin vertex
   *  @param F11    (OUTPUT) the jacobian: d(px,py,pz,E)/d(xc,yc,E)
   *  @param F12    (OUTPUT) the jacobian: d(px,py,pz,E)/d(xv,yv,zv)
   *  @return 4-momentum estimate 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-08--29
   */
  Gaudi::LorentzVector  _jacobians_( const LHCb::CaloPosition& calo   , 
                                     const Gaudi::XYZPoint&    origin ,  
                                     Gaudi::Matrix4x3*         F11    , 
                                     Gaudi::Matrix4x3*         F12    ){
    //
    const double dx        = calo.x() - origin.x() ;
    const double dy        = calo.y() - origin.y() ;
    const double dz        = calo.z() - origin.z() ;
    const double sx        =  dx / dz   ;
    const double sy        =  dy / dz   ;
    const double sz        =  1.0       ;
    const double sx2       =  sx * sx   ;
    const double sy2       =  sy * sy   ;
    const double sz2       =  1.0       ;
    const double gamma     =  1.0  / sqrt ( sx2 + sy2 + sz2 ) ;    
    const double gz        = gamma / dz ;
    const double gz2       = gz * gz    ;
    const double gz3       = gz * gz2   ; 
    //
    const double e         = calo.e() ;
    
    // various derivatives for jacobian evaluation:
    
    const double dx_dxc  =  1.0 ; // d( dx ) / d( xc ) 
    const double dx_dyc  =  0.0 ; // d( dx ) / d( yc )
    
    const double dy_dxc  =  0.0 ; // d( dy ) / d( xc ) 
    const double dy_dyc  =  1.0 ; // d( dy ) / d( yc ) 
    
    const double dz_dxc  =  0.0 ; // d( dz ) / d( xc ) 
    const double dz_dyc  =  0.0 ; // d( dz ) / d( yc ) 
    
    const double dgz_dxc = -1.0 * dx * gz3 ; // d( gz ) / d( xc ) 
    const double dgz_dyc = -1.0 * dy * gz3 ; // d( gz ) / d( yc )

    // 
    // Jacobian-I:   d(px,py,pz,E)/d(xc,yc,E)
    // 
    if ( 0 != F11 )    {
      
      Gaudi::Matrix4x3& f11 = *F11 ;
      
      //   px =     e * dx * gz  
      
      //   d(px)/d(xc) 
      f11 ( 0 , 0 ) =  e * ( dx_dxc * gz + dx * dgz_dxc ) ;
      //   d(px)/d(yc) 
      f11 ( 0 , 1 ) =  e * ( dx_dyc * gz + dx * dgz_dyc ) ;
      //   d(px)/d(e) 
      f11 ( 0 , 2 ) = dx * gz ;
      
      //   py = e * dy * gz  
      
      //   d(py)/d(xc) 
      f11 ( 1 , 0 ) =  e * ( dy_dxc * gz + dy * dgz_dxc ) ;
      //   d(py)/d(yc) 
      f11 ( 1 , 1 ) =  e * ( dy_dyc * gz + dy * dgz_dyc ) ;
      //   d(py)/d(e) 
      f11 ( 1 , 2 ) = dy * gz;
      
      //   pz = e * dz * gz   
      
      //   d(pz)/d(xc) 
      f11 ( 2 , 0 ) =  e * ( dz_dxc * gz + dz * dgz_dxc ) ;
      //   d(pz)/d(yc) 
      f11 ( 2 , 1 ) =  e * ( dz_dyc * gz + dz * dgz_dyc ) ;
      //   d(pz)/d(e) 
      f11 ( 2 , 2 ) = dz * gz ;
      
      //   e (new) = e (old)          
      
      //   d(e)/d(xc) 
      f11 ( 3 , 0 ) =  0.0 ;
      //   d(e)/d(yc) 
      f11 ( 3 , 1 ) =  0.0 ;
      //   d(e)/d(e) 
      f11 ( 3 , 2 ) =  1.0 ;
      
    }
    
    // 
    // Jacobian-II:   d(px,py,pz,E)/d(xv,yv,zv) 
    // 
    
    if ( 0 != F12 ){
      
      Gaudi::Matrix4x3& f12 = *F12 ;
      
      const double dx_dxv  = -1.0 ; // d( dx ) / d( xv ) 
      const double dx_dyv  =  0.0 ; // d( dx ) / d( yv ) 
      const double dx_dzv  =  0.0 ; // d( dx ) / d( zv )
      
      const double dy_dxv  =  0.0 ; // d( dy ) / d( xv ) 
      const double dy_dyv  = -1.0 ; // d( dy ) / d( yv ) 
      const double dy_dzv  =  0.0 ; // d( dy ) / d( zv ) 
      
      const double dz_dxv  =  0.0 ; // d( dz ) / d( xv )
      const double dz_dyv  =  0.0 ; // d( dz ) / d( yv ) 
      const double dz_dzv  = -1.0 ; // d( dz ) / d( zv ) 
      
      const double dgz_dxv = dx * gz3 ; // d( gz ) / d( xv ) 
      const double dgz_dyv = dy * gz3 ; // d( gz ) / d( yv ) 
      const double dgz_dzv = dz * gz3 ; // d( gz ) / d( zv ) 
      
      //   d(px)/d(xv) 
      f12 ( 0 , 0 ) =  e * ( dx_dxv * gz + dx * dgz_dxv ) ;
      //   d(px)/d(yv) 
      f12 ( 0 , 1 ) =  e * ( dx_dyv * gz + dx * dgz_dyv ) ;
      //   d(px)/d(zv) 
      f12 ( 0 , 2 ) =  e * ( dx_dzv * gz + dx * dgz_dzv ) ;
      
      //   d(py)/d(xv) 
      f12 ( 1 , 0 ) =  e * ( dy_dxv * gz + dy * dgz_dxv ) ;
      //   d(py)/d(yv) 
      f12 ( 1 , 1 ) =  e * ( dy_dyv * gz + dy * dgz_dyv ) ;
      //   d(py)/d(zv) 
      f12 ( 1 , 2 ) =  e * ( dy_dzv * gz + dy * dgz_dzv )  ;
      
      //   d(pz)/d(xv) 
      f12 ( 2 , 0 ) =  e * ( dz_dxv * gz + dz * dgz_dxv ) ;
      //   d(pz)/d(yv) 
      f12 ( 2 , 1 ) =  e * ( dz_dyv * gz + dz * dgz_dyv ) ;
      //   d(pz)/d(zv) 
      f12 ( 2 , 2 ) =  e * ( dz_dzv * gz + dz * dgz_dzv ) ;
      
      //   d(e)/d(xv) 
      f12 ( 3 , 0 ) =  0.0 ;
      //   d(e)/d(yv) 
      f12 ( 3 , 1 ) =  0.0 ;
      //   d(e)/d(zv) 
      f12 ( 3 , 2 ) =  0.0 ;
    }
    
    // 4-momentum:
    return  Gaudi::LorentzVector( e * gamma * sx ,
                                  e * gamma * sy ,
                                  e * gamma * sz ,
                                  e              ) ; 
  }
  // ==========================================================================
} //                                                end of anonymous namespace 
// ============================================================================
/*  evaluate 4-momentum of photon hypothesis 
 *  for the given calo-position object
 *  @param calo    (INPUT) calo-position object 
 *  @param origin  (INPUT) the assumed origin point 
 *  @return the estimate for the photon 4-momentum 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08--29
 */
// ============================================================================
Gaudi::LorentzVector Calo::Kinematics::momentum( const LHCb::CaloPosition& calo   , 
                                                 const Gaudi::XYZPoint&    origin ){
  return _jacobians_ ( calo , origin , 0 , 0 ) ;
}
// ============================================================================
/*  evaluate 4-momentum of photon hythothesis 
 *  for the given calo-position object
 *  @param calo    (INPUT)  calo-position object 
 *  @param origin  (INPUT)  the assumed origin point 
 *  @param output  (OUTPUT) the estimate for the photon 4-momentum 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08--29
 */
// ============================================================================
void Calo::Kinematics::momentum( const LHCb::CaloPosition& calo   , 
                                 const Gaudi::XYZPoint&    origin , 
                                 Gaudi::LorentzVector&     output ){ 
  output = _jacobians_ ( calo , origin , 0 , 0 ) ; 
}
// ============================================================================
/*  evaluate 4-momentum of photon hypothesis 
 *  for the given calo-position object
 *  @param calo    (INPUT)  calo-position object 
 *  @param origin  (INPUT)  the assumed origin point 
 *  @param output  (OUTPUT) the estimate for the photon 4-momentum 
 *  @param matrix  (OUTPUT) the estimate for the photon covariance matrix 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08--29
 */
// ============================================================================
void Calo::Kinematics::momentum( const LHCb::CaloPosition& calo   , 
                                 const Gaudi::XYZPoint&    origin , 
                                 Gaudi::LorentzVector&     output , 
                                 Gaudi::SymMatrix4x4&      matrix ){
  //
  // covariance matrix for (xc,yx,E):
  //
  const Gaudi::SymMatrix3x3& S11 = calo.covariance() ;
  
  // 
  // Jacobian:   d(px,py,pz,E)/d(xc,yc,E)
  // 
  Gaudi::Matrix4x3 F11  ;
  
  //
  // fill the output: 
  //
  
  // evaluate the  momentum 
  output = _jacobians_ ( calo , origin , &F11 , 0 ) ;
  
  // evaluate the covariance matrix matrix:
  matrix = Similarity  ( F11 , S11 ) ;
  //
}
// ============================================================================
/*  evaluate 4-momentum of photon hypothesis 
 *  for the given calo-position object
 *  @param calo    (INPUT)  calo-position object 
 *  @param origin  (INPUT)  the assumed origin point 
 *  @param output  (OUTPUT) the estimate for the photon 4-momentum 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08--29
 */
// ============================================================================
void Calo::Kinematics::momentum( const LHCb::CaloPosition&            calo   , 
                                 const Gaudi::XYZPoint&               origin , 
                                 Gaudi::Math::LorentzVectorWithError& output ){
  momentum ( calo   , 
             origin , 
             output , 
             output ) ; 
}
// ============================================================================
/** add photon 4-momentum for the given calo-position object
 *  @param calo    (INPUT)  calo-position object 
 *  @param origin  (INPUT)  the assumed origin point 
 *  @param output  (UPDATE) the estimate for 4-momentum
 *  @param matrix  (UPDATE) the estimate for the covariance matrix 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08--29
 */
// ============================================================================
void Calo::Kinematics::add2momentum 
( const LHCb::CaloPosition& calo   , 
  const Gaudi::XYZPoint&    origin , 
  Gaudi::LorentzVector&     output , 
  Gaudi::SymMatrix4x4&      matrix ) 
{
  //
  // covariance matrix for (xc,yx,E):
  //
  const Gaudi::SymMatrix3x3& S11 = calo.covariance() ;
  
  // 
  // Jacobian:   d(px,py,pz,E)/d(xc,yc,E)
  // 
  Gaudi::Matrix4x3 F11  ;

  //
  // fill the output: 
  //
  
  // update momentum 
  output += _jacobians_ ( calo , origin , &F11 , 0 ) ;
  
  // update covariance matrix matrix:
  matrix += Similarity ( F11 , S11 ) ;
  //
}
// ============================================================================
/*  add photon 4-momentum for the given calo-position object
 *  for the given calo-position object
 *  @param calo    (INPUT)  calo-position object 
 *  @param origin  (INPUT)  the assumed origin point 
 *  @param output  (OUTPUT) the estimate for the 4-momentum 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08--29
 */
// ============================================================================
void Calo::Kinematics::add2momentum 
( const LHCb::CaloPosition&            calo   , 
  const Gaudi::XYZPoint&               origin , 
  Gaudi::Math::LorentzVectorWithError& output ) 
{
  add2momentum ( calo   , 
                 origin ,
                 output , 
                 output ) ;
}
// ============================================================================
/*  get the jacobian: d(px,py,pz,E)/d(xc,yc,E)
 *  @param calo    (INPUT)  calo-position object 
 *  @param origin  (INPUT)  the assumed origin point 
 *  @param F11     (OUTPUT) the jacobiand (px,py,pz,E)/d(xc,yc,E)
 *  @return 4-momentum estimate 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08--29
 */        
// ============================================================================
Gaudi::LorentzVector Calo::Kinematics::jacobian1 
( const LHCb::CaloPosition& calo     , 
  const Gaudi::XYZPoint&    origin   , 
  Gaudi::Matrix4x3&         F11      ) 
{
  return _jacobians_ ( calo , origin , &F11 , 0 ) ;
}
// ============================================================================
/*  get the jacobian: d(px,py,pz,E)/d(xv,yv,zv)
 *  @param calo    (INPUT)  calo-position object 
 *  @param origin  (INPUT)  the assumed origin point 
 *  @param F11     (OUTPUT) the jacobian d(px,py,pz,E)/d(xv,yv,zv)
 *  @return 4-momentum estimate 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08--29
 */        
// ============================================================================
Gaudi::LorentzVector Calo::Kinematics::jacobian2
( const LHCb::CaloPosition& calo     , 
  const Gaudi::XYZPoint&    origin   , 
  Gaudi::Matrix4x3&         F12      ) 
{
  return _jacobians_ ( calo , origin , 0 , &F12 ) ;
}
// ============================================================================
/*  get the jacobians: d(px,py,pz,E)/d(xc,yc,E) and d(px,py,pz,E)/d(xv,yv,zv)
 *  @param calo     (INPUT)  calo-position object 
 *  @param origin   (INPUT)  the assumed origin point 
 *  @param F11      (OUTPUT) the jacobian d(px,py,pz,E)/d(xc,yc,E)
 *  @param F12      (OUTPUT) the jacobian d(px,py,pz,E)/d(xv,yv,zv)
 *  @return 4-momentum estimate 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08--29
 */        
// ============================================================================
Gaudi::LorentzVector Calo::Kinematics::jacobians
( const LHCb::CaloPosition& calo     , 
  const Gaudi::XYZPoint&    origin   , 
  Gaudi::Matrix4x3&         F11      ,
  Gaudi::Matrix4x3&         F12      ) 
{
  return _jacobians_ ( calo , origin , &F11 , &F12 ) ;
}
// ========================================================================
