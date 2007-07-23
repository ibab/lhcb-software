// $Id: Geometry.cpp,v 1.7 2007-07-23 17:07:43 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Math Definitions
// ============================================================================
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Geometry.h"
#include "LoKi/Constants.h"
#include "LoKi/Report.h"
// ============================================================================
/** @file
 *
 *  Implementation file for functions form the namespace LoKi::Geometry
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-17 
 */
// ============================================================================
/*  The trivial function, which evaluated the distance 
 *  between the point and the line. 
 *
 *  The line is parameterized with the point 'p0' and the 
 *   direction vector 'v0' 
 *  
 *  @param point the point to be considered 
 *  @param p0    the point at the line 
 *  @param v0    the direction vector along the line 
 *  @return the distance between the point and the line
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-02-17
 */
// ============================================================================
double LoKi::Geometry::distance 
( const LoKi::Point3D&  point , 
  const LoKi::Point3D&  p0    , 
  const LoKi::Vector3D& v0    ) 
{
  // v0*v0 
  const double v02 = v0.Mag2() ;
  if ( 0 == v02 ) 
  {
    LoKi::Report::Error("distance: 'v0' is invalid! return 'InvalidDistance'");
    return LoKi::Constants::InvalidDistance ;
  }
  //   p0-p
  const LoKi::Vector3D delta = p0 - point  ;
  // distance vector 
  const LoKi::Vector3D dist  = delta - ( v0 * delta.Dot(v0) / v02 ) ;
  // length of the distance vector 
  return dist.R() ;
}
// ============================================================================
/*  The trivial function, which evaluates the distance 
 *  between two lines 
 *  Each line is parameterized with the point at the line 
 *  and the direction vectors
 *  
 *  @param p1  the point at the first line 
 *  @param v1  the direction vector along the first line 
 *  @param p2  the point at the second line 
 *  @param v2  the direction vector along the second line 
 *  @return the distance between the lines 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-02-17
 */
// ============================================================================
double LoKi::Geometry::distance 
( const LoKi::Point3D&  p1 , 
  const LoKi::Vector3D& v1 ,
  const LoKi::Point3D&  p2 , 
  const LoKi::Vector3D& v2 ) 
{
  const double v1_2 = v1.Mag2() ;
  if ( 0 == v1_2 ) { return distance ( p1 , p2 , v2 ) ; }   // RETURN 
  const double v2_2 = v2.Mag2() ;
  if ( 0 == v2_2 ) { return distance ( p2 , p1 , v1 ) ; }   // RETURN

  //
  // distance between 2 points at 2 different lines is 
  //
  //     dist = ( p1 + v1*t1 - p2 - v2*t2 ) 
  //
  // 't1' and 't2' are found from the conditions that vector 'dist' is 
  // orthogonal both to 'v1' and to 'v2' 
  //
  // The equations are:
  //
  //   0 = dist * v1 
  //   0 = dist * v2 
  //
  // use Kramer method for solution:
  //
  
  // 1) evaluate the determinant 
  
  const double v1v2 = v1.Dot(v2) ;
  // determinant of the system :
  const double D    = v1_2*v2_2 - v1v2*v1v2 ;
  
  // vectors are collinear, distance is constant one, take one point
  if ( 0 == D ) { return distance ( p1 , p2 , v2 ) ; }
  
  const LoKi::Vector3D delta  = p1 - p2 ;
  
  const double dv1 = delta.Dot(v1) ;
  const double dv2 = delta.Dot(v2) ;
  
  // 2) solve the system 
  
  const double t1  = (-dv1*v2_2+dv2*v1v2) / D ;
  const double t2  = ( dv2*v1_2-dv1*v1v2) / D ;
  
  // 3) construct the distance vector :
  
  LoKi::Vector3D dist = delta ;
  dist += v1 * t1 ;
  dist -= v2 * t2 ;
  
  return dist.R() ;
}
// ============================================================================
/*  The trivial function, 
 *  which evaluates the distance between two points 
 *  in "chi2" units 
 *  
 *  
 *   chi2 = [(p1-p2)^T][(cov1+cov2)^-1][(p1-p2)]
 * 
 *
 *  @param p1  the first  point 
 *  @param c1  the first  covariance matrix 
 *  @param p2  the second point 
 *  @param c2  the second covariance matrix 
 *  @param value the chi2 value (to be updated)
 *  @return status code 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-02-17
 */
// ============================================================================
StatusCode LoKi::Geometry::chi2 
( const LoKi::Point3D&       p1    , 
  const Gaudi::SymMatrix3x3& cov1  ,
  const LoKi::Point3D&       p2    , 
  const Gaudi::SymMatrix3x3& cov2  , 
  double&                    value ) 
{
  const Gaudi::Vector3 delta 
    ( p1.x() - p2.x() , 
      p1.y() - p2.y() , 
      p1.z() - p2.z() ) ;
  
  Gaudi::SymMatrix3x3 cov ( cov1 ) ; cov += cov2 ;
  if ( !cov.Invert() ) 
  {
    value = LoKi::Constants::HugeChi2 ;
    return LoKi::Report::Error ("Chi2(1): Error in Matrix inversion!");
  }
  
  // delta^T*cov*delta 
  //value = ROOT::Math::Product( delta , cov ) ;
  value = ROOT::Math::Similarity( delta , cov ) ;
  // 
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  The trivial function, 
 *  which evaluates the distance between two points 
 *  in "chi2" units 
 *  
 *   chi2 = [(p1-p2)^T][(cov)^-1][(p1-p2)]
 * 
 *  @param p1   the first  point 
 *  @param p2   the second point 
 *  @param c    the covariance matrix 
 *  @param value the chi2 value (to be updated)
 *  @return status code 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-02-17
 */
// ============================================================================
StatusCode LoKi::Geometry::chi2 
( const LoKi::Point3D&       p1    , 
  const LoKi::Point3D&       p2    , 
  const Gaudi::SymMatrix3x3& c     , 
  double&                    value ) 
{
  const Gaudi::Vector3 delta 
    ( p1.x() - p2.x() , 
      p1.y() - p2.y() , 
      p1.z() - p2.z() ) ;
  
  Gaudi::SymMatrix3x3 cov( c ) ;
  if ( !cov.Invert() ) 
  {
    value = LoKi::Constants::InvalidChi2 ;
    return LoKi::Report::Error ("Chi2(2): Error in Matrix inversion!");
  }
  
  // delta^T*cov*delta 
  // value = ROOT::Math::Product( delta , cov ) ;
  value = ROOT::Math::Similarity( delta , cov ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================



// ============================================================================
// The END 
// ============================================================================


