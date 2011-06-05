// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// Event
// ============================================================================
#include "Event/ProtoParticle.h"
#include "Event/RecVertex.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Print.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles8.h"
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
LoKi::Particles::HasProtos::HasProtos
( const LHCb::ProtoParticle* p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  (   ) 
  , LoKi::UniqueKeeper<LHCb::ProtoParticle> ( p ) 
{}
// ============================================================================
LoKi::Particles::HasProtos::HasProtos
( const LHCb::ProtoParticle::ConstVector& pp ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  () 
  , LoKi::UniqueKeeper<LHCb::ProtoParticle> ( pp.begin() , pp.end() ) 
{}
// ============================================================================
LoKi::Particles::HasProtos::HasProtos
( const LHCb::Particle* p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  (   ) 
  , LoKi::UniqueKeeper<LHCb::ProtoParticle> () 
{ 
  addProtos ( p ) ; 
}
// ============================================================================
LoKi::Particles::HasProtos::HasProtos
( const LHCb::Particle::ConstVector& p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  (   ) 
  , LoKi::UniqueKeeper<LHCb::ProtoParticle> () 
{ 
  addProtos ( p.begin() , p.end() ) ; 
}
// ============================================================================
LoKi::Particles::HasProtos::HasProtos
( const LoKi::Types::Range& p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  (   ) 
  , LoKi::UniqueKeeper<LHCb::ProtoParticle> () 
{ 
  addProtos ( p.begin() , p.end() ) ; 
}
// ============================================================================
LoKi::Particles::HasProtos::result_type 
LoKi::Particles::HasProtos::operator() 
  ( LoKi::Particles::HasProtos::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Argument is invalid! return 'false' " ) ;
    return false ;                                              
  }
  //
  if ( empty() ) 
  {
    Warning ( " Empty list of protoparticles is specified! return 'false' " ) ;
    return false ;
  }
  //
  return inList ( p ) ;
} 
// ============================================================================
bool LoKi::Particles::HasProtos::inList ( const LHCb::Particle* p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Argument is invalid! return 'false' " ) ;
    return false ;                                              
  }
  //
  if ( empty() ) 
  {
    Warning ( " Empty list of protoparticles is specified! return 'false' " ) ;
    return false ;
  }
  //
  return std::binary_search ( begin() , end() , p->proto() ) ;
}
// ============================================================================
bool LoKi::Particles::HasProtos::inTree ( const LHCb::Particle* p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Argument is invalid! return 'false' " ) ;
    return false ;                                              
  }
  //
  if ( empty() ) 
  {
    Warning ( " Empty list of protoparticles is specified! return 'false' " ) ;
    return false ;
  }
  //
  // scan the tree 
  using namespace boost::lambda ;
  return LoKi::PhysAlgs::found 
    ( p , bind ( &LoKi::Particles::HasProtos::inList , this , _1 ) ) ;
}
// ============================================================================
std::ostream& 
LoKi::Particles::HasProtos::fillStream
( std::ostream& stream ) const 
{ return stream << "PROTOS" ; }
// ============================================================================
std::size_t LoKi::Particles::HasProtos::addProtos 
( const LHCb::ProtoParticle* p ) 
{
  if ( 0 == p ) { return 0 ; }
  //
  std::size_t _size = size() ;
  addObject ( p ) ;
  return size() - _size  ;
}
// ============================================================================
std::size_t LoKi::Particles::HasProtos::addProtos 
( const LHCb::Particle* p ) 
{
  //
  if ( 0 == p ) { return 0 ; }
  //
  const SmartRefVector<LHCb::Particle>& daughters = p->daughters() ;
  //
  return    
    addProtos ( p-> proto()                         ) +
    addProtos ( daughters.begin() , daughters.end() ) ;
}
// ============================================================================


// ============================================================================
LoKi::Particles::HasProtosInTree::HasProtosInTree
( const LHCb::ProtoParticle* p ) 
  : LoKi::Particles::HasProtos ( p ) 
{}
// ============================================================================
LoKi::Particles::HasProtosInTree::HasProtosInTree
( const LHCb::ProtoParticle::ConstVector& pp ) 
  : LoKi::Particles::HasProtos ( pp ) 
{}
// ============================================================================
LoKi::Particles::HasProtosInTree::HasProtosInTree
( const LHCb::Particle* p ) 
  : LoKi::Particles::HasProtos ( p ) 
{}
// ============================================================================
LoKi::Particles::HasProtosInTree::HasProtosInTree
( const LHCb::Particle::ConstVector& p ) 
  : LoKi::Particles::HasProtos ( p ) 
{}
// ============================================================================
LoKi::Particles::HasProtosInTree::HasProtosInTree
( const LoKi::Types::Range& p ) 
  : LoKi::Particles::HasProtos ( p ) 
{}
// ============================================================================
LoKi::Particles::HasProtosInTree::result_type 
LoKi::Particles::HasProtosInTree::operator() 
  ( LoKi::Particles::HasProtosInTree::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( " Argument is invalid! return 'false' " ) ;
    return false ;                                              
  }
  if ( empty() ) 
  {
    Warning ( " Empty list of protoparticles is specified! return 'false' " ) ;
    return false ;
  }
  // scan the tree 
  return inTree ( p  ) ;
} 
// ============================================================================
std::ostream& 
LoKi::Particles::HasProtosInTree::fillStream
( std::ostream& stream ) const 
{ return stream << "PROTOSINTREE" ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

