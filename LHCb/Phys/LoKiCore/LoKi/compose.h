// $Id: compose.h,v 1.1 2007-12-03 12:03:23 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_COMPOSE_H 
#define LOKI_COMPOSE_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Primitives.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** compose two functions (with rather liberal interfaces)
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-29
   */
  template <class TYPE,class TYPE1, class TYPE2, class TYPE3>
  inline 
  LoKi::Compose<TYPE,TYPE1,TYPE2,TYPE3>
  compose ( const LoKi::Functor<TYPE,TYPE1>&  fun1 , 
            const LoKi::Functor<TYPE3,TYPE2>& fun2 )
  {
    return LoKi::Compose<TYPE,TYPE1,TYPE2,TYPE3> ( fun1 , fun2 ) ;
  }
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_COMPOSE_H
// ============================================================================
