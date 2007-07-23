// $Id: GenMoniDicts.h,v 1.1 2007-07-23 17:23:36 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GENMONIDICTS_H 
#define LOKI_GENMONIDICTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GenTypes.h"
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
    LoKi::PredicateFromPredicate<const HepMC::GenParticle*>
    monitor
    ( const LoKi::Predicate<const HepMC::GenParticle*>& cut  , 
      StatEntity*                                       stat ) ;
    // ========================================================================
    LoKi::FunctionFromFunction<const HepMC::GenParticle*>
    monitor
    ( const LoKi::Function<const HepMC::GenParticle*>&  fun  , 
      StatEntity*                                       stat ) ;
    // ========================================================================
    LoKi::FunctionFromFunction<const HepMC::GenParticle*>
    monitor
    ( const LoKi::Function<const HepMC::GenParticle*>&  fun  , 
      AIDA::IHistogram1D*                               hist ) ;
    // ========================================================================
    LoKi::PredicateFromPredicate<const HepMC::GenVertex*>
    monitor
    ( const LoKi::Predicate<const HepMC::GenVertex*>&   cut  , 
      StatEntity*                                       stat ) ;
    // ========================================================================
    LoKi::FunctionFromFunction<const HepMC::GenVertex*>
    monitor
    ( const LoKi::Function<const HepMC::GenVertex*>&    fun  , 
      StatEntity*                                       stat ) ;
    // ========================================================================
    LoKi::FunctionFromFunction<const HepMC::GenVertex*>
    monitor
    ( const LoKi::Function<const HepMC::GenVertex*>&    fun  , 
      AIDA::IHistogram1D*                               hist ) ;
    // ========================================================================    
  } // end of namespace LoKi::Dicts
} // end of namespace LoKi
// ============================================================================
#endif // LOKI_GENMONIDICTS_H
// ============================================================================
