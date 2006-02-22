// $Id: Particles9.cpp,v 1.1 2006-02-22 20:53:48 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-22
 */
// ============================================================================

// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LHCb::Track* p ) 
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , LoKi::UniqueKeeper<LHCb::Track> ( p ) 
{};
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LHCb::Track::Vector& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  () 
  , LoKi::UniqueKeeper<LHCb::Track> ( pp.begin() , pp.end() ) 
{};
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LHCb::Track::ConstVector& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  () 
  , LoKi::UniqueKeeper<LHCb::Track> ( pp.begin() , pp.end() ) 
{};
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LoKi::Keeper<LHCb::Track>& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  (    ) 
  , LoKi::UniqueKeeper<LHCb::Track> ( pp ) 
{};
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LoKi::UniqueKeeper<LHCb::Track>& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  (    ) 
  , LoKi::UniqueKeeper<LHCb::Track> ( pp ) 
{};
// ============================================================================
LoKi::Particles::HasTracks::HasTracks
( const LoKi::Particles::HasTracks& right ) 
  : LoKi::Predicate<const LHCb::Particle*>  ( right ) 
  , LoKi::UniqueKeeper<LHCb::Track> ( right ) 
{};
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
} ;
// ============================================================================


// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LHCb::Track* p ) 
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , m_cut ( p ) 
{};
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LHCb::Track::Vector& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  ()
  , m_cut ( pp ) 
{};
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LHCb::Track::ConstVector& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  () 
  , m_cut ( pp )  
{};
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LoKi::Keeper<LHCb::Track>& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  (    ) 
  , m_cut ( pp ) 
{};
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LoKi::UniqueKeeper<LHCb::Track>& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  (    ) 
  , m_cut ( pp ) 
{};
// ============================================================================
LoKi::Particles::HasTracksInTree::HasTracksInTree
( const LoKi::Particles::HasTracksInTree& right ) 
  : LoKi::Predicate<const LHCb::Particle*>  ( right ) 
  , m_cut                                   ( right.m_cut )
{};
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
} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

