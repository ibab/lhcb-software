// $Id: BasicFunctors.h,v 1.1 2007-11-28 13:56:32 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_BASICFUNCTORS_H 
#define LOKI_BASICFUNCTORS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functor.h"
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
 *  @date 2007-10-31 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @struct BasicFunctors 
   *
   *  The helper structure to propagate the actual types for the basic functors 
   *
   *  The idea comes from Gerhard "The Great" Raven
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-10-31   
   */
  template <class TYPE>
  struct BasicFunctors 
  {
  public:
    //
    typedef LoKi::Functor<TYPE,double>             Function               ;
    typedef LoKi::Functor<TYPE,bool>               Predicate              ;
    //
    typedef LoKi::FunctorFromFunctor<TYPE,double>  FunctionFromFunction   ;
    typedef LoKi::FunctorFromFunctor<TYPE,bool>    PredicateFromPredicate ;
    //
    typedef LoKi::Constant<TYPE,double>            Constant               ;
    typedef LoKi::Constant<TYPE,bool>              BooleanConstant        ;
    //
  } ;
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_BASICFUNCTORS_H
// ============================================================================
