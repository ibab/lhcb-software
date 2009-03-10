// $Id: Legacy1.cpp,v 1.2 2009-03-10 22:49:57 spradlin Exp $
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
  /// "invalid" vertex
  const LHCb::VertexBase* const s_VERTEX = 0 ;

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


// ============================================================================
// constructor from the vertex  
// ============================================================================
LoKi::Legacy::TrgPointingScoreWithPt::TrgPointingScoreWithPt
( const LHCb::VertexBase* vertex ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( vertex ) 
{}
// ============================================================================
// constructor from the point 
// ============================================================================
LoKi::Legacy::TrgPointingScoreWithPt::TrgPointingScoreWithPt
( const LoKi::Point3D& point ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( point ) 
{}
// ============================================================================
// constructor from the holder 
// ============================================================================
LoKi::Legacy::TrgPointingScoreWithPt::TrgPointingScoreWithPt
( const LoKi::Vertices::VertexHolder& holder ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , LoKi::Vertices::VertexHolder ( holder ) 
{}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::Legacy::TrgPointingScoreWithPt::TrgPointingScoreWithPt
( const LoKi::Legacy::TrgPointingScoreWithPt& right ) 
  : LoKi::AuxFunBase                      ( right )
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ( right )
  , LoKi::Vertices::VertexHolder          ( right ) 
{}
// ============================================================================
LoKi::Legacy::TrgPointingScoreWithPt::result_type 
LoKi::Legacy::TrgPointingScoreWithPt::pointing
( LoKi::Legacy::TrgPointingScoreWithPt::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000");
    return -1000 ;
  }
  const LHCb::Vertex* v = p->endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::Particle::endVertex points to NULL, return -2000");
    return -2000 ;
  }
  if ( !valid() ) 
  {
    Error ( "VertexHolder base is invalid, return -3000");
    return -3000 ;
  }

  LHCb::Particle::ConstVector vdaugs = v->outgoingParticlesVector();
  if( vdaugs.empty() )
  {
    Warning ("Particle endVertex has no outgoing particles, return 1.0") ;
    return 1.0 ;
  }

  const LoKi::ThreeVector mom = p->momentum().Vect() ;
  const LoKi::ThreeVector disp = v->position() - position() ;
  const LoKi::ThreeVector cross = mom.Cross(disp) ;
  const double dispmag = disp.Mag2() ;
  if ( 0 >= dispmag ) 
  {
    Warning ( "Vector of illegal length, return -4000") ;
    return -4000 ;
  }

  const double pstheta = ::sqrt(cross.Mag2() / dispmag) ;

  double ptsum = 0.0;
  LHCb::Particle::ConstVector::const_iterator daug ;
  for( daug = vdaugs.begin(); daug != vdaugs.end(); ++daug )
  {
    if ( 0 == (*daug) )
    {
      // Perhaps this condition should raise an exception.  It should never
      //   occur, and if it does occur then it indicates a bug.
      Error ("Daughter LHCb::Particle* points to NULL, return -1000");
      return -1000 ;
    }

    ptsum += (*daug)->pt();
  }

  const double denom = pstheta + ptsum ;
  if ( 0 >= denom ) 
  {
    Warning ( "Invalid divisor, return 1") ;
    return 1 ;
  }

  return pstheta / denom ;
} 
// ============================================================================

// ============================================================================
// the default constructor: create the object in invalid state
// ============================================================================
LoKi::Legacy::TrgPointingScoreWithPtWithBestPV::
TrgPointingScoreWithPtWithBestPV()
  : LoKi::AuxDesktopBase()
  , LoKi::Legacy::TrgPointingScoreWithPt ( s_VERTEX ) 
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Legacy::TrgPointingScoreWithPtWithBestPV* 
LoKi::Legacy::TrgPointingScoreWithPtWithBestPV::clone() const 
{ return new TrgPointingScoreWithPtWithBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Legacy::TrgPointingScoreWithPtWithBestPV::result_type 
LoKi::Legacy::TrgPointingScoreWithPtWithBestPV::operator() 
  ( LoKi::Legacy::TrgPointingScoreWithPtWithBestPV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, retuen -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the best vertex from desktop and use it 
  setVertex ( desktop() -> relatedVertex ( p ) ) ;
  //
  return pointing ( p ) ;
}
// ============================================================================

// =============================================================================
// The END
// =============================================================================
