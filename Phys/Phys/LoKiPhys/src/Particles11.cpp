// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Print.h"
#include "LoKi/Keeper.h"
#include "LoKi/UniqueKeeper.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles11.h"
#include "LoKi/PhysAlgs.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(disable:177) // ... was declared but never referenced
#endif
// ============================================================================
// Boost 
// ============================================================================
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace  LoKi::Particles
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
 *  @date 2006-02-22
 */
// ============================================================================
//  constructor from one particle 
// ============================================================================
LoKi::Particles::IsAParticle::IsAParticle 
( const LHCb::Particle*                 p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ( p ) 
{} 
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticle::IsAParticle 
( const LHCb::Particle::Container*                p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ()
{ if ( 0 != p ) { addObjects( p->begin() , p->end() ) ; } }
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticle::IsAParticle 
( const LHCb::Particle::ConstVector&    p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ( p.begin() , p.end() ) 
{} 
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticle::IsAParticle 
( const LoKi::PhysTypes::Range&         p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ( p.begin() , p.end() ) 
{} 
// ============================================================================
//  MANDATORY: the only one essential method
// ============================================================================
bool LoKi::Particles::IsAParticle::inList
( const LHCb::Particle* p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error    ( " Argument is invalid! return 'false' " );
    return false ;
  }
  //
  if ( empty() ) 
  {
    Warning  ( " Empty list of particles is specified! return 'false' " );
    return false ;
  }
  // look for the particle 
  return std::binary_search ( begin() , end() , p ) ;
} 
// ============================================================================
bool LoKi::Particles::IsAParticle::inTree
( const LHCb::Particle* p ) const 
{
  if ( 0 == p ) 
  {
    Error    ( " Argument is invalid! return 'false' " );
    return false ;
  }
  if ( empty() ) 
  {
    Warning  ( " Empty list of particles is specified! return 'false' " );
    return false ;
  }
  //
  using namespace boost::lambda ;
  return LoKi::PhysAlgs::found 
    ( p , bind ( &LoKi::Particles::IsAParticle::inList , this , _1 ) ) ;
}
// ============================================================================
std::ostream&
LoKi::Particles::IsAParticle::fillStream
( std::ostream& stream ) const
{ return stream << "IS" ; }

// ============================================================================
//  constructor from one particle 
// ============================================================================
LoKi::Particles::IsAParticleInTree::IsAParticleInTree 
( const LHCb::Particle*                 p )
  : LoKi::Particles::IsAParticle ( p ) 
{}  
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticleInTree::IsAParticleInTree
( const LHCb::Particle::Container*   p ) 
  : LoKi::Particles::IsAParticle ( p ) 
{}  
// ============================================================================
//  constructor from vector of particles 
// ============================================================================
LoKi::Particles::IsAParticleInTree::IsAParticleInTree
( const LHCb::Particle::ConstVector&    p ) 
  : LoKi::Particles::IsAParticle ( p ) 
{}  
// ============================================================================
//  constructor from container of particle 
// ============================================================================
LoKi::Particles::IsAParticleInTree::IsAParticleInTree
( const LoKi::PhysTypes::Range&         p ) 
  : LoKi::Particles::IsAParticle ( p ) 
{}  
// ============================================================================
//  MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::IsAParticleInTree::result_type 
LoKi::Particles::IsAParticleInTree::operator() 
  ( LoKi::Particles::IsAParticleInTree::argument p ) const
{
  return inTree ( p ) ;
} 
// ============================================================================
std::ostream&
LoKi::Particles::IsAParticleInTree::fillStream
( std::ostream& stream ) const
{ return stream << "ISINTREE" ; }


// ============================================================================
//  constructor from one particle 
// ============================================================================
LoKi::Particles::IsAParticleFromTree::IsAParticleFromTree 
( const LHCb::Particle*                 p ) 
  : LoKi::Particles::IsAParticleInTree ( p ) 
{}
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticleFromTree::IsAParticleFromTree 
( const LHCb::Particle::Container*      p ) 
  : LoKi::Particles::IsAParticleInTree ( p ) 
{}
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticleFromTree::IsAParticleFromTree
( const LHCb::Particle::ConstVector&    p ) 
  : LoKi::Particles::IsAParticleInTree ( p ) 
{}
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticleFromTree::IsAParticleFromTree 
( const LoKi::PhysTypes::Range&         p ) 
  : LoKi::Particles::IsAParticleInTree ( p ) 
{}
// ============================================================================
//  MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::IsAParticleFromTree::result_type 
LoKi::Particles::IsAParticleFromTree::operator() 
  ( LoKi::Particles::IsAParticleFromTree::argument p ) const
{
  if ( 0 == p ) 
  {
    Error    ( " Argument is invalid! return 'false' " );
    return false ;                                             // RETURN 
  }
  if ( empty() ) 
  {
    Warning  ( " Empty list of particles is specified! return 'false' " );
    return false ;                                             // RETURN 
  }
  // find the particle by scanning of the decay trees of the other particles 
  for ( iterator ip = begin() ; end() != ip ; ++ip ) 
  {
    // find the particle by scanning of the decay tree of another particle  
    const bool found = LoKi::PhysAlgs::found 
      ( *ip , std::bind2nd ( std::equal_to<const LHCb::Particle*>() , p ) ) ;
    // OK ?
    if ( found ) { return true ; }                             // RETURN
  }
  //
  return false ;                                               // RETURN 
} 
// ============================================================================
std::ostream&
LoKi::Particles::IsAParticleFromTree::fillStream
( std::ostream& stream ) const
{ return stream << "FROM" ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

