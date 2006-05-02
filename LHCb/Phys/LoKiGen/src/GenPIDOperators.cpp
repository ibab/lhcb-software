// $Id: GenPIDOperators.cpp,v 1.2 2006-05-02 14:30:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// Local 
// ============================================================================
#include "LoKi/GenParticles.h"
#include "LoKi/GenPIDOperators.h"
#include "LoKi/ParticleProperties.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for functions from the file LoKi/GenPIDOperators.h
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
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const LoKi::GenParticles::Identifier& id   , 
            const LHCb::ParticleID&               pid  )
{
  typedef LoKi::Function<const HepMC::GenParticle*> Func ;
  return static_cast<const Func&> ( id ) == (double) pid.pid() ;
};
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const LoKi::GenParticles::Identifier& id   , 
            const LHCb::ParticleID&               pid  )
{
  typedef LoKi::Function<const HepMC::GenParticle*> Func ;
  return static_cast<const Func&> ( id ) != (double) pid.pid() ;
};
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const LHCb::ParticleID&               pid  ,
            const LoKi::GenParticles::Identifier& id   )
{ return id == pid ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const LHCb::ParticleID&               pid  ,
            const LoKi::GenParticles::Identifier& id   )
{ return id != pid ; } ;
// ============================================================================

// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const LoKi::GenParticles::Identifier& id   , 
            const std::string&                 name )
{ return id == LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const std::string&                 name , 
            const LoKi::GenParticles::Identifier& id   )
{ return id == LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const LoKi::GenParticles::Identifier& id   , 
            const std::string&                 name )
{ return id != LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const std::string&                 name , 
            const LoKi::GenParticles::Identifier& id   )
{ return id != LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================

// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const LoKi::GenParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                  pid  )
{
  typedef LoKi::Function<const HepMC::GenParticle*> Func ;
  return static_cast<const Func&> ( id ) == (double) pid.abspid() ;
};
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const LoKi::GenParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                  pid  )
{
  typedef LoKi::Function<const HepMC::GenParticle*> Func ;
  return static_cast<const Func&> ( id ) != (double) pid.abspid() ;
};
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const LHCb::ParticleID&                  pid  ,
            const LoKi::GenParticles::AbsIdentifier& id   )
{ return id == pid ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const LHCb::ParticleID&                  pid  ,
            const LoKi::GenParticles::AbsIdentifier& id   )
{ return id != pid ; } ;
// ============================================================================

// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const LoKi::GenParticles::AbsIdentifier& id   , 
            const std::string&                       name )
{ return id == LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const std::string&                       name , 
            const LoKi::GenParticles::AbsIdentifier& id   )
{ return id == LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const LoKi::GenParticles::AbsIdentifier& id   , 
            const std::string&                       name )
{ return id != LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const std::string&                       name , 
            const LoKi::GenParticles::AbsIdentifier& id   )
{ return id != LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
