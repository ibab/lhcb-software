// $Id: Calls.h,v 1.5 2008-03-30 13:31:28 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_CALLS_H 
#define LOKI_CALLS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartRef.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/BasicFunctors.h"
// ============================================================================
namespace LoKi
{
  namespace Dicts 
  {
    // ========================================================================
    /** @class FunCalls LoKi/Calls.h
     *
     *  @attention The "generic" implementation fits nicely only 
     *             to few classes, for all other cases one needs to 
     *             look for template specializations 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-29
     */
    template <class TYPE>
    class FunCalls 
    {
    private:
      typedef TYPE                                                Type ;
      typedef typename LoKi::BasicFunctors<const Type*>::Function Fun  ;
    public:
      //
      static typename Fun::result_type __call__ 
      ( const Fun& fun  , const Type*           o ) { return fun ( o ) ; }
      static typename Fun::result_type __call__ 
      ( const Fun& fun  , const SmartRef<Type>& o ) { return fun ( o ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__ 
      static std::vector<typename Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const typename Type::ConstVector& o ) 
      { return o >> fun  ; }
      // __rrshift__ 
      static std::vector<typename Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const typename Type::Vector&      o ) 
      { return o >> fun  ; }
      // __rrshift__ 
      static typename Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const Type*           o ) { return fun ( o ) ; }
      // __rrshift__ 
      static typename Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const SmartRef<Type>& o ) { return fun ( o ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<const Type*,double> __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,double>& o    ) { return fun >> o  ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<const Type*,bool>   __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,bool>&   o    ) { return fun >> o  ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CutCalls Calls.h LoKi/Calls.h
     *
     *  @attention The "generic" implementation fits nicely only 
     *             to few classes, for all other cases one needs to 
     *             look for template specializations 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-29
     */
    template <class TYPE>
    class CutCalls 
    {
    private:
      typedef TYPE                                                 Type ;
      typedef typename LoKi::BasicFunctors<const Type*>::Predicate Fun  ;
    public:
      //
      static typename Fun::result_type __call__ 
      ( const Fun& fun  , const Type*                       o ) { return fun ( o ) ; }
      static typename Fun::result_type __call__ 
      ( const Fun& fun  , const SmartRef<Type>&             o ) { return fun ( o ) ; }
      // ======================================================================
      static typename Type::ConstVector __rrshift__ 
      ( const Fun& fun  , const typename Type::ConstVector& o ) { return o >> fun  ; }
      static typename Type::Vector      __rrshift__ 
      ( const Fun& fun  , const typename Type::Vector&      o ) { return o >> fun  ; }
      static typename Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const Type*                       o ) { return fun ( o ) ; }
      static typename Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const SmartRef<Type>&             o ) { return fun ( o ) ; }
      // ======================================================================
      static LoKi::FunctorFromFunctor<const Type*,bool> __rshift__            
      ( const Fun& fun  , const Fun&                        o ) { return fun >> o  ; }
      //
    } ;
    // ========================================================================
  } // end of namespace LoKi::Dicts
} //end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CALLS_H
// ============================================================================
