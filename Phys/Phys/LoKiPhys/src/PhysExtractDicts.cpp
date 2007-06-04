// $Id: PhysExtractDicts.cpp,v 1.1 2007-06-04 18:25:37 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/Track.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysExtract.h"
#include "LoKi/PhysExtractDicts.h"
// ============================================================================
/** @file 
 *  Implementation file for functionc fomr namespace LoKi::Dicts::Extract 
 *  @date 2007-06-04 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
// extract from the tree 
// ============================================================================
LHCb::Particle::ConstVector
LoKi::Dicts::Extract::particles 
( const LHCb::Particle*        particle , 
  const LoKi::PhysTypes::Cuts& cut      ) 
{
  LHCb::Particle::ConstVector result ;
  LoKi::Extract::particles ( particle , std::back_inserter ( result ) , cut ) ;
  return result ;
}
// ============================================================================
// extract from the tree 
// ============================================================================
LHCb::ProtoParticle::ConstVector
LoKi::Dicts::Extract::protoParticles 
( const LHCb::Particle*        particle , 
  const LoKi::PhysTypes::Cuts& cut      ) 
{
  LHCb::ProtoParticle::ConstVector result ;
  LoKi::Extract::getProtoParticles_if 
    ( particle , std::back_inserter ( result ) , cut ) ;
  return result ;
}
// ============================================================================
// extract from the tree 
// ============================================================================
LHCb::Track::ConstVector
LoKi::Dicts::Extract::tracks 
( const LHCb::Particle*        particle , 
  const LoKi::PhysTypes::Cuts& cut      ) 
{
  LHCb::Track::ConstVector result ;
  LoKi::Extract::getTracks_if 
    ( particle , std::back_inserter ( result ) , cut ) ;
  return result ;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
