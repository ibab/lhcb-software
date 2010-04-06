// $Id: GenPIDOperators.h,v 1.7 2010-04-06 20:11:52 ibelyaev Exp $
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
LoKi::GenTypes::GCut
operator==( const LoKi::GenParticles::Identifier& id   , 
            const LHCb::ParticleID&               pid  ) ;
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const LoKi::GenParticles::Identifier& id   , 
            const LHCb::ParticleID&               pid  ) ;
// ============================================================================
LoKi::GenTypes::GCut
operator==( const LHCb::ParticleID&               pid  ,
            const LoKi::GenParticles::Identifier& id   ) ;
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const LHCb::ParticleID&               pid  ,
            const LoKi::GenParticles::Identifier& id   ) ;
// ============================================================================
inline 
LoKi::GenTypes::GCut
operator==( const long                         pid  ,
            const LoKi::GenParticles::Identifier& id   ) 
{ return LHCb::ParticleID( pid ) == id ; }
// ============================================================================
inline
LoKi::GenTypes::GCut
operator!=( const int                          pid  ,
            const LoKi::GenParticles::Identifier& id   ) 
{ return LHCb::ParticleID( pid ) != id ; }
// ============================================================================
inline 
LoKi::GenTypes::GCut
operator==( const LoKi::GenParticles::Identifier& id   , 
            const int                          pid  ) 
{ return id == LHCb::ParticleID( pid ) ; }
// ============================================================================
inline 
LoKi::GenTypes::GCut
operator!=( const LoKi::GenParticles::Identifier& id   , 
            const int                          pid  ) 
{ return id != LHCb::ParticleID( pid ) ; }
// ============================================================================
LoKi::GenTypes::GCut
operator==( const LoKi::GenParticles::Identifier& id   , 
            const std::string&                 name ) ;
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const LoKi::GenParticles::Identifier& id   , 
            const std::string&                 name ) ;
// ============================================================================
LoKi::GenTypes::GCut
operator==( const std::string&                 name , 
            const LoKi::GenParticles::Identifier& id   ) ;
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const std::string&                 name , 
            const LoKi::GenParticles::Identifier& id   ) ;
// ============================================================================
LoKi::GenTypes::GCut
operator==( const LoKi::GenParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                  pid  ) ;
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const LoKi::GenParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                  pid  ) ;
// ============================================================================
LoKi::GenTypes::GCut
operator==( const LHCb::ParticleID&                  pid  ,
            const LoKi::GenParticles::AbsIdentifier& id   ) ;
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const LHCb::ParticleID&                  pid  ,
            const LoKi::GenParticles::AbsIdentifier& id   ) ;
// ============================================================================
inline 
LoKi::GenTypes::GCut
operator==( const int                                pid  ,
            const LoKi::GenParticles::AbsIdentifier& id   ) 
{ return LHCb::ParticleID( pid ) == id ; }
// ============================================================================
inline
LoKi::GenTypes::GCut
operator!=( const int                                pid  ,
            const LoKi::GenParticles::AbsIdentifier& id   ) 
{ return LHCb::ParticleID( pid ) != id ; }
// ============================================================================
inline 
LoKi::GenTypes::GCut
operator==( const LoKi::GenParticles::AbsIdentifier& id   , 
            const int                                pid  ) 
{ return id == LHCb::ParticleID( pid ) ; }
// ============================================================================
inline 
LoKi::GenTypes::GCut
operator!=( const LoKi::GenParticles::AbsIdentifier& id   , 
            const int                                pid  ) 
{ return id != LHCb::ParticleID( pid ) ; }
// ============================================================================
LoKi::GenTypes::GCut
operator==( const LoKi::GenParticles::AbsIdentifier& id   , 
            const std::string&                       name ) ;
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const LoKi::GenParticles::AbsIdentifier& id   , 
            const std::string&                       name ) ;
// ============================================================================
LoKi::GenTypes::GCut
operator==( const std::string&                       name , 
            const LoKi::GenParticles::AbsIdentifier& id   ) ;
// ============================================================================
LoKi::GenTypes::GCut
operator!=( const std::string&                       name , 
            const LoKi::GenParticles::AbsIdentifier& id   ) ;
// ============================================================================

// ============================================================================
// vectors 
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<int>&              pids ) ;
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<unsigned int>&     pids ) ;
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<std::string>&      pids ) ;
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<LHCb::ParticleID>& pids ) ;
// ============================================================================
// abspid 
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<int>&                 pids ) ;
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<unsigned int>&        pids ) ;
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<std::string>&         pids ) ;
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator==( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<LHCb::ParticleID>&    pids ) ;
// ============================================================================

// ============================================================================
// vectors 
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator== ( const std::vector<int>&              pids , 
             const LoKi::GenParticles::Identifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator== ( const std::vector<unsigned int>&     pids , 
             const LoKi::GenParticles::Identifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator== ( const std::vector<std::string>&      pids , 
             const LoKi::GenParticles::Identifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator== ( const std::vector<LHCb::ParticleID>&  pids , 
             const LoKi::GenParticles::Identifier&    id   ) { return id == pids ; }
// ============================================================================
// abspid 
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator== ( const std::vector<int>&                 pids , 
             const LoKi::GenParticles::AbsIdentifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator== ( const std::vector<unsigned int>&        pids , 
             const LoKi::GenParticles::AbsIdentifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator== ( const std::vector<std::string>&         pids , 
             const LoKi::GenParticles::AbsIdentifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator== ( const std::vector<LHCb::ParticleID>&    pids , 
             const LoKi::GenParticles::AbsIdentifier&   id   ) { return id == pids ; }
// ============================================================================

// ============================================================================
// vectors 
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<int>&              pids ) ;
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<unsigned int>&     pids ) ;
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<std::string>&      pids ) ;
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::Identifier&   id   , 
            const std::vector<LHCb::ParticleID>& pids ) ;
// ============================================================================
// abspid 
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<int>&                 pids ) ;
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<unsigned int>&        pids ) ;
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<std::string>&         pids ) ;
// ============================================================================
LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!=( const LoKi::GenParticles::AbsIdentifier&   id   , 
            const std::vector<LHCb::ParticleID>&    pids ) ;
// ============================================================================

// ============================================================================
// vectors 
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!= ( const std::vector<int>&              pids , 
             const LoKi::GenParticles::Identifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!= ( const std::vector<unsigned int>&     pids , 
             const LoKi::GenParticles::Identifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!= ( const std::vector<std::string>&      pids , 
             const LoKi::GenParticles::Identifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!= ( const std::vector<LHCb::ParticleID>& pids ,
             const LoKi::GenParticles::Identifier&   id   ) { return id != pids ; }
// ============================================================================
// abspid 
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!= ( const std::vector<int>&                 pids , 
             const LoKi::GenParticles::AbsIdentifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!= ( const std::vector<unsigned int>&        pids , 
             const LoKi::GenParticles::AbsIdentifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!= ( const std::vector<std::string>&         pids , 
             const LoKi::GenParticles::AbsIdentifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const HepMC::GenParticle*>::PredicateFromPredicate
operator!= ( const std::vector<LHCb::ParticleID>&    pids , 
             const LoKi::GenParticles::AbsIdentifier&   id   ) { return id != pids ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENPIDOPERATORS_H
// ============================================================================
