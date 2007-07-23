// $Id: Particles8.cpp,v 1.5 2007-07-23 17:35:56 ibelyaev Exp $
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
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , LoKi::UniqueKeeper<LHCb::ProtoParticle> ( p ) 
{};
// ============================================================================
LoKi::Particles::HasProtos::HasProtos
( const std::vector<LHCb::ProtoParticle*>& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  () 
  , LoKi::UniqueKeeper<LHCb::ProtoParticle> ( pp.begin() , pp.end() ) 
{};
// ============================================================================
LoKi::Particles::HasProtos::HasProtos
( const std::vector<const LHCb::ProtoParticle*>& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  () 
  , LoKi::UniqueKeeper<LHCb::ProtoParticle> ( pp.begin() , pp.end() ) 
{};
// ============================================================================
LoKi::Particles::HasProtos::HasProtos
( const LoKi::Keeper<LHCb::ProtoParticle>& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  (    ) 
  , LoKi::UniqueKeeper<LHCb::ProtoParticle> ( pp ) 
{};
// ============================================================================
LoKi::Particles::HasProtos::HasProtos
( const LoKi::UniqueKeeper<LHCb::ProtoParticle>& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  (    ) 
  , LoKi::UniqueKeeper<LHCb::ProtoParticle> ( pp ) 
{};
// ============================================================================
LoKi::Particles::HasProtos::HasProtos
( const LoKi::Particles::HasProtos& right ) 
  : LoKi::AuxFunBase                        ( right ) 
  , LoKi::Predicate<const LHCb::Particle*>  ( right ) 
  , LoKi::UniqueKeeper<LHCb::ProtoParticle> ( right ) 
{};
// ============================================================================
LoKi::Particles::HasProtos::result_type 
LoKi::Particles::HasProtos::operator() 
  ( LoKi::Particles::HasProtos::argument p ) const 
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
  return std::binary_search ( begin() , end() , p->proto() ) ;
} ;
// ============================================================================
std::ostream& 
LoKi::Particles::HasProtos::fillStream
( std::ostream& stream ) const 
{ return stream << "PROTOS" ; }
// ============================================================================

// ============================================================================
LoKi::Particles::HasProtosInTree::HasProtosInTree
( const LHCb::ProtoParticle* p ) 
  : LoKi::Predicate<const LHCb::Particle*>  (   ) 
  , m_cut ( p ) 
{};
// ============================================================================
LoKi::Particles::HasProtosInTree::HasProtosInTree
( const std::vector<LHCb::ProtoParticle*>& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  ()
  , m_cut ( pp ) 
{};
// ============================================================================
LoKi::Particles::HasProtosInTree::HasProtosInTree
( const std::vector<const LHCb::ProtoParticle*>& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  () 
  , m_cut ( pp )  
{};
// ============================================================================
LoKi::Particles::HasProtosInTree::HasProtosInTree
( const LoKi::Keeper<LHCb::ProtoParticle>& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  (    ) 
  , m_cut ( pp ) 
{};
// ============================================================================
LoKi::Particles::HasProtosInTree::HasProtosInTree
( const LoKi::UniqueKeeper<LHCb::ProtoParticle>& pp ) 
  : LoKi::Predicate<const LHCb::Particle*>  (    ) 
  , m_cut ( pp ) 
{};
// ============================================================================
LoKi::Particles::HasProtosInTree::HasProtosInTree
( const LoKi::Particles::HasProtosInTree& right ) 
  : LoKi::AuxFunBase                        ( right ) 
  , LoKi::Predicate<const LHCb::Particle*>  ( right ) 
  , m_cut                                   ( right.m_cut )
{};
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
  if ( m_cut.empty() ) 
  {
    Warning ( " Empty list of protoparticles is specified! return 'false' " ) ;
    return false ;
  }
  // scan the tree 
  return LoKi::PhysAlgs::found ( p , m_cut ) ;
} ;
// ============================================================================
std::ostream& 
LoKi::Particles::HasProtosInTree::fillStream
( std::ostream& stream ) const 
{ return stream << "PROTOSINTREE" ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

