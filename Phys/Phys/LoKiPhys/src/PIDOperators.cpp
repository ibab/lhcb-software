// $Id: PIDOperators.cpp,v 1.4 2007-11-28 14:39:30 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// Kernel
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ParticleProperties.h"
#include "LoKi/Particles0.h"
#include "LoKi/PIDOperators.h"
// ============================================================================
/** @file
 *
 * Implementation file for functions from the file LoKi/PIDOperators.h
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
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::Identifier& id   , 
            const LHCb::ParticleID&            pid  )
{
  typedef LoKi::BasicFunctors<const LHCb::Particle*>::Function Func ;
  return static_cast<const Func&> ( id ) == (double) pid.pid() ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::Identifier& id   , 
            const LHCb::ParticleID&            pid  )
{
  typedef LoKi::BasicFunctors<const LHCb::Particle*>::Function Func ;
  return static_cast<const Func&> ( id ) != (double) pid.pid() ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LHCb::ParticleID&            pid  ,
            const LoKi::Particles::Identifier& id   )
{ return id == pid ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LHCb::ParticleID&            pid  ,
            const LoKi::Particles::Identifier& id   )
{ return id != pid ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::Identifier& id   , 
            const std::string&                 name )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const std::string&                 name , 
            const LoKi::Particles::Identifier& id   )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::Identifier& id   , 
            const std::string&                 name )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const std::string&                 name , 
            const LoKi::Particles::Identifier& id   )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::AbsIdentifier& id   , 
            const LHCb::ParticleID&               pid  )
{ 
  typedef LoKi::BasicFunctors<const LHCb::Particle*>::Function Func ;
  return static_cast<const Func&> ( id ) == (double) pid.abspid() ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::AbsIdentifier& id   , 
            const LHCb::ParticleID&               pid  )
{
  typedef LoKi::BasicFunctors<const LHCb::Particle*>::Function Func ;
  return static_cast<const Func&> ( id ) != (double) pid.abspid() ;
}
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LHCb::ParticleID&               pid  ,
            const LoKi::Particles::AbsIdentifier& id   )
{ return id == pid ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LHCb::ParticleID&               pid  ,
            const LoKi::Particles::AbsIdentifier& id   )
{ return id != pid ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::AbsIdentifier& id   , 
            const std::string&                    name )
{ return id == LoKi::Particles::pidFromName( name ) ;} 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const std::string&                    name , 
            const LoKi::Particles::AbsIdentifier& id   )
{ return id == LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::AbsIdentifier& id   , 
            const std::string&                    name )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const std::string&                    name , 
            const LoKi::Particles::AbsIdentifier& id   )
{ return id != LoKi::Particles::pidFromName( name ) ; } 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
