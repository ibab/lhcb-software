// $Id: PIDOperators.cpp,v 1.2 2007-04-16 16:16:27 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/04/23 10:06:13  ibelyaev
//   add operators for ID and ABSID
//
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// Kernel
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ParticleProperties.h"
#include "LoKi/Particles0.h"
#include "LoKi/PIDOperators.h"
// ============================================================================

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

// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const LoKi::Particles::Identifier& id   , 
            const LHCb::ParticleID&            pid  )
{
  typedef LoKi::Function<const LHCb::Particle*> Func ;
  return static_cast<const Func&> ( id ) == (double) pid.pid() ;
};
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const LoKi::Particles::Identifier& id   , 
            const LHCb::ParticleID&            pid  )
{
  typedef LoKi::Function<const LHCb::Particle*> Func ;
  return static_cast<const Func&> ( id ) != (double) pid.pid() ;
};
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const LHCb::ParticleID&            pid  ,
            const LoKi::Particles::Identifier& id   )
{ return id == pid ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const LHCb::ParticleID&            pid  ,
            const LoKi::Particles::Identifier& id   )
{ return id != pid ; } ;
// ============================================================================

// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const LoKi::Particles::Identifier& id   , 
            const std::string&                 name )
{ return id == LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const std::string&                 name , 
            const LoKi::Particles::Identifier& id   )
{ return id == LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const LoKi::Particles::Identifier& id   , 
            const std::string&                 name )
{ return id != LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const std::string&                 name , 
            const LoKi::Particles::Identifier& id   )
{ return id != LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================

// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const LoKi::Particles::AbsIdentifier& id   , 
            const LHCb::ParticleID&               pid  )
{ 
  typedef LoKi::Function<const LHCb::Particle*> Func ;
  return static_cast<const Func&> ( id ) == (double) pid.abspid() ;
};
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const LoKi::Particles::AbsIdentifier& id   , 
            const LHCb::ParticleID&               pid  )
{
  typedef LoKi::Function<const LHCb::Particle*> Func ;
  return static_cast<const Func&> ( id ) != (double) pid.abspid() ;
};
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const LHCb::ParticleID&               pid  ,
            const LoKi::Particles::AbsIdentifier& id   )
{ return id == pid ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const LHCb::ParticleID&               pid  ,
            const LoKi::Particles::AbsIdentifier& id   )
{ return id != pid ; } ;
// ============================================================================

// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const LoKi::Particles::AbsIdentifier& id   , 
            const std::string&                    name )
{ return id == LoKi::Particles::pidFromName( name ) ;} ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator==( const std::string&                    name , 
            const LoKi::Particles::AbsIdentifier& id   )
{ return id == LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const LoKi::Particles::AbsIdentifier& id   , 
            const std::string&                    name )
{ return id != LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================
LoKi::PredicateFromPredicate<const LHCb::Particle*> 
operator!=( const std::string&                    name , 
            const LoKi::Particles::AbsIdentifier& id   )
{ return id != LoKi::Particles::pidFromName( name ) ; } ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
