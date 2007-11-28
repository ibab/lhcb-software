// $Id: PIDOperators.cpp,v 1.6 2007-11-28 14:14:00 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ParticleProperties.h"
#include "LoKi/MCParticles.h"
#include "LoKi/MCPIDOperators.h"
// ============================================================================
/** @file
 *
 * Implementation file for functions from the file LoKi/MCPIDOperators.h
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
 *  @date 2001-01-23 
 */
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier& id   , 
            const LHCb::ParticleID&              pid  )
{
  typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Function Func ;
  return static_cast<const Func&> ( id ) == (double) pid.pid() ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier& id   , 
            const LHCb::ParticleID&                    pid  )
{
  typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Function Func ;
  return static_cast<const Func&> ( id ) != (double) pid.pid() ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LHCb::ParticleID&                    pid  ,
            const LoKi::MCParticles::Identifier& id   )
{ return id == pid ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LHCb::ParticleID&                    pid  ,
            const LoKi::MCParticles::Identifier& id   )
{ return id != pid ; } 
// ============================================================================

// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier& id   , 
            const std::string&                   name )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const std::string&                   name , 
            const LoKi::MCParticles::Identifier& id   )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier& id   , 
            const std::string&                   name )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const std::string&                   name , 
            const LoKi::MCParticles::Identifier& id   )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================

// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                     pid  )
{ 
  typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Function Func ;
  return static_cast<const Func&> ( id ) == (double) pid.abspid() ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                  pid  )
{
  typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Function Func ;
  return static_cast<const Func&> ( id ) != (double) pid.abspid() ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LHCb::ParticleID&                       pid  ,
            const LoKi::MCParticles::AbsIdentifier& id   )
{ return id == pid ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LHCb::ParticleID&                       pid  ,
            const LoKi::MCParticles::AbsIdentifier& id   )
{ return id != pid ; } 
// ============================================================================

// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier& id   , 
            const std::string&                      name )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const std::string&                      name , 
            const LoKi::MCParticles::AbsIdentifier& id   )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier& id   , 
            const std::string&                      name )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const std::string&                      name , 
            const LoKi::MCParticles::AbsIdentifier& id   )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
// The END 
// ============================================================================
