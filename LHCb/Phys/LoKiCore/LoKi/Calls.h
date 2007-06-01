// $Id: Calls.h,v 1.1 2007-06-01 11:35:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
// ============================================================================
namespace LoKi
{
  namespace Dicts 
  {
    /** @class FunCalls Calls.h LoKi/Calls.h
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-29
     */
    template <class TYPE>
    class FunCalls 
    {
    private:
      typedef TYPE                        Type ;
      typedef LoKi::Function<const Type*> Fun  ;
    public:
      static typename Fun::result_type __call__ 
      ( const Fun& fun  , const Type*           o ) { return fun ( o ) ; }
      static typename Fun::result_type __call__ 
      ( const Fun& fun  , const SmartRef<Type>& o ) { return fun ( o ) ; }
    } ;
    /** @class CutCalls Calls.h LoKi/Calls.h
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-05-29
     */
    template <class TYPE>
    class CutCalls 
    {
    private:
      typedef TYPE                         Type ;
      typedef LoKi::Predicate<const Type*> Fun ;
    public:
      static typename Fun::result_type __call__ 
      ( const Fun& fun  , const Type*           o ) { return fun ( o ) ; }
      static typename Fun::result_type __call__ 
      ( const Fun& fun  , const SmartRef<Type>& o ) { return fun ( o ) ; }
    } ;
  } // end of namespace LoKi::Dicts
} //end oif namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CALLS_H
// ============================================================================
