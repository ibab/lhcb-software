// $Id: MCPIDOperators.h,v 1.7 2010-04-06 20:14:39 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCPIDOPERATORS_H 
#define LOKI_MCPIDOPERATORS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// LHCnKernel
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Operators.h"
// ============================================================================
// forwarde declarations 
// ============================================================================
namespace LHCb
{
  // ==========================================================================
  class MCParticle ;
  class ParticleID ;
  // ==========================================================================
} 
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace MCParticles 
  {
    // ========================================================================
    class    Identifier ;
    class AbsIdentifier ;    
    // ========================================================================
  } 
  // ==========================================================================
}
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
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier& id   , 
            const LHCb::ParticleID&              pid  ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier& id   , 
            const LHCb::ParticleID&              pid  ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LHCb::ParticleID&              pid  ,
            const LoKi::MCParticles::Identifier& id   ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LHCb::ParticleID&              pid  ,
            const LoKi::MCParticles::Identifier& id   ) ;
// ============================================================================
inline 
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const long                           pid  ,
            const LoKi::MCParticles::Identifier& id   ) 
{ return LHCb::ParticleID( pid ) == id ; }
// ============================================================================
inline
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const long                           pid  ,
            const LoKi::MCParticles::Identifier& id   ) 
{ return LHCb::ParticleID( pid ) != id ; }
// ============================================================================
inline 
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier& id   , 
            const long                           pid  ) 
{ return id == LHCb::ParticleID( pid ) ; }
// ============================================================================
inline 
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier& id   , 
            const long                           pid  ) 
{ return id != LHCb::ParticleID( pid ) ; }
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier& id   , 
            const std::string&                   name ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier& id   , 
            const std::string&                   name ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const std::string&                   name , 
            const LoKi::MCParticles::Identifier& id   ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const std::string&                   name , 
            const LoKi::MCParticles::Identifier& id   ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                 pid  ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier& id   , 
            const LHCb::ParticleID&                 pid  ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LHCb::ParticleID&                 pid  ,
            const LoKi::MCParticles::AbsIdentifier& id   ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LHCb::ParticleID&                 pid  ,
            const LoKi::MCParticles::AbsIdentifier& id   ) ;
// ============================================================================
inline 
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const long                              pid  ,
            const LoKi::MCParticles::AbsIdentifier& id   ) 
{ return LHCb::ParticleID( pid ) == id ; }
// ============================================================================
inline
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const long                              pid  ,
            const LoKi::MCParticles::AbsIdentifier& id   ) 
{ return LHCb::ParticleID( pid ) != id ; }
// ============================================================================
inline 
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier& id   , 
            const long                              pid  ) 
{ return id == LHCb::ParticleID( pid ) ; }
// ============================================================================
inline 
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier& id   , 
            const long                            pid  ) 
{ return id != LHCb::ParticleID( pid ) ; }
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier& id   , 
            const std::string&                      name ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier& id   , 
            const std::string&                      name ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const std::string&                      name , 
            const LoKi::MCParticles::AbsIdentifier& id   ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const std::string&                      name , 
            const LoKi::MCParticles::AbsIdentifier& id   ) ;
// ============================================================================



// ============================================================================
// vectors 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<int>&              pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<unsigned int>&     pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<std::string>&      pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<LHCb::ParticleID>& pids ) ;
// ============================================================================
// abspid 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<int>&                 pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<unsigned int>&        pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<std::string>&         pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator==( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<LHCb::ParticleID>&    pids ) ;
// ============================================================================

// ============================================================================
// vectors 
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator== ( const std::vector<int>&              pids , 
             const LoKi::MCParticles::Identifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator== ( const std::vector<unsigned int>&     pids , 
             const LoKi::MCParticles::Identifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator== ( const std::vector<std::string>&      pids , 
             const LoKi::MCParticles::Identifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator== ( const std::vector<LHCb::ParticleID>&  pids , 
             const LoKi::MCParticles::Identifier&    id   ) { return id == pids ; }
// ============================================================================
// abspid 
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator== ( const std::vector<int>&                 pids , 
             const LoKi::MCParticles::AbsIdentifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator== ( const std::vector<unsigned int>&        pids , 
             const LoKi::MCParticles::AbsIdentifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator== ( const std::vector<std::string>&         pids , 
             const LoKi::MCParticles::AbsIdentifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator== ( const std::vector<LHCb::ParticleID>&    pids , 
             const LoKi::MCParticles::AbsIdentifier&   id   ) { return id == pids ; }
// ============================================================================

// ============================================================================
// vectors 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<int>&              pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<unsigned int>&     pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<std::string>&      pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::Identifier&   id   , 
            const std::vector<LHCb::ParticleID>& pids ) ;
// ============================================================================
// abspid 
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<int>&                 pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<unsigned int>&        pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<std::string>&         pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!=( const LoKi::MCParticles::AbsIdentifier&   id   , 
            const std::vector<LHCb::ParticleID>&    pids ) ;
// ============================================================================

// ============================================================================
// vectors 
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!= ( const std::vector<int>&              pids , 
             const LoKi::MCParticles::Identifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!= ( const std::vector<unsigned int>&     pids , 
             const LoKi::MCParticles::Identifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!= ( const std::vector<std::string>&      pids , 
             const LoKi::MCParticles::Identifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!= ( const std::vector<LHCb::ParticleID>& pids ,
             const LoKi::MCParticles::Identifier&   id   ) { return id != pids ; }
// ============================================================================
// abspid 
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!= ( const std::vector<int>&                 pids , 
             const LoKi::MCParticles::AbsIdentifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!= ( const std::vector<unsigned int>&        pids , 
             const LoKi::MCParticles::AbsIdentifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!= ( const std::vector<std::string>&         pids , 
             const LoKi::MCParticles::AbsIdentifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::MCParticle*>::PredicateFromPredicate
operator!= ( const std::vector<LHCb::ParticleID>&    pids , 
             const LoKi::MCParticles::AbsIdentifier&   id   ) { return id != pids ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCPIDOPERATORS_H
// ============================================================================
