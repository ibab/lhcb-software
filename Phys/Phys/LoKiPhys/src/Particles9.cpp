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
// ============================================================================
// Event 
// ============================================================================
#include "Event/RecVertex.h"
#include "Event/ProtoParticle.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles9.h"
#include "LoKi/PhysAlgs.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning(disable:177) // ... was declared but never referenced
#endif
// ============================================================================
/// Boost 
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
LoKi::Particles::HasTracks::HasTracks
( const LHCb::Track* p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  (   ) 
  , LoKi::UniqueKeeper<LHCb::Track> ( p ) 
{}
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LHCb::Track::ConstVector& pp ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  () 
  , LoKi::UniqueKeeper<LHCb::Track> ( pp.begin() , pp.end() ) 
{}
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LHCb::Track::Container* pp ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  () 
  , LoKi::UniqueKeeper<LHCb::Track> () 
{
  if ( 0 != pp ) { addObjects ( pp->begin() , pp->end() ) ; }
}
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LHCb::Particle* p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  () 
  , LoKi::UniqueKeeper<LHCb::Track> () 
{
  addTracks ( p ) ;
}
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LHCb::Particle::ConstVector& p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  () 
  , LoKi::UniqueKeeper<LHCb::Track> () 
{
  addTracks ( p.begin() , p.end() ) ;
}
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LoKi::PhysTypes::Range& p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  () 
  , LoKi::UniqueKeeper<LHCb::Track> () 
{
  addTracks ( p.begin() , p.end() ) ;
}
// ============================================================================
LoKi::Particles::HasTracks::result_type 
LoKi::Particles::HasTracks::operator() 
  ( LoKi::Particles::HasTracks::argument p ) const 
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
    Warning ( " Empty list of tracks is specified! return 'false' " ) ;
    return false ;
  }
  //
  return inList ( p ) ;
} 
// ============================================================================
// check if particle has a track in list 
// ============================================================================
bool LoKi::Particles::HasTracks::inList ( const LHCb::Particle* p ) const 
{
  if ( 0 == p ) { return false ; } ;
  return std::binary_search ( begin() , end() , m_extract( p ) ) ;  
}
// ============================================================================
bool LoKi::Particles::HasTracks::inTree ( const LHCb::Particle* p ) const 
{
  if ( 0 == p ) { return false ; }
  //
  if ( empty() ) 
  {
    Warning ( " Empty list of tracks is specified! return 'false' " ) ;
    return false ;
  }
  //
  // scan the tree 
  using namespace boost::lambda ;
  return LoKi::PhysAlgs::found 
    ( p , bind ( &LoKi::Particles::HasTracks::inList , this , _1 ) ) ;
}
// ============================================================================


// ============================================================================
std::ostream& 
LoKi::Particles::HasTracks::fillStream
( std::ostream& stream ) const 
{ return stream << "TRACKS" ; }
// ============================================================================
std::size_t LoKi::Particles::HasTracks::addTracks ( const LHCb::Track* track    ) 
{
  std::size_t _size = size() ;
  addObject ( track ) ;
  return size() - _size ;
}
// ============================================================================
std::size_t LoKi::Particles::HasTracks::addTracks 
( const LHCb::ProtoParticle* pp ) 
{
  if ( 0 == pp ) { return 0 ; } // RETURN 
  return addTracks ( pp->track () ) ;
}
// ============================================================================
std::size_t LoKi::Particles::HasTracks::addTracks 
( const LHCb::Particle* p ) 
{
  //
  if ( 0 == p ) { return 0 ; } // RETURN 
  //
  const SmartRefVector<LHCb::Particle>& daughters = p->daughters() ;
  //
  return
    addTracks ( p->proto()                          ) + 
    addTracks ( daughters.begin() , daughters.end() ) ;

}
// ============================================================================
std::size_t LoKi::Particles::HasTracks::addTracks 
( const LHCb::RecVertex* v ) 
{
  //
  if ( 0 == v ) { return 0 ; } // RETURN 
  //
  const SmartRefVector<LHCb::Track>& tracks = v->tracks() ;
  //
  return addTracks ( tracks.begin() , tracks.end() ) ;
}
// ============================================================================
std::size_t LoKi::Particles::HasTracks::addTracks 
( const LHCb::Vertex* v ) 
{
  //
  if ( 0 == v ) { return 0 ; } // RETURN 
  //
  const SmartRefVector<LHCb::Particle>& outgoing = v->outgoingParticles() ;
  //
  return addTracks ( outgoing.begin() , outgoing.end() ) ;
}
// ============================================================================
std::size_t LoKi::Particles::HasTracks::addTracks 
( const LHCb::VertexBase* v ) 
{
  //
  if ( 0 == v ) { return 0 ; } // RETURN 
  //
  const LHCb::Vertex*    vx = dynamic_cast<const LHCb::Vertex*>    ( v ) ;
  if ( 0 != vx ) { return addTracks ( vx ) ; }
  const LHCb::RecVertex* rv = dynamic_cast<const LHCb::RecVertex*> ( v ) ;
  if ( 0 != rv ) { return addTracks ( rv ) ; }
  //
  Warning("Invalid vertex type, skip");
  //
  return 0 ;
}
// ============================================================================


// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LHCb::Track* p ) 
  : LoKi::Particles::HasTracks ( p ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LHCb::Track::ConstVector& pp ) 
  : LoKi::Particles::HasTracks ( pp ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LHCb::Track::Container* pp ) 
  : LoKi::Particles::HasTracks ( pp ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LHCb::Particle* pp ) 
  : LoKi::Particles::HasTracks ( pp ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LHCb::Particle::ConstVector& pp ) 
  : LoKi::Particles::HasTracks ( pp ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LoKi::PhysTypes::Range& pp ) 
  : LoKi::Particles::HasTracks ( pp ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTree::result_type 
LoKi::Particles::HasTracksInTree::operator() 
  ( LoKi::Particles::HasTracksInTree::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( " Argument is invalid! return 'false' " ) ;
    return false ;                                              
  }
  if ( empty() ) 
  {
    Warning ( " Empty list of tracks is specified! return 'false' " ) ;
    return false ;
  }
  // scan the tree 
  return inTree ( p ) ;
}
// ============================================================================
std::ostream& 
LoKi::Particles::HasTracksInTree::fillStream
( std::ostream& stream ) const 
{ return stream << "TRACKSINTREE" ; }
// ============================================================================
// The END 
// ============================================================================

