// $Id: MCMatchDicts.cpp,v 1.1 2007-06-10 20:17:54 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCTypes.h"
#include "LoKi/MCMatch.h"
#include "LoKi/MCMatchObj.h"
#include "LoKi/MCMatchDicts.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Dicts::MCMatchDicts
 *  @date 2007-06-06 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
// valid object?
// ============================================================================
bool LoKi::Dicts::MCMatchDicts::valid 
( const LoKi::MCMatch& m )
{ return m.validPointer() ; }
// ============================================================================
/// do these MC particles make contribution to this RC particle?
// ============================================================================
bool LoKi::Dicts::MCMatchDicts::match
( const LoKi::MCMatch&    m  ,
  const LHCb::Particle*   p  ,
  const LHCb::MCParticle* mc ) 
{ return m.validPointer() ? m->match ( p , mc ) : false ; }
// ============================================================================
// do these MC particles make contribution to this RC particle?
// ============================================================================
bool LoKi::Dicts::MCMatchDicts::match
( const LoKi::MCMatch&        m  ,
  const LHCb::Particle*       p  ,
  const LoKi::Types::MCRange& mc ) 
{
  return  
    m.validPointer() ? 
    mc.end() != m->match ( p , mc.begin() , mc.end() ) : false ; 
} 
// ============================================================================
// do these MC particles make contribution to this RC particle?
// ============================================================================
bool LoKi::Dicts::MCMatchDicts::match
( const LoKi::MCMatch&        m  ,
  const LHCb::Particle*       p  ,
  const LHCb::MCParticle::Vector& mc ) 
{
  return  
    m.validPointer() ? 
    mc.end() != m->match ( p , mc.begin() , mc.end() ) : false ; 
} 
// ============================================================================
// do these MC particles make contribution to this RC particle?
// ============================================================================
bool LoKi::Dicts::MCMatchDicts::match
( const LoKi::MCMatch&        m  ,
  const LHCb::Particle*       p  ,
  const LHCb::MCParticle::ConstVector& mc ) 
{
  return  
    m.validPointer() ? 
    mc.end() != m->match ( p , mc.begin() , mc.end() ) : false ; 
} 
// ============================================================================
// do these MC particles make contribution to this RC particle?
// ============================================================================
bool LoKi::Dicts::MCMatchDicts::match
( const LoKi::MCMatch&        m  ,
  const LHCb::Particle*       p  ,
  const LoKi::Keeper<LHCb::MCParticle>& mc ) 
{
  return  
    m.validPointer() ? 
    mc.end() != m->match ( p , mc.begin() , mc.end() ) : false ; 
} 
// ============================================================================
// do these MC particles make contribution to this RC particle?
// ============================================================================
bool LoKi::Dicts::MCMatchDicts::match
( const LoKi::MCMatch&        m  ,
  const LHCb::Particle*       p  ,
  const LoKi::UniqueKeeper<LHCb::MCParticle>& mc ) 
{
  return  
    m.validPointer() ? 
    mc.end() != m->match ( p , mc.begin() , mc.end() ) : false ; 
} 



// ============================================================================
// The END
// ============================================================================
