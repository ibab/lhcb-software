// $Id$
// ============================================================================
// Include files 
// ============================================================================
#include <algorithm>
#include <functional>
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
/* get all tracks from the given particle 
 *  @param particle the particle 
 *  @return container of tracks 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-07-10
 */
// ============================================================================
LHCb::Track::ConstVector
LoKi::Dicts::Extract::tracks ( const LHCb::Particle* particle ) 
{
  LHCb::Track::ConstVector result ;
  LoKi::Extract::getTracks ( particle , std::back_inserter ( result ) ) ;
  return result ;
}
// ============================================================================
/*  get all tracks from the given particle 
 *  @param particle the particle 
 *  @param pcut     the cut
 *  @return container of tracks 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-07-10
 */
// ============================================================================
LHCb::Track::ConstVector
LoKi::Dicts::Extract::tracks 
( const LHCb::Particle*                                        particle ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& pcut     ) 
{
  LHCb::Track::ConstVector result ;
  LoKi::Extract::getTracks_if ( particle , std::back_inserter ( result ) , pcut ) ;
  return result ;
}
// ==========================================================================
/*  get all tracks from the given particle 
 *  @param particle the particle 
 *  @param tcut     the cut
 *  @return container of tracks 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-07-10
 */
// ==========================================================================
LHCb::Track::ConstVector
LoKi::Dicts::Extract::tracks 
( const LHCb::Particle*                                        particle ,
  const LoKi::BasicFunctors<const LHCb::Track*>::Predicate&    tcut     ) 
{
  LHCb::Track::ConstVector r0 ;
  LoKi::Extract::getTracks   ( particle , std::back_inserter ( r0 ) ) ;
  LHCb::Track::ConstVector r1 ; r1.reserve ( r0.size() ) ;
  std::copy_if ( r0.begin() , r0.end(), std::back_inserter ( r1 ) , std::cref(tcut) ) ;
  return r1 ;
}
// ==========================================================================
/*  get all tracks from the given particle 
 *  @param particle the particle 
 *  @param pcut     the cut
 *  @param tcut     the cut
 *  @return container of tracks 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-07-10
 */
// ==========================================================================
LHCb::Track::ConstVector
LoKi::Dicts::Extract::tracks 
( const LHCb::Particle*                                        particle ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& pcut     ,
  const LoKi::BasicFunctors<const LHCb::Track*>::Predicate&    tcut     ) 
{
  LHCb::Track::ConstVector r0 ;
  LoKi::Extract::getTracks_if ( particle , std::back_inserter ( r0 ) , pcut ) ;
  LHCb::Track::ConstVector r1 ; r1.reserve ( r0.size() ) ;
  std::copy_if ( r0.begin() , r0.end(), std::back_inserter ( r1 ) , std::cref(tcut) ) ;
  return r1 ;
}
// ==========================================================================
/*  get all tracks from the given particle 
 *  @param particle the particle 
 *  @param pcut     the cut
 *  @param tcut     the cut
 *  @return container of tracks 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2015-07-10
 */
// ==========================================================================
LHCb::Track::ConstVector
LoKi::Dicts::Extract::tracks 
( const LHCb::Particle*                                        particle ,
  const LoKi::BasicFunctors<const LHCb::Track*>::Predicate&    tcut     ,
  const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& pcut     )
{ return tracks ( particle , pcut , tcut ) ; }



// ============================================================================
// The END 
// ============================================================================
