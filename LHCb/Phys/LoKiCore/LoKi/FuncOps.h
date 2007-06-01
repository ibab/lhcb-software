// $Id: FuncOps.h,v 1.1 2007-06-01 11:35:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
#ifndef LOKI_FUNCOPS_H 
#define LOKI_FUNCOPS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include  "LoKi/Functions.h"
#include  "LoKi/Operators.h"
#include  "LoKi/Funcs.h"
// ============================================================================
namespace LoKi
{
  namespace Dicts
  {
    /** @class Operators Bender/Operators.h
     *  Wrapper class for operatiosn with functions and predicates
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-10-07
     */
    template <class TYPE>
    class FuncOps 
    {
    public:
      typedef LoKi::Function<TYPE>               Func ;
      typedef LoKi::Predicate<TYPE>              Cuts ;
      typedef LoKi::FunctionFromFunction<TYPE>   Fun  ;
      typedef LoKi::PredicateFromPredicate<TYPE> Cut  ;
    public:   
      static Cut __lt__     ( const Func&  fun1 , 
                              const Func&  fun2 ) { return fun1 <  fun2 ; }
      static Cut __lt__     ( const Func&  fun1 , 
                              const double fun2 ) { return fun1 <  fun2 ; }
      static Cut __le__     ( const Func&  fun1 , 
                              const Func&  fun2 ) { return fun1 <= fun2 ; }
      static Cut __le__     ( const Func&  fun1 , 
                              const double fun2 ) { return fun1 <= fun2 ; }
      static Cut __gt__     ( const Func&  fun1 , 
                              const Func&  fun2 ) { return fun1 >  fun2 ; }
      static Cut __gt__     ( const Func&  fun1 , 
                              const double fun2 ) { return fun1 >  fun2 ; }
      static Cut __ge__     ( const Func&  fun1 , 
                              const Func&  fun2 ) { return fun1 >= fun2 ; }
      static Cut __ge__     ( const Func&  fun1 , 
                              const double fun2 ) { return fun1 >= fun2 ; }
      static Cut __eq__     ( const Func&  fun1 , 
                              const Func&  fun2 ) { return fun1 == fun2 ; }
      static Cut __eq__     ( const Func&  fun1 , 
                            const double fun2 ) { return fun1 == fun2 ; }
      static Cut __ne__     ( const Func&  fun1 , 
                              const Func&  fun2 ) { return fun1 != fun2 ; }
      static Cut __ne__     ( const Func&  fun1 , 
                              const double fun2 ) { return fun1 != fun2 ; }
      static Fun __add__    ( const Func&  fun1 , 
                              const Func&  fun2 ) { return fun1 + fun2  ; }
      static Fun __sub__    ( const Func&  fun1 , 
                              const Func&  fun2 ) { return fun1 - fun2  ; }
      static Fun __mul__    ( const Func&  fun1 , 
                              const Func&  fun2 ) { return fun1 * fun2  ; }
      static Fun __div__    ( const Func&  fun1 , 
                              const Func&  fun2 ) { return fun1 / fun2  ; }
      static Fun __add__    ( const Func&  fun1 , 
                              const double fun2 ) { return fun1 + fun2  ; }
      static Fun __sub__    ( const Func&  fun1 , 
                              const double fun2 ) { return fun1 - fun2  ; }
      static Fun __mul__    ( const Func&  fun1 , 
                              const double fun2 ) { return fun1 * fun2  ; }
      static Fun __div__    ( const Func&  fun1 , 
                              const double fun2 ) { return fun1 / fun2  ; }
      static Fun __radd__   ( const Func&  fun1 , 
                              const Func&  fun2 ) { return fun2 + fun1  ; }
      static Fun __radd__   ( const Func&  fun1 , 
                              const double fun2 ) { return fun2 + fun1  ; }
      static Fun __rsub__   ( const Func&  fun1 , 
                              const double fun2 ) { return fun2 - fun1  ; }
      static Fun __rmul__   ( const Func&  fun1 , 
                              const double fun2 ) { return fun2 * fun1  ; }
      static Fun __rdiv__   ( const Func&  fun1 , 
                              const double fun2 ) { return fun2 / fun1  ; }
      static Cut __or__     ( const Cuts&  cut1 , 
                              const Cuts&  cut2 ) { return cut1 || cut2 ; }
      static Cut __or__     ( const Cuts&  cut1 , 
                              const bool   cut2 ) { return cut1 || cut2 ; }
      static Cut __and__    ( const Cuts&  cut1 , 
                              const Cuts&  cut2 ) { return cut1 && cut2 ; }
      static Cut __and__    ( const Cuts&  cut1 , 
                              const bool   cut2 ) { return cut1 && cut2 ; }
      static Cut __ror__    ( const Cuts&  cut1 , 
                              const bool   cut2 ) { return cut2 || cut1 ; }
      static Cut __rand__   ( const Cuts&  cut1 , 
                              const bool   cut2 ) { return cut2 && cut1 ; }
      static Fun __neg__    ( const Func&  fun  ) { return -1 * fun         ; }
      static Fun __abs__    ( const Func&  fun  ) { return LoKi::abs( fun ) ; }
      static Fun __pow__    ( const Func&  fun1 , 
                              const Func&  fun2 ) { return LoKi::pow ( fun1 , fun2 ) ; }
      static Fun __pow__    ( const Func&  fun1 , 
                              const double fun2 ) { return LoKi::pow ( fun1 , fun2 ) ; }
      static Fun __pow__    ( const Func&  fun1 , 
                              const int    fun2 ) { return LoKi::pow ( fun1 , fun2 ) ; }
      static Fun __rpow__   ( const Func&  fun1 , 
                              const double fun2 ) { return LoKi::pow ( fun2 , fun1 ) ; }
      static Fun __rpow__   ( const Func&  fun1 , 
                              const int    fun2 ) { return LoKi::pow ( fun2 , fun1 ) ; }
      static Cut __invert__ ( const Cuts&  cut  ) { return !cut ; }
    } ;
  } // end of namespace LoKi::Dicts
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_FUNCOPS_H
// ============================================================================
