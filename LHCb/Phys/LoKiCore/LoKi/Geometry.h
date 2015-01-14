// ============================================================================
#ifndef LOKI_GEOMETRY_H 
#define LOKI_GEOMETRY_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// Math definitions
// ============================================================================
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Kinematics.h"
// ============================================================================
/** @file
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
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::Geometry LoKi/Geometry.h
   *  Namespace with the collection of useful "geoemtry" functions   
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-17
   */
  namespace Geometry 
  {
    // ========================================================================
    /** The trivial function, which evaluated the distance 
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
    double distance 
    ( const LoKi::Point3D&  point , 
      const LoKi::Point3D&  p0    , 
      const LoKi::Vector3D& v0    ) ;
    // ========================================================================    
    /** The trivial function, which evaluates the distance 
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
    double distance 
    ( const LoKi::Point3D&  p1 , 
      const LoKi::Vector3D& v1 ,
      const LoKi::Point3D&  p2 , 
      const LoKi::Vector3D& v2 ) ;
    // ========================================================================    
    /** The trivial function, 
     *  which evaluates the distance between two points 
     *  in "chi2" units 
     *  
     *   chi2 = [(p1-p2)^T][(cov1+cov2)^-1][(p1-p2)]
     * 
     *  @param p1   the first  point 
     *  @param cov1 the first  covariance matrix 
     *  @param p2   the second point 
     *  @param cov2 the second covariance matrix 
     *  @param value the chi2 value (to be updated)
     *  @return status code 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-17
     */
    StatusCode chi2 
    ( const LoKi::Point3D&       p1    , 
      const Gaudi::SymMatrix3x3& cov1  ,
      const LoKi::Point3D&       p2    , 
      const Gaudi::SymMatrix3x3& cov2  , 
      double&                    value ) ;
    // ========================================================================
    /** The trivial function, 
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
    StatusCode chi2 
    ( const LoKi::Point3D&       p1    , 
      const LoKi::Point3D&       p2    , 
      const Gaudi::SymMatrix3x3& c     , 
      double&                    value ) ;
    // ========================================================================
  } // end of the namespace LoKi::Geometry 
  // ==========================================================================
} // end of the namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GEOMETRY_H
// ============================================================================
