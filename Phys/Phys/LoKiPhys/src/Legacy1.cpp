// $Id: Legacy1.cpp,v 1.1 2008-06-03 14:07:02 ibelyaev Exp $
// =============================================================================
// Include files 
// =============================================================================
// STD& STL 
// =============================================================================
#include <cmath>
// =============================================================================
// LoKi
// =============================================================================
#include "LoKi/Geometry.h"
#include "LoKi/Legacy1.h"
// =============================================================================
/** @file 
 *  Implementation file for namespace LoKi::Legacy
 *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
 *  @date 2008-06-03
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /** evaluate (the incorrect) significance of vertex separation 
   *  as the ratio of the vertex distance over the distance error 
   */
  double vtxSeparation
  ( const LoKi::Point3D&       p1    , 
    const Gaudi::SymMatrix3x3& cov1  ,
    const LoKi::Point3D&       p2    , 
    const Gaudi::SymMatrix3x3& cov2  ) 
  {
    const Gaudi::XYZVector diff = p1 - p2;
    
    Gaudi::SymMatrix6x6 errMatrix;
    errMatrix.Place_at( cov1, 0, 0 );
    errMatrix.Place_at( cov2, 3, 3 );
    const Gaudi::XYZVector u = diff.Unit();
    const Gaudi::Vector6 derivDist( u.x(),  u.y(),  u.z(),
                                    -u.x(), -u.y(), -u.z() );
    
    const double error  = std::sqrt(std::fabs(ROOT::Math::Dot(derivDist,errMatrix*derivDist)));
    
    return (p1-p2).R() / error ;
  }
}
// ============================================================================
// MANDATORY: the only one essential method              
// ============================================================================
LoKi::Legacy::DistanceSignificanceWithBestPV::result_type 
LoKi::Legacy::DistanceSignificanceWithBestPV::operator() 
  ( LoKi::Legacy::DistanceSignificanceWithBestPV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // check the vertex
  const LHCb::VertexBase* sv = p->endVertex() ;
  if ( 0 == sv ) 
  {
    Error ( "EndVertex points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the best related vertex from the desktop and use it 
  const LHCb::VertexBase* pv = desktop()->relatedVertex ( p ) ;
  if ( 0 == pv ) 
  {
    Error ( "Related Vertex points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // make the actual evalaution 
  return vtxSeparation 
    ( sv -> position  () , 
      sv -> covMatrix () ,
      pv -> position  () , 
      pv -> covMatrix () ) ;
}
// =============================================================================
// The END
// =============================================================================
