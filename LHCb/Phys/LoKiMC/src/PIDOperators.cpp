// $Id: PIDOperators.cpp,v 1.5 2007-06-04 09:57:30 cattanem Exp $
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

// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator==( const LoKi::MCParticles::Identifier& id   , 
            const LHCb::ParticleID&              pid  )
{
  typedef LoKi::Function<const LHCb::MCParticle*> Func ;
  return static_cast<const Func&> ( id ) == (double) pid.pid() ;
};
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator!=( const LoKi::MCParticles::Identifier& id   , 
            const LHCb::ParticleID&                    pid  )
{
  typedef LoKi::Function<const LHCb::MCParticle*> Func ;
  return static_cast<const Func&> ( id ) != (double) pid.pid() ;
};
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator==( const LHCb::ParticleID&                    pid  ,
            const LoKi::MCParticles::Identifier& id   )
{ return id == pid ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator!=( const LHCb::ParticleID&                    pid  ,
            const LoKi::MCParticles::Identifier& id   )
{ return id != pid ; } ;
// ============================================================================

// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator==( const LoKi::MCParticles::Identifier& id   , 
            const std::string&                   name )
{ return id == LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator==( const std::string&                   name , 
            const LoKi::MCParticles::Identifier& id   )
{ return id == LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator!=( const LoKi::MCParticles::Identifier& id   , 
            const std::string&                   name )
{ return id != LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator!=( const std::string&                   name , 
            const LoKi::MCParticles::Identifier& id   )
{ return id != LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================

// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator==( const LoKi::MCParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                     pid  )
{ 
  typedef LoKi::Function<const LHCb::MCParticle*> Func ;
  return static_cast<const Func&> ( id ) == (double) pid.abspid() ;
};
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator!=( const LoKi::MCParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                  pid  )
{
  typedef LoKi::Function<const LHCb::MCParticle*> Func ;
  return static_cast<const Func&> ( id ) != (double) pid.abspid() ;
};
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator==( const LHCb::ParticleID&                       pid  ,
            const LoKi::MCParticles::AbsIdentifier& id   )
{ return id == pid ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator!=( const LHCb::ParticleID&                       pid  ,
            const LoKi::MCParticles::AbsIdentifier& id   )
{ return id != pid ; } ;
// ============================================================================

// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator==( const LoKi::MCParticles::AbsIdentifier& id   , 
            const std::string&                      name )
{ return id == LoKi::Particles::pidFromName( name ) ;} ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator==( const std::string&                      name , 
            const LoKi::MCParticles::AbsIdentifier& id   )
{ return id == LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator!=( const LoKi::MCParticles::AbsIdentifier& id   , 
            const std::string&                      name )
{ return id != LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::MCParticle*> 
operator!=( const std::string&                      name , 
            const LoKi::MCParticles::AbsIdentifier& id   )
{ return id != LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
