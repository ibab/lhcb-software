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
( LoKi::Particles::IsAParticle::argument p ) const 
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
  // look for the particle 
  return std::binary_search ( begin() , end() , p ) ;
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
namespace 
{
  struct InList : public std::unary_function<const LHCb::Particle*,bool>
  {
  public:
    // ========================================================================
    InList ( const LoKi::Particles::IsAParticle& cut ) : m_cut ( cut ) {}    
    // ========================================================================
    bool operator() ( const LHCb::Particle* p ) const 
    { return m_cut.inList ( p ) ; }
    // =======================================================================
  private: 
    // =======================================================================
    InList () ;
    // =======================================================================
  private: 
    // =======================================================================
    const LoKi::Particles::IsAParticle& m_cut ;
    // =======================================================================
  };
  // ==========================================================================
}
// ============================================================================
LoKi::Particles::IsAParticleInTree::result_type 
LoKi::Particles::IsAParticleInTree::operator() 
  ( LoKi::Particles::IsAParticleInTree::argument p ) const
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
  return LoKi::PhysAlgs::found ( p , InList ( *this ) ) ;
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

