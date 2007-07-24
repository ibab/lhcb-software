// $Id: AMoniDicts.h,v 1.1 2007-07-24 05:20:09 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_AMONIDICTS_H 
#define LOKI_AMONIDICTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ATypes.h"
// ============================================================================
// forward declarations
// ============================================================================
namespace AIDA { class IHistogram1D ; }
class StatEntity ;
// ============================================================================
namespace LoKi
{
  namespace Dicts
  {
    // ========================================================================
    LoKi::PredicateFromPredicate<LoKi::ATypes::Combination>
    monitor
    ( const LoKi::Predicate<LoKi::ATypes::Combination>& cut  , 
      StatEntity*                                       stat ) ;
    // ========================================================================
    LoKi::FunctionFromFunction<LoKi::ATypes::Combination>
    monitor
    ( const LoKi::Function<LoKi::ATypes::Combination>&  fun  , 
      StatEntity*                                       stat ) ;
    // ========================================================================
    LoKi::FunctionFromFunction<LoKi::ATypes::Combination>
    monitor
    ( const LoKi::Function<LoKi::ATypes::Combination>&  fun   , 
      AIDA::IHistogram1D*                               histo ) ;
    // ========================================================================    
  } 
}
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_AMONIDICTS_H
// ============================================================================
