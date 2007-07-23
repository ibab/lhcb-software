// $Id: GenPIDOperators.h,v 1.5 2007-07-23 17:23:36 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GENPIDOPERATORS_H 
#define LOKI_GENPIDOPERATORS_H 1
// ============================================================================
// Include files
// ============================================================================
// LCbKernel
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Operators.h"
#include "LoKi/GenParticles.h"
// ============================================================================
/** @file
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
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const LoKi::GenParticles::Identifier& id   , 
            const LHCb::ParticleID&               pid  ) ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const LoKi::GenParticles::Identifier& id   , 
            const LHCb::ParticleID&               pid  ) ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const LHCb::ParticleID&               pid  ,
            const LoKi::GenParticles::Identifier& id   ) ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const LHCb::ParticleID&               pid  ,
            const LoKi::GenParticles::Identifier& id   ) ;
// ============================================================================
inline 
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const long                         pid  ,
            const LoKi::GenParticles::Identifier& id   ) 
{ return LHCb::ParticleID( pid ) == id ; }
// ============================================================================
inline
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const long                         pid  ,
            const LoKi::GenParticles::Identifier& id   ) 
{ return LHCb::ParticleID( pid ) != id ; }
// ============================================================================
inline 
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const LoKi::GenParticles::Identifier& id   , 
            const long                         pid  ) 
{ return id == LHCb::ParticleID( pid ) ; }
// ============================================================================
inline 
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const LoKi::GenParticles::Identifier& id   , 
            const long                         pid  ) 
{ return id != LHCb::ParticleID( pid ) ; }
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const LoKi::GenParticles::Identifier& id   , 
            const std::string&                 name ) ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const LoKi::GenParticles::Identifier& id   , 
            const std::string&                 name ) ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const std::string&                 name , 
            const LoKi::GenParticles::Identifier& id   ) ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const std::string&                 name , 
            const LoKi::GenParticles::Identifier& id   ) ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const LoKi::GenParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                  pid  ) ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const LoKi::GenParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                  pid  ) ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const LHCb::ParticleID&                  pid  ,
            const LoKi::GenParticles::AbsIdentifier& id   ) ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const LHCb::ParticleID&                  pid  ,
            const LoKi::GenParticles::AbsIdentifier& id   ) ;
// ============================================================================
inline 
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const long                               pid  ,
            const LoKi::GenParticles::AbsIdentifier& id   ) 
{ return LHCb::ParticleID( pid ) == id ; }
// ============================================================================
inline
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const long                               pid  ,
            const LoKi::GenParticles::AbsIdentifier& id   ) 
{ return LHCb::ParticleID( pid ) != id ; }
// ============================================================================
inline 
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const LoKi::GenParticles::AbsIdentifier& id   , 
            const long                               pid  ) 
{ return id == LHCb::ParticleID( pid ) ; }
// ============================================================================
inline 
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const LoKi::GenParticles::AbsIdentifier& id   , 
            const long                               pid  ) 
{ return id != LHCb::ParticleID( pid ) ; }
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const LoKi::GenParticles::AbsIdentifier& id   , 
            const std::string&                       name ) ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const LoKi::GenParticles::AbsIdentifier& id   , 
            const std::string&                       name ) ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator==( const std::string&                       name , 
            const LoKi::GenParticles::AbsIdentifier& id   ) ;
// ============================================================================
LoKi::PredicateFromPredicate<const HepMC::GenParticle*> 
operator!=( const std::string&                       name , 
            const LoKi::GenParticles::AbsIdentifier& id   ) ;
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENPIDOPERATORS_H
// ============================================================================
