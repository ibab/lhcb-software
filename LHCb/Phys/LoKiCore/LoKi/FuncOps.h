// $Id: FuncOps.h,v 1.5 2007-08-12 18:53:35 ibelyaev Exp $
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
#include  "LoKi/Monitoring.h"
#include  "LoKi/MoreFunctions.h"
// ============================================================================
namespace LoKi
{
  namespace Dicts
  {
    // ========================================================================
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
      // hand-made pseudo-operators to simplify the treatment of math:
      static Fun __sin__    ( const Func&  f    ) { return LoKi:: sin   ( f )  ; }
      static Fun __cos__    ( const Func&  f    ) { return LoKi:: cos   ( f )  ; }
      static Fun __tan__    ( const Func&  f    ) { return LoKi:: tan   ( f )  ; }
      static Fun __sinh__   ( const Func&  f    ) { return LoKi:: sinh  ( f )  ; }
      static Fun __cosh__   ( const Func&  f    ) { return LoKi:: cosh  ( f )  ; }
      static Fun __tanh__   ( const Func&  f    ) { return LoKi:: tanh  ( f )  ; }
      static Fun __asin__   ( const Func&  f    ) { return LoKi::asin   ( f )  ; }
      static Fun __acos__   ( const Func&  f    ) { return LoKi::acos   ( f )  ; }
      static Fun __atan__   ( const Func&  f    ) { return LoKi::atan   ( f )  ; }
      static Fun __exp__    ( const Func&  f    ) { return LoKi:: exp   ( f )  ; }
      static Fun __log__    ( const Func&  f    ) { return LoKi:: log   ( f )  ; }
      static Fun __log10__  ( const Func&  f    ) { return LoKi:: log10 ( f )  ; }
      static Fun __sqrt__   ( const Func&  f    ) { return LoKi:: sqrt  ( f )  ; }
      static Fun __cbrt__   ( const Func&  f    ) { return LoKi:: cbrt  ( f )  ; }
      static Fun __pow2__   ( const Func&  f    ) { return LoKi:: pow2  ( f )  ; }
      static Fun __pow3__   ( const Func&  f    ) { return LoKi:: pow3  ( f )  ; }
      static Fun __pow4__   ( const Func&  f    ) { return LoKi:: pow4  ( f )  ; }
      static Fun __atan__   ( const Func&  f1   , 
                              const Func&  f2   ) { return LoKi::atan2 ( f1 , f2 ) ; }
      static Fun __atan__   ( const Func&  f1   , 
                              const double f2   ) { return LoKi::atan2 ( f1 , f2 ) ; }
      static Fun __atan2__  ( const Func&  f1   , 
                              const Func&  f2   ) { return LoKi::atan2 ( f1 , f2 ) ; }
      static Fun __atan2__  ( const Func&  f1   , 
                              const double f2   ) { return LoKi::atan2 ( f1 , f2 ) ; }
      // ======================================================================
      // min
      static Fun __min__    ( const Func&  f1   , 
                              const Func&  f2   ) 
      { return LoKi::min ( f1 , f2 ) ; }
      // min
      static Fun __min__    ( const Func&  f1   , 
                              const double f2   ) 
      { return LoKi::min ( f1 , f2 ) ; }
      // min
      static Fun __min__    ( const Func&  f1   , 
                              const Func&  f2   , 
                              const Func&  f3   ) 
      { return LoKi::min( f1 , f2 , f3 ) ; }
      // min
      static Fun __min__    ( const Func&  f1   , 
                              const Func&  f2   , 
                              const Func&  f3   , 
                              const Func&  f4   ) 
      { return LoKi::min ( f1 , f2 , f3 , f4 ) ; }      
      // ======================================================================
      // max
      static Fun __max__    ( const Func&  f1   , 
                              const Func&  f2   ) 
      { return LoKi::max ( f1 , f2 ) ; }
      // max
      static Fun __max__    ( const Func&  f1   , 
                              const double f2   ) 
      { return LoKi::max ( f1 , f2 ) ; }
      // max
      static Fun __max__    ( const Func&  f1   , 
                              const Func&  f2   , 
                              const Func&  f3   ) 
      { return LoKi::max ( f1 , f2 , f3 ) ; }
      // max
      static Fun __max__    ( const Func&  f1   , 
                              const Func&  f2   , 
                              const Func&  f3   , 
                              const Func&  f4   ) 
      { return LoKi::max ( f1 , f2 , f3 , f4 ) ; }
      // ======================================================================
      // monitoring 
      // ======================================================================
      static Cut __monitor__ ( const Cuts&         c , 
                               StatEntity*         e )
      { return LoKi::monitor ( c , e ) ; }
      // ======================================================================
      static Fun __monitor__ ( const Func&         c , 
                               StatEntity*         e )
      { return LoKi::monitor ( c , e ) ; }
      // ======================================================================
      static Fun __monitor__ ( const Func&         c , 
                               AIDA::IHistogram1D* h )
      { return LoKi::monitor ( c , h ) ; }
      // ======================================================================
      // Switch
      // ======================================================================
      static Fun __switch__   ( const Cuts&  cut  , 
                                const Func&  fun1 ,
                                const Func&  fun2 )
      { return LoKi::Switch<TYPE>       ( cut , fun1 , fun2 ) ; }
      // ======================================================================
      static Fun __switch__   ( const Cuts&  cut  , 
                                const Func&  fun1 ,
                                const double fun2 )
      { return LoKi::Switch<TYPE>       ( cut , fun1 , fun2 ) ; }
      // ======================================================================
      static Fun __switch__   ( const Cuts&  cut  , 
                                const double fun1 ,
                                const Func&  fun2 )
      { return Switch<TYPE>             ( cut , fun1 , fun2 ) ; }
      // ======================================================================
      static Fun __switch__   ( const Cuts&  cut  , 
                                const double fun1 ,
                                const double fun2 )
      { return LoKi::SimpleSwitch<TYPE> ( cut , fun1 , fun2 ) ; }
      // ======================================================================
      // EqualTo
      // ======================================================================
      static Cut __equal_to__ ( const Func&   fun  , 
                                const double  val  )
      { return LoKi::EqualToValue<TYPE> ( fun , val ) ; }
      // ======================================================================
      static Cut __equal_to__ ( const Func&   fun  , 
                                const Func&   fun1 ) { return fun == fun1 ; }
    } ;
    // ========================================================================
  } // end of namespace LoKi::Dicts
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_FUNCOPS_H
// ============================================================================
