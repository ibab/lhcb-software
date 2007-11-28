// $Id: Particles11.cpp,v 1.7 2007-11-28 14:39:30 ibelyaev Exp $
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
( const LHCb::Particles*                p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ()
{ if ( 0 != p ) { addObjects( p->begin() , p->end() ) ; } }
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticle::IsAParticle 
( const SmartRefVector<LHCb::Particle>& p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ( p.begin() , p.end() ) 
{} 
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticle::IsAParticle 
( const LHCb::Particle::Vector&         p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ( p.begin() , p.end() ) 
{} 
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
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticle::IsAParticle 
( const LoKi::Keeper<LHCb::Particle>&   p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
    , LoKi::UniqueKeeper<LHCb::Particle> ( p ) 
{} 
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticle::IsAParticle 
( const LoKi::UniqueKeeper<LHCb::Particle>& p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ( p ) 
{} 
// ============================================================================
//  copy constructor
// ============================================================================
LoKi::Particles::IsAParticle::IsAParticle 
( const LoKi::Particles::IsAParticle& right ) 
  : LoKi::AuxFunBase                      ( right )
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate( right ) 
  , LoKi::UniqueKeeper<LHCb::Particle>    ( right ) 
{} 
// ============================================================================
//  MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::IsAParticle::result_type 
LoKi::Particles::IsAParticle::operator() 
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
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut ( p )
{}  
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticleInTree::IsAParticleInTree
( const LHCb::Particles*                p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut ( p )
{}
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticleInTree::IsAParticleInTree
( const SmartRefVector<LHCb::Particle>& p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut ( p )
{}  
// ============================================================================
//  constructor from vector of particles 
// ============================================================================
LoKi::Particles::IsAParticleInTree::IsAParticleInTree
( const LHCb::Particle::Vector&         p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut ( p )
{}  
// ============================================================================
//  constructor from vector of particles 
// ============================================================================
LoKi::Particles::IsAParticleInTree::IsAParticleInTree
( const LHCb::Particle::ConstVector&    p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut ( p )
{}  
// ============================================================================
//  constructor from container of particle 
// ============================================================================
LoKi::Particles::IsAParticleInTree::IsAParticleInTree
( const LoKi::PhysTypes::Range&         p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut ( p )
{}  
// ============================================================================
//  constructor from container of particle 
// ============================================================================
LoKi::Particles::IsAParticleInTree::IsAParticleInTree
( const LoKi::Keeper<LHCb::Particle>&   p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut ( p )
{}  
// ============================================================================
//  constructor from container of particle 
// ============================================================================
LoKi::Particles::IsAParticleInTree::IsAParticleInTree
( const LoKi::UniqueKeeper<LHCb::Particle>& p )       
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
  , m_cut ( p )
{}  
// ============================================================================
//  copy constructor 
// ============================================================================
LoKi::Particles::IsAParticleInTree::IsAParticleInTree
( const LoKi::Particles::IsAParticleInTree&  right ) 
  : LoKi::AuxFunBase                       ( right )
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate ( right ) 
  , m_cut                                  ( right.m_cut  )
{}  
// ============================================================================
//  MANDATORY: the only one essential method
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
  // scan the tree 
  return LoKi::PhysAlgs::found ( p , m_cut ) ;
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
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ( p ) 
{} 
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticleFromTree::IsAParticleFromTree 
( const LHCb::Particles*                p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ()
{ if ( 0 != p ) { addObjects( p->begin() , p->end() ) ; } }
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticleFromTree::IsAParticleFromTree
( const SmartRefVector<LHCb::Particle>& p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ( p.begin() , p.end() ) 
{} 
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticleFromTree::IsAParticleFromTree
( const LHCb::Particle::Vector&         p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ( p.begin() , p.end() ) 
{} 
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticleFromTree::IsAParticleFromTree
( const LHCb::Particle::ConstVector&    p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ( p.begin() , p.end() ) 
{} 
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticleFromTree::IsAParticleFromTree 
( const LoKi::PhysTypes::Range&         p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ( p.begin() , p.end() ) 
{} 
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticleFromTree::IsAParticleFromTree
( const LoKi::Keeper<LHCb::Particle>&   p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
    , LoKi::UniqueKeeper<LHCb::Particle> ( p ) 
{} 
// ============================================================================
//  constructor from container of particles
// ============================================================================
LoKi::Particles::IsAParticleFromTree::IsAParticleFromTree
( const LoKi::UniqueKeeper<LHCb::Particle>& p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate() 
  , LoKi::UniqueKeeper<LHCb::Particle> ( p ) 
{} 
// ============================================================================
//  copy constructor
// ============================================================================
LoKi::Particles::IsAParticleFromTree::IsAParticleFromTree 
( const LoKi::Particles::IsAParticleFromTree& right ) 
  : LoKi::AuxFunBase                      ( right )
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate( right ) 
  , LoKi::UniqueKeeper<LHCb::Particle>    ( right ) 
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

