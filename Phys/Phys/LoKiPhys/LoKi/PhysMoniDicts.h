// $Id: PhysMoniDicts.h,v 1.1 2007-07-23 17:35:48 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PHYSMONIDICTS_H 
#define LOKI_PHYSMONIDICTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
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
    LoKi::PredicateFromPredicate<const LHCb::Particle*>
    monitor 
    ( const LoKi::Predicate<const LHCb::Particle*>&   cut  ,
      StatEntity*                                     stat ) ;
    // ========================================================================    
    LoKi::FunctionFromFunction<const LHCb::Particle*>
    monitor 
    ( const LoKi::Function<const LHCb::Particle*>&    fun  ,
      StatEntity*                                     stat ) ;
    // ========================================================================
    LoKi::FunctionFromFunction<const LHCb::Particle*>
    monitor 
    ( const LoKi::Function<const LHCb::Particle*>&    fun  ,
      AIDA::IHistogram1D*                             stat ) ;
    // ========================================================================
    LoKi::PredicateFromPredicate<const LHCb::VertexBase*>
    monitor 
    ( const LoKi::Predicate<const LHCb::VertexBase*>& cut  ,
      StatEntity*                                     stat ) ;
    // ========================================================================    
    LoKi::FunctionFromFunction<const LHCb::VertexBase*>
    monitor 
    ( const LoKi::Function<const LHCb::VertexBase*>&  fun  ,
      StatEntity*                                     stat ) ;
    // ========================================================================
    LoKi::FunctionFromFunction<const LHCb::VertexBase*>
    monitor 
    ( const LoKi::Function<const LHCb::VertexBase*>&  fun  ,
      AIDA::IHistogram1D*                             stat ) ;
    // ========================================================================
  } // end of namespace LoKi::Dicts 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSMONIDICTS_H
// ============================================================================
