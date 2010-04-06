// $Id: PIDOperators.h,v 1.7 2010-04-06 20:17:18 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PIDOPERATORS_H 
#define LOKI_PIDOPERATORS_H 1
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
  class Particle ;
  class ParticleID ;
  // ==========================================================================
} 
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles 
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
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::Identifier& id   , 
            const LHCb::ParticleID&            pid  ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::Identifier& id   , 
            const LHCb::ParticleID&            pid  ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LHCb::ParticleID&            pid  ,
            const LoKi::Particles::Identifier& id   ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LHCb::ParticleID&            pid  ,
            const LoKi::Particles::Identifier& id   ) ;
// ============================================================================
inline 
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const int                          pid  ,
            const LoKi::Particles::Identifier& id   ) 
{ return LHCb::ParticleID( pid ) == id ; }
// ============================================================================
inline
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const int                          pid  ,
            const LoKi::Particles::Identifier& id   ) 
{ return LHCb::ParticleID( pid ) != id ; }
// ============================================================================
inline 
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::Identifier& id   , 
            const int                          pid  ) 
{ return id == LHCb::ParticleID( pid ) ; }
// ============================================================================
inline 
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::Identifier& id   , 
            const int                          pid  ) 
{ return id != LHCb::ParticleID( pid ) ; }
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::Identifier& id   , 
            const std::string&                 name ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::Identifier& id   , 
            const std::string&                 name ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const std::string&                 name , 
            const LoKi::Particles::Identifier& id   ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const std::string&                 name , 
            const LoKi::Particles::Identifier& id   ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::AbsIdentifier& id   , 
            const LHCb::ParticleID&               pid  ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::AbsIdentifier& id   , 
            const LHCb::ParticleID&               pid  ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LHCb::ParticleID&               pid  ,
            const LoKi::Particles::AbsIdentifier& id   ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LHCb::ParticleID&               pid  ,
            const LoKi::Particles::AbsIdentifier& id   ) ;
// ============================================================================
inline 
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const long                            pid  ,
            const LoKi::Particles::AbsIdentifier& id   ) 
{ return LHCb::ParticleID( pid ) == id ; }
// ============================================================================
inline
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const long                            pid  ,
            const LoKi::Particles::AbsIdentifier& id   ) 
{ return LHCb::ParticleID( pid ) != id ; }
// ============================================================================
inline 
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::AbsIdentifier& id   , 
            const int                             pid  ) 
{ return id == LHCb::ParticleID( pid ) ; }
// ============================================================================
inline 
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::AbsIdentifier& id   , 
            const int                             pid  ) 
{ return id != LHCb::ParticleID( pid ) ; }
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::AbsIdentifier& id   , 
            const std::string&                    name ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::AbsIdentifier& id   , 
            const std::string&                    name ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const std::string&                    name , 
            const LoKi::Particles::AbsIdentifier& id   ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const std::string&                    name , 
            const LoKi::Particles::AbsIdentifier& id   ) ;
// ============================================================================


// ============================================================================
// vectors 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::Identifier&   id   , 
            const std::vector<int>&              pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::Identifier&   id   , 
            const std::vector<unsigned int>&     pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::Identifier&   id   , 
            const std::vector<std::string>&      pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::Identifier&   id   , 
            const std::vector<LHCb::ParticleID>& pids ) ;
// ============================================================================
// abspid 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::AbsIdentifier&   id   , 
            const std::vector<int>&                 pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::AbsIdentifier&   id   , 
            const std::vector<unsigned int>&        pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::AbsIdentifier&   id   , 
            const std::vector<std::string>&         pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator==( const LoKi::Particles::AbsIdentifier&   id   , 
            const std::vector<LHCb::ParticleID>&    pids ) ;
// ============================================================================

// ============================================================================
// vectors 
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator== ( const std::vector<int>&              pids , 
             const LoKi::Particles::Identifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator== ( const std::vector<unsigned int>&     pids , 
             const LoKi::Particles::Identifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator== ( const std::vector<std::string>&      pids , 
             const LoKi::Particles::Identifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator== ( const std::vector<LHCb::ParticleID>&  pids , 
             const LoKi::Particles::Identifier&    id   ) { return id == pids ; }
// ============================================================================
// abspid 
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator== ( const std::vector<int>&                 pids , 
             const LoKi::Particles::AbsIdentifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator== ( const std::vector<unsigned int>&        pids , 
             const LoKi::Particles::AbsIdentifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator== ( const std::vector<std::string>&         pids , 
             const LoKi::Particles::AbsIdentifier&   id   ) { return id == pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator== ( const std::vector<LHCb::ParticleID>&    pids , 
             const LoKi::Particles::AbsIdentifier&   id   ) { return id == pids ; }
// ============================================================================

// ============================================================================
// vectors 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::Identifier&   id   , 
            const std::vector<int>&              pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::Identifier&   id   , 
            const std::vector<unsigned int>&     pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::Identifier&   id   , 
            const std::vector<std::string>&      pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::Identifier&   id   , 
            const std::vector<LHCb::ParticleID>& pids ) ;
// ============================================================================
// abspid 
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::AbsIdentifier&   id   , 
            const std::vector<int>&                 pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::AbsIdentifier&   id   , 
            const std::vector<unsigned int>&        pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::AbsIdentifier&   id   , 
            const std::vector<std::string>&         pids ) ;
// ============================================================================
LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!=( const LoKi::Particles::AbsIdentifier&   id   , 
            const std::vector<LHCb::ParticleID>&    pids ) ;
// ============================================================================

// ============================================================================
// vectors 
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!= ( const std::vector<int>&              pids , 
             const LoKi::Particles::Identifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!= ( const std::vector<unsigned int>&     pids , 
             const LoKi::Particles::Identifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!= ( const std::vector<std::string>&      pids , 
             const LoKi::Particles::Identifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!= ( const std::vector<LHCb::ParticleID>& pids ,
             const LoKi::Particles::Identifier&   id   ) { return id != pids ; }
// ============================================================================
// abspid 
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!= ( const std::vector<int>&                 pids , 
             const LoKi::Particles::AbsIdentifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!= ( const std::vector<unsigned int>&        pids , 
             const LoKi::Particles::AbsIdentifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!= ( const std::vector<std::string>&         pids , 
             const LoKi::Particles::AbsIdentifier&   id   ) { return id != pids ; }
// ============================================================================
inline LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate
operator!= ( const std::vector<LHCb::ParticleID>&    pids , 
             const LoKi::Particles::AbsIdentifier&   id   ) { return id != pids ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PIDOPERATORS_H
// ============================================================================
