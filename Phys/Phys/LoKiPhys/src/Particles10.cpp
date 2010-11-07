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
#include "Event/RecVertex.h"
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
#include "LoKi/Particles10.h"
#include "LoKi/PhysAlgs.h"
#include "LoKi/VertexCast.h"
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
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LHCb::RecVertex* pv ) 
  : LoKi::Particles::HasTracks( LHCb::Track::ConstVector() ) 
{ addTracks ( pv ) ; }
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LHCb::RecVertex::ConstVector& pv ) 
  : LoKi::Particles::HasTracks( LHCb::Track::ConstVector() ) 
{ addTracks ( pv.begin() , pv.end() ) ; } 
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LHCb::RecVertex::Container* pv ) 
  : LoKi::Particles::HasTracks ( LHCb::Track::ConstVector() ) 
{ if ( 0 != pv ) { addTracks ( pv->begin() , pv->end() ) ; } } 
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LHCb::VertexBase* pv ) 
  : LoKi::Particles::HasTracks ( LHCb::Track::ConstVector() ) 
{ addTracks ( pv ) ; } 
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LHCb::VertexBase::ConstVector& pv ) 
  : LoKi::Particles::HasTracks( LHCb::Track::ConstVector() ) 
{ addTracks ( pv.begin() , pv.end() ) ; } 
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LoKi::PhysTypes::VRange& pv ) 
  : LoKi::Particles::HasTracks( LHCb::Track::ConstVector() ) 
{ addTracks ( pv.begin() , pv.end() ) ; } 
// ============================================================================
std::ostream& 
LoKi::Particles::HasTracksFromPV::fillStream
( std::ostream& stream ) const
{ return stream << "PVTRACK" ; }
// ============================================================================



// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LHCb::RecVertex*               pv  ) 
  : LoKi::Particles::HasTracksFromPV ( pv ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LHCb::RecVertex::ConstVector& pv  ) 
  : LoKi::Particles::HasTracksFromPV ( pv ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LHCb::RecVertex::Container* pv  ) 
  : LoKi::Particles::HasTracksFromPV ( pv ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LHCb::VertexBase*               pv  ) 
  : LoKi::Particles::HasTracksFromPV ( pv ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LHCb::VertexBase::ConstVector& pv  ) 
  : LoKi::Particles::HasTracksFromPV ( pv ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LoKi::PhysTypes::VRange& pv  ) 
  : LoKi::Particles::HasTracksFromPV ( pv ) 
{}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::result_type 
LoKi::Particles::HasTracksInTreeFromPV::operator() 
  ( LoKi::Particles::HasTracksInTreeFromPV::argument p ) const
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
  // scan the decay tree 
  return inTree ( p ) ;
} 
// ============================================================================
std::ostream& 
LoKi::Particles::HasTracksInTreeFromPV::fillStream
( std::ostream& stream ) const
{ return stream << "PVTRACKINTREE" ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================

