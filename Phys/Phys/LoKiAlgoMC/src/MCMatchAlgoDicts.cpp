// $Id: MCMatchAlgoDicts.cpp,v 1.1 2007-06-10 20:21:43 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Keeper.h"
#include "LoKi/UniqueKeeper.h"
#include "LoKi/MCTypes.h"
#include "LoKi/MCMatch.h"
#include "LoKi/Loop.h"
#include "LoKi/MCMatchDicts.h"
#include "LoKi/MCMatchAlgoDicts.h"
// ============================================================================
/** @file
 *  implementation file for functionf from namespace LOKi:Dicts::MCMatchDicts
 *  @author Vanya BELAYEV ibelyaev@physics.syr.edu
 *  @date 2007-07-05
 */
// ============================================================================
// do these MC particles make contribution to this RC particle?
// ============================================================================
bool LoKi::Dicts::MCMatchDicts::match
( const LoKi::MCMatch&    m  ,
  const LoKi::Loop&       l  ,
  const LHCb::MCParticle* mc ) 
{ return match ( m , l.particle() , mc ) ; }
// ============================================================================
// do these MC particles make contribution to this RC particle?
// ============================================================================
bool LoKi::Dicts::MCMatchDicts::match
( const LoKi::MCMatch&        m  ,
  const LoKi::Loop&           l  ,
  const LoKi::Types::MCRange& mc ) 
{ return match ( m , l.particle() , mc ) ; }
// ============================================================================
// do these MC particles make contribution to this RC particle?
// ============================================================================
bool LoKi::Dicts::MCMatchDicts::match
( const LoKi::MCMatch&        m  ,
  const LoKi::Loop&           l  ,
  const LHCb::MCParticle::Vector& mc ) 
{ return match ( m , l.particle() , mc ) ; }
// ============================================================================
// do these MC particles make contribution to this RC particle?
// ============================================================================
bool LoKi::Dicts::MCMatchDicts::match
( const LoKi::MCMatch&        m  ,
  const LoKi::Loop&           l  ,
  const LHCb::MCParticle::ConstVector& mc ) 
{ return match ( m , l.particle() , mc ) ; }
// ============================================================================
// do these MC particles make contribution to this RC particle?
// ============================================================================
bool LoKi::Dicts::MCMatchDicts::match
( const LoKi::MCMatch&        m  ,
  const LoKi::Loop&           l  ,
  const LoKi::Keeper<LHCb::MCParticle>& mc ) 
{ return match ( m , l.particle() , mc ) ; }
// ============================================================================
// do these MC particles make contribution to this RC particle?
// ============================================================================
bool LoKi::Dicts::MCMatchDicts::match
( const LoKi::MCMatch&        m  ,
  const LoKi::Loop&           l  ,
  const LoKi::UniqueKeeper<LHCb::MCParticle>& mc ) 
{ return match ( m , l.particle() , mc ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

