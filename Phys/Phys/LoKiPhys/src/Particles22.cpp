// $Id: Particles22.cpp,v 1.1 2008-09-29 16:50:40 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Child.h"
#include "LoKi/Particles1.h"
#include "LoKi/Vertices1.h"
#include "LoKi/Particles22.h"
// ============================================================================
/** @file
 *  Implementation file for functions from the file LoKi/Particles22.h
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl 
 *  @date 2008-09-29 
 */
// ============================================================================
/// anonymous namespace to hide some technicalities
namespace 
{
  // ==========================================================================
  /// NULL VERTEX 
  const LHCb::VertexBase * const s_VERTEX = 0 ;
  // ==========================================================================
}
// ============================================================================
// constructor form the daughter indices 
// ============================================================================
LoKi::Particles::DaughterVertexDistance::DaughterVertexDistance  
( const unsigned int d1 , 
  const unsigned int d2 )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_d1   ( d1 ) 
  , m_d2   ( d2 ) 
  , m_vfun ( s_VERTEX ) 
{}
// ============================================================================
// the only essential method 
// ============================================================================
LoKi::Particles::DaughterVertexDistance::result_type 
LoKi::Particles::DaughterVertexDistance::operator() 
  ( LoKi::Particles::DaughterVertexDistance::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error (" LHCb::Particle* points to NULL, return 'Invalid Distance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the corresponding "second" daughters 
  const LHCb::Particle* d2 = LoKi::Child::child ( p , m_d2 ) ;
  if ( 0 == d2 ) 
  {
    Error (" The second 'daugher' is invalid , return 'Invalid Distance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the corresponding "first" daughters 
  const LHCb::Particle* d1 = LoKi::Child::child ( p , m_d1 ) ;
  if ( 0 == d1 ) 
  {
    Error (" The first 'daugher' is invalid , return 'Invalid Distance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the corresponding vertex 
  const LHCb::VertexBase* v2 = d2 -> endVertex () ;
  if ( 0 == v2 ) 
  {
    Error (" The second 'vertex' is invalid , return 'Invalid Distance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the corresponding vertex 
  const LHCb::VertexBase* v1 = d1 -> endVertex () ;
  if ( 0 == v1 ) 
  {
    Error (" The second 'vertex' is invalid , return 'Invalid Distance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // reset the vertex in the functor 
  m_vfun.setVertex ( v2 ) ; 
  // use the functor 
  result_type result = m_vfun.distance ( v1 ) ;
  // reset the vertex in the functor 
  m_vfun.setVertex ( s_VERTEX ) ; 
  // 
  return result ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DaughterVertexDistance::fillStream ( std::ostream& s ) const
{
  s << "D2DVVD(" << m_d1 ;
  if ( 0 != m_d2 ) { s << "," << m_d2 ; }
  return s << ")" ;  
}
// ============================================================================
// constructor form the daughter indices 
// ============================================================================
LoKi::Particles::DaughterVertexSignedDistance::DaughterVertexSignedDistance  
( const unsigned int d1 , 
  const unsigned int d2 )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_d1   ( d1 ) 
  , m_d2   ( d2 ) 
  , m_vfun ( s_VERTEX ) 
{}
// ============================================================================
// the only essential method 
// ============================================================================
LoKi::Particles::DaughterVertexSignedDistance::result_type 
LoKi::Particles::DaughterVertexSignedDistance::operator() 
  ( LoKi::Particles::DaughterVertexSignedDistance::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error (" LHCb::Particle* points to NULL, return 'Invalid Distance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the corresponding "second" daughters 
  const LHCb::Particle* d2 = LoKi::Child::child ( p , m_d2 ) ;
  if ( 0 == d2 ) 
  {
    Error (" The second 'daugher' is invalid , return 'Invalid Distance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the corresponding "first" daughters 
  const LHCb::Particle* d1 = LoKi::Child::child ( p , m_d1 ) ;
  if ( 0 == d1 ) 
  {
    Error (" The first 'daugher' is invalid , return 'Invalid Distance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the corresponding vertex 
  const LHCb::VertexBase* v2 = d2 -> endVertex () ;
  if ( 0 == v2 ) 
  {
    Error (" The second 'vertex' is invalid , return 'Invalid Distance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the corresponding vertex 
  const LHCb::VertexBase* v1 = d1 -> endVertex () ;
  if ( 0 == v1 ) 
  {
    Error (" The second 'vertex' is invalid , return 'Invalid Distance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // reset the vertex in the functor 
  m_vfun.setVertex ( v2 ) ; 
  // use the functor 
  result_type result = m_vfun.distance ( v1 ) ;
  // reset the vertex in the functor 
  m_vfun.setVertex ( s_VERTEX ) ; 
  // 
  return result ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DaughterVertexSignedDistance::fillStream ( std::ostream& s ) const
{
  s << "D2DVVDSIGN(" << m_d1 ;
  if ( 0 != m_d2 ) { s << "," << m_d2 ; }
  return s << ")" ;  
}
// ============================================================================
// constructor form the daughter indices 
// ============================================================================
LoKi::Particles::DaughterVertexDistanceChi2::DaughterVertexDistanceChi2  
( const unsigned int d1 , 
  const unsigned int d2 )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_d1   ( d1 ) 
  , m_d2   ( d2 ) 
  , m_vfun ( s_VERTEX ) 
{}
// ============================================================================
// the only essential method 
// ============================================================================
LoKi::Particles::DaughterVertexDistanceChi2::result_type 
LoKi::Particles::DaughterVertexDistanceChi2::operator() 
  ( LoKi::Particles::DaughterVertexDistanceChi2::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error (" LHCb::Particle* points to NULL, return 'Invalid Chi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // get the corresponding "second" daughters 
  const LHCb::Particle* d2 = LoKi::Child::child ( p , m_d2 ) ;
  if ( 0 == d2 ) 
  {
    Error (" The second 'daugher' is invalid , return 'Invalid Chi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // get the corresponding "first" daughters 
  const LHCb::Particle* d1 = LoKi::Child::child ( p , m_d1 ) ;
  if ( 0 == d1 ) 
  {
    Error (" The first 'daugher' is invalid , return 'Invalid Chi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // get the corresponding vertex 
  const LHCb::VertexBase* v2 = d2 -> endVertex () ;
  if ( 0 == v2 ) 
  {
    Error (" The second 'vertex' is invalid , return 'Invalid Chi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // get the corresponding vertex 
  const LHCb::VertexBase* v1 = d1 -> endVertex () ;
  if ( 0 == v1 ) 
  {
    Error (" The second 'vertex' is invalid , return 'Invalid Chi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // reset the vertex in the functor 
  m_vfun.setVertex ( v2 ) ; 
  // use the functor 
  result_type result = m_vfun.chi2 ( v1 ) ;
  // reset the vertex in the functor 
  m_vfun.setVertex ( s_VERTEX ) ; 
  // 
  return result ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DaughterVertexDistanceChi2::fillStream ( std::ostream& s ) const
{
  s << "D2DVVDCHI2(" << m_d1 ;
  if ( 0 != m_d2 ) { s << "," << m_d2 ; }
  return s << ")" ;  
}
// ============================================================================
// constructor form the daughter indices 
// ============================================================================
LoKi::Particles::DaughterVertexDistanceDot::DaughterVertexDistanceDot
( const unsigned int d1 , 
  const unsigned int d2 )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_d1   ( d1 ) 
  , m_d2   ( d2 ) 
  , m_vfun ( s_VERTEX ) 
{}
// ============================================================================
// the only essential method 
// ============================================================================
LoKi::Particles::DaughterVertexDistanceDot::result_type 
LoKi::Particles::DaughterVertexDistanceDot::operator() 
  ( LoKi::Particles::DaughterVertexDistanceDot::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error (" LHCb::Particle* points to NULL, return 'Invalid Distance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the corresponding "second" daughters 
  const LHCb::Particle* d2 = LoKi::Child::child ( p , m_d2 ) ;
  if ( 0 == d2 ) 
  {
    Error (" The second 'daugher' is invalid , return 'Invalid Distance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the corresponding "first" daughters 
  const LHCb::Particle* d1 = LoKi::Child::child ( p , m_d1 ) ;
  if ( 0 == d1 ) 
  {
    Error (" The first 'daugher' is invalid , return 'Invalid Distance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // get the corresponding vertex 
  const LHCb::VertexBase* v2 = d2 -> endVertex () ;
  if ( 0 == v2 ) 
  {
    Error (" The second 'vertex' is invalid , return 'Invalid Distance'") ;
    return LoKi::Constants::InvalidDistance ;
  }
  // reset the vertex in the functor 
  m_vfun.setVertex ( v2 ) ; 
  // use the functor 
  result_type result = m_vfun.distance ( d1 ) ;
  // reset the vertex in the functor 
  m_vfun.setVertex ( s_VERTEX ) ; 
  // 
  return result ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DaughterVertexDistanceDot::fillStream ( std::ostream& s ) const
{
  s << "D2DVVDDOT(" << m_d1 ;
  if ( 0 != m_d2 ) { s << "," << m_d2 ; }
  return s << ")" ;  
}
// ============================================================================
// constructor form the daughter indices 
// ============================================================================
LoKi::Particles::DaughterVertexDistanceSignedChi2::DaughterVertexDistanceSignedChi2  
( const unsigned int d1 , 
  const unsigned int d2 )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function ()
  , m_d1   ( d1 ) 
  , m_d2   ( d2 ) 
  , m_vfun ( s_VERTEX ) 
{}
// ============================================================================
// the only essential method 
// ============================================================================
LoKi::Particles::DaughterVertexDistanceSignedChi2::result_type 
LoKi::Particles::DaughterVertexDistanceSignedChi2::operator() 
  ( LoKi::Particles::DaughterVertexDistanceSignedChi2::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error (" LHCb::Particle* points to NULL, return 'Invalid Chi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // get the corresponding "second" daughters 
  const LHCb::Particle* d2 = LoKi::Child::child ( p , m_d2 ) ;
  if ( 0 == d2 ) 
  {
    Error (" The second 'daugher' is invalid , return 'Invalid Chi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // get the corresponding "first" daughters 
  const LHCb::Particle* d1 = LoKi::Child::child ( p , m_d1 ) ;
  if ( 0 == d1 ) 
  {
    Error (" The first 'daugher' is invalid , return 'Invalid Chi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // get the corresponding vertex 
  const LHCb::VertexBase* v2 = d2 -> endVertex () ;
  if ( 0 == v2 ) 
  {
    Error (" The second 'vertex' is invalid , return 'Invalid Chi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // get the corresponding vertex 
  const LHCb::VertexBase* v1 = d1 -> endVertex () ;
  if ( 0 == v1 ) 
  {
    Error (" The second 'vertex' is invalid , return 'Invalid Chi2'") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // reset the vertex in the functor 
  m_vfun.setVertex ( v2 ) ; 
  // use the functor 
  result_type result = m_vfun.chi2 ( v1 ) ;
  // reset the vertex in the functor 
  m_vfun.setVertex ( s_VERTEX ) ; 
  // 
  // evaluate the sign:
  const int sign = 
    0 <= ( v1->position() - v2->position() ).Dot( d1->momentum().Vect() ) ? 1 : -1 ;
  //
  return result * sign ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DaughterVertexDistanceSignedChi2::fillStream ( std::ostream& s ) const
{
  s << "D2DVVDCHI2SIGN(" << m_d1 ;
  if ( 0 != m_d2 ) { s << "," << m_d2 ; }
  return s << ")" ;  
}

// ============================================================================
// The END 
// ============================================================================
