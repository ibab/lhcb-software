// $Id: MCMoniDicts.h,v 1.1 2007-07-23 17:27:31 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCMONIDICTS_H 
#define LOKI_MCMONIDICTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCTypes.h"
// ============================================================================
// forward declarations
// ============================================================================
class StatEntity ;
namespace AIDA { class IHistogram1D ; }
// ============================================================================
namespace LoKi
{
  namespace Dicts 
  {
    // ========================================================================
    LoKi::PredicateFromPredicate<const LHCb::MCParticle*>
    monitor
    ( const LoKi::Predicate<const LHCb::MCParticle*>&   cut  , 
      StatEntity*                                       stat ) ;
    // ========================================================================
    LoKi::FunctionFromFunction<const LHCb::MCParticle*>
    monitor
    ( const LoKi::Function<const LHCb::MCParticle*>&    fun  , 
      StatEntity*                                       stat ) ;
    // ========================================================================
    LoKi::FunctionFromFunction<const LHCb::MCParticle*>
    monitor
    ( const LoKi::Function<const LHCb::MCParticle*>&    fun  , 
      AIDA::IHistogram1D*                               hist ) ;
    // ========================================================================
    LoKi::PredicateFromPredicate<const LHCb::MCVertex*>
    monitor
    ( const LoKi::Predicate<const LHCb::MCVertex*>&     cut  , 
      StatEntity*                                       stat ) ;
    // ========================================================================
    LoKi::FunctionFromFunction<const LHCb::MCVertex*>
    monitor
    ( const LoKi::Function<const LHCb::MCVertex*>&      fun  , 
      StatEntity*                                       stat ) ;
    // ========================================================================
    LoKi::FunctionFromFunction<const LHCb::MCVertex*>
    monitor
    ( const LoKi::Function<const LHCb::MCVertex*>&      fun  , 
      AIDA::IHistogram1D*                               hist ) ;
    // ========================================================================    
  } // end of namespace LoKi::Dicts
} // end of namespace LoKi
// ============================================================================
#endif // LOKI_MCMONIDICTS_H
