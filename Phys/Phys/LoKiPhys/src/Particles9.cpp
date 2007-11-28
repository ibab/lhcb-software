// $Id: Particles9.cpp,v 1.6 2007-11-28 14:39:31 ibelyaev Exp $
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
// LoKiPhys
// ============================================================================
#include "LoKi/Particles9.h"
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
LoKi::Particles::HasTracks::HasTracks
( const LHCb::Track* p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  (   ) 
  , LoKi::UniqueKeeper<LHCb::Track> ( p ) 
{}
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LHCb::Track::Vector& pp ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  () 
  , LoKi::UniqueKeeper<LHCb::Track> ( pp.begin() , pp.end() ) 
{}
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LHCb::Track::ConstVector& pp ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  () 
  , LoKi::UniqueKeeper<LHCb::Track> ( pp.begin() , pp.end() ) 
{}
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const SmartRefVector<LHCb::Track>& pp ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  () 
  , LoKi::UniqueKeeper<LHCb::Track> ( pp.begin() , pp.end() ) 
{}
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LoKi::Keeper<LHCb::Track>& pp ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  (    ) 
  , LoKi::UniqueKeeper<LHCb::Track> ( pp ) 
{}
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LoKi::UniqueKeeper<LHCb::Track>& pp ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  (    ) 
  , LoKi::UniqueKeeper<LHCb::Track> ( pp ) 
{}
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LoKi::Particles::HasTracks& right ) 
  : LoKi::AuxFunBase                        ( right ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  ( right ) 
  , LoKi::UniqueKeeper<LHCb::Track> ( right ) 
{}
// ============================================================================
LoKi::Particles::HasTracks::result_type 
LoKi::Particles::HasTracks::operator() 
  ( LoKi::Particles::HasTracks::argument p ) const 
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
  return std::binary_search ( begin() , end() , m_extract( p ) ) ;
} 
// ============================================================================
std::ostream& 
LoKi::Particles::HasTracks::fillStream
( std::ostream& stream ) const 
{ return stream << "TRACKS" ; }
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LHCb::Track* p ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  (   ) 
  , m_cut ( p ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LHCb::Track::Vector& pp ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  ()
  , m_cut ( pp ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LHCb::Track::ConstVector& pp ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  () 
  , m_cut ( pp )  
{}
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const SmartRefVector<LHCb::Track>& pp ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  () 
  , m_cut ( pp )  
{}
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LoKi::Keeper<LHCb::Track>& pp ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  (    ) 
  , m_cut ( pp ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LoKi::UniqueKeeper<LHCb::Track>& pp ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  (    ) 
  , m_cut ( pp ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LoKi::Particles::HasTracksInTree& right ) 
  : LoKi::AuxFunBase                        ( right ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Predicate  ( right ) 
  , m_cut                                   ( right.m_cut )
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
  if ( m_cut.empty() ) 
  {
    Warning ( " Empty list of tracks is specified! return 'false' " ) ;
    return false ;
  }
  // scan the tree 
  return LoKi::PhysAlgs::found ( p , m_cut ) ;
}
// ============================================================================
std::ostream& 
LoKi::Particles::HasTracksInTree::fillStream
( std::ostream& stream ) const 
{ return stream << "TRACKSINTREE" ; }
// ============================================================================
// The END 
// ============================================================================

