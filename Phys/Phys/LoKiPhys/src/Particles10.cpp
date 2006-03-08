// $Id: Particles10.cpp,v 1.2 2006-03-08 14:14:52 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
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
#include "Event/PrimVertex.h"
#include "Event/Track.h"
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
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LHCb::PrimVertex* pv ) 
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , m_cut ( LHCb::Track::Vector() ) 
{ addVertex ( pv ) ; } ;
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LHCb::PrimVertex::Vector& pv ) 
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , m_cut ( LHCb::Track::Vector() ) 
{ addVertices ( pv.begin() , pv.end() ) ; } ;
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LHCb::PrimVertex::ConstVector& pv ) 
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , m_cut ( LHCb::Track::Vector() ) 
{ addVertices ( pv.begin() , pv.end() ) ; } ;
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LHCb::PrimVertices* pv ) 
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , m_cut ( LHCb::Track::Vector() ) 
{ if ( 0 != pv ) { addVertices ( pv->begin() , pv->end() ) ; } } ;
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LoKi::Keeper<LHCb::PrimVertex>& pv ) 
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , m_cut ( LHCb::Track::Vector() ) 
{ addVertices ( pv.begin() , pv.end() ) ; } ;
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LoKi::UniqueKeeper<LHCb::PrimVertex>& pv ) 
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , m_cut ( LHCb::Track::Vector() ) 
{ addVertices ( pv.begin() , pv.end() ) ; } ;
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LHCb::Vertex* pv ) 
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , m_cut ( LHCb::Track::Vector() ) 
{ addVertex ( pv ) ; } ;
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LHCb::Vertex::Vector& pv ) 
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , m_cut ( LHCb::Track::Vector() ) 
{ addVertices ( pv.begin() , pv.end() ) ; } ;
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LHCb::Vertex::ConstVector& pv ) 
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , m_cut ( LHCb::Track::Vector() ) 
{ addVertices ( pv.begin() , pv.end() ) ; } ;
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LoKi::Keeper<LHCb::Vertex>& pv ) 
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , m_cut ( LHCb::Track::Vector() ) 
{ addVertices ( pv.begin() , pv.end() ) ; } ;
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LoKi::UniqueKeeper<LHCb::Vertex>& pv ) 
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , m_cut ( LHCb::Track::Vector() ) 
{ addVertices ( pv.begin() , pv.end() ) ; } ;
// ============================================================================
LoKi::Particles::HasTracksFromPV::HasTracksFromPV
( const LoKi::Particles::HasTracksFromPV& right ) 
  : LoKi::Predicate<const LHCb::Particle*>  ( right ) 
  , m_cut ( right.m_cut )  
{} ;
// ============================================================================
size_t LoKi::Particles::HasTracksFromPV::addVertex
( const LHCb::PrimVertex* pv ) 
{
  if ( 0 != pv ) 
  { m_cut.addObjects ( pv->tracks().begin() , pv->tracks().end() ) ; }
  return size() ;
} ;
// ============================================================================
size_t LoKi::Particles::HasTracksFromPV::addVertex
( const LHCb::Vertex* pv ) 
{
  if ( 0 == pv || !pv->isPrimary() ) { return m_cut.size() ; }
  return addVertex( dynamic_cast<const LHCb::PrimVertex*> ( pv ) ) ;
} ;
// ============================================================================
LoKi::Particles::HasTracksFromPV::result_type 
LoKi::Particles::HasTracksFromPV::operator() 
  ( LoKi::Particles::HasTracksFromPV::argument p ) const 
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
  return m_cut ( p ) ;
} ;
// ============================================================================
std::ostream& 
LoKi::Particles::HasTracksFromPV::fillStream
( std::ostream& stream ) const
{ return stream << "PVTRACK" ; }
// ============================================================================



// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LHCb::PrimVertex*               pv  ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_cut ( pv ) {}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LHCb::PrimVertex::Vector& pv  ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_cut ( pv ) {}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LHCb::PrimVertex::ConstVector& pv  ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_cut ( pv ) {}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LHCb::PrimVertices* pv  ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_cut ( pv ) {}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LoKi::Keeper<LHCb::PrimVertex>& pv  ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_cut ( pv ) {}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LoKi::UniqueKeeper<LHCb::PrimVertex>& pv  ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_cut ( pv ) {}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LHCb::Vertex*               pv  ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_cut ( pv ) {}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LHCb::Vertex::Vector& pv  ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_cut ( pv ) {}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LHCb::Vertex::ConstVector& pv  ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_cut ( pv ) {}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LoKi::Keeper<LHCb::Vertex>& pv  ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_cut ( pv ) {}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV 
( const LoKi::UniqueKeeper<LHCb::Vertex>& pv  ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_cut ( pv ) {}
// ============================================================================
LoKi::Particles::HasTracksInTreeFromPV::HasTracksInTreeFromPV
( const LoKi::Particles::HasTracksInTreeFromPV& right ) 
  : LoKi::Predicate<const LHCb::Particle*> ( right       ) 
  , m_cut                                  ( right.m_cut ) 
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
  return LoKi::PhysAlgs::found ( p , m_cut ) ;
} ; 
// ============================================================================
std::ostream& 
LoKi::Particles::HasTracksInTreeFromPV::fillStream
( std::ostream& stream ) const
{ return stream << "PVTRACKINTREE" ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================

