// $Id: FuncOps.h,v 1.16 2008-10-19 16:11:40 ibelyaev Exp $
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
#include  "LoKi/BasicFunctors.h"
#include  "LoKi/Filters.h"
#include  "LoKi/compose.h"
#include  "LoKi/ExtraInfo.h"
// ============================================================================
namespace LoKi
{
  namespace Dicts
  {
    // ========================================================================
    /** @class FuncOps_
     *  Wrapper class for operations with functions 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-10-07
     */
    template <class TYPE, class TYPE2=TYPE>
    class FuncOps_ 
    {
    private:
      typedef typename LoKi::BasicFunctors<TYPE>::Function               Func ;
      typedef typename LoKi::BasicFunctors<TYPE>::Predicate              Cuts ;
      typedef typename LoKi::BasicFunctors<TYPE>::FunctionFromFunction   Fun  ;
      typedef typename LoKi::BasicFunctors<TYPE>::PredicateFromPredicate Cut  ;
    public:   
      // ======================================================================
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
      static Cut __eq__     ( const Func&  fun1 , 
                              const int    fun2 ) { return fun1 == fun2 ; }
      static Cut __eq__     ( const Func&  fun1 , 
                              const unsigned int fun2 ) { return fun1 == fun2 ; }
      static Cut __ne__     ( const Func&  fun1 , 
                              const Func&  fun2 ) { return fun1 != fun2 ; }
      static Cut __ne__     ( const Func&  fun1 , 
                              const double fun2 ) { return fun1 != fun2 ; }
      static Cut __ne__     ( const Func&  fun1 , 
                              const int    fun2 ) { return fun1 != fun2 ; }
      static Cut __ne__     ( const Func&  fun1 , 
                              const unsigned int fun2 ) { return fun1 != fun2 ; }
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
      // ======================================================================
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
      // ======================================================================
      static Fun __min__    ( const Func&  f1   , 
                              const Func&  f2   ) 
      { return    LoKi::min ( f1 , f2 ) ; }
      // min
      static Fun __min__    ( const Func&  f1   , 
                              const double f2   ) 
      { return    LoKi::min ( f1 , f2 ) ; }
      // min
      static Fun __min__    ( const Func&  f1   , 
                              const Func&  f2   , 
                              const Func&  f3   ) 
      { return    LoKi::min ( f1 , f2 , f3 ) ; }
      // min
      static Fun __min__    ( const Func&  f1   , 
                              const Func&  f2   , 
                              const Func&  f3   , 
                              const Func&  f4   ) 
      { return    LoKi::min ( f1 , f2 , f3 , f4 ) ; }      
      // ======================================================================
      // max
      // ======================================================================
      static Fun __max__    ( const Func&  f1   , 
                              const Func&  f2   ) 
      { return    LoKi::max ( f1 , f2 ) ; }
      static Fun __max__    ( const Func&  f1   , 
                              const double f2   ) 
      { return    LoKi::max ( f1 , f2 ) ; }
      static Fun __max__    ( const Func&  f1   , 
                              const Func&  f2   , 
                              const Func&  f3   ) 
      { return    LoKi::max ( f1 , f2 , f3 ) ; }
      static Fun __max__    ( const Func&  f1   , 
                              const Func&  f2   , 
                              const Func&  f3   , 
                              const Func&  f4   ) 
      { return    LoKi::max ( f1 , f2 , f3 , f4 ) ; }
      // ======================================================================
      // monitoring 
      // ======================================================================
      static Fun __monitor__ ( const Func&                c ,
                               const std::string& s = "\n"  , 
                               const std::string& p = ""    ) 
      { return LoKi::print ( c , std::cout , s , p ) ; }        
      static Fun __monitor__ ( const Func&              c , 
                               StatEntity*              e ) 
      { return LoKi::monitor ( c , e ) ; }
      static Fun __monitor__ ( const Func&              c , 
                               AIDA::IHistogram1D*      h ) 
      { return LoKi::monitor ( c , h ) ; }
      static Fun __monitor__ ( const Func&              c , 
                               const std::string&       p , 
                               const Gaudi::Histo1DDef& h )
      { return LoKi::plot    ( c , p , h ) ; }
      static Fun __monitor__ ( const Func&              c , 
                               const std::string&       d , 
                               const std::string&       i , 
                               const Gaudi::Histo1DDef& h )
      { return LoKi::plot    ( c , d , i , h ) ; }
      static Fun __monitor__ ( const Func&              c , 
                               const std::string&       d , 

                               const int                i , 
                               const Gaudi::Histo1DDef& h )
      { return LoKi::plot    ( c , d , i , h ) ; }
      // ======================================================================
      // EqualTo
      // ======================================================================
      static Cut __equal_to__ ( const Func&   fun  , 
                                const double  val  )
      { return LoKi::EqualToValue<TYPE> ( fun , val ) ; }
      static Cut __equal_to__ ( const Func&   fun  , 
                                const Func&   fun1 ) { return fun == fun1 ; }
    } ;
    // ========================================================================
    /** @class FuncOps
     *  Wrapper class for operations with functions 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-10-07
     */
    template <class TYPE, class TYPE2=TYPE>
    class FuncOps : public FuncOps_<TYPE>
    {
    private:
      typedef typename LoKi::BasicFunctors<TYPE>::Function               Func ;
      typedef typename LoKi::BasicFunctors<TYPE>::Predicate              Cuts ;
      typedef typename LoKi::BasicFunctors<TYPE>::FunctionFromFunction   Fun  ;
      typedef typename LoKi::BasicFunctors<TYPE>::PredicateFromPredicate Cut  ;
    public:   
      // ======================================================================
      // functional part 
      // ======================================================================
      // yields/map 
      static LoKi::FunctorFromFunctor<std::vector<TYPE2>,std::vector<double> > 
      __yields__          ( const Func& fun ) 
      { return LoKi::yields<TYPE2>  ( fun ) ; }
      // process__  
      static LoKi::FunctorFromFunctor<std::vector<TYPE2>,std::vector<TYPE2> >
      __process__         ( const Func& fun ) 
      { return LoKi::process<TYPE2> ( fun ) ; }
      // min value  
      static LoKi::FunctorFromFunctor<std::vector<TYPE2>,double>
      __min_value__       ( const Func& fun ) 
      { return LoKi::min_value<TYPE2>     ( fun ) ; }
      // abs min value  
      static LoKi::FunctorFromFunctor<std::vector<TYPE2>,double>
      __min_abs_value__   ( const Func& fun ) 
      { return LoKi::min_abs_value<TYPE2>  ( fun ) ; }
      // maximal value  
      static LoKi::FunctorFromFunctor<std::vector<TYPE2>,double>
      __max_value__       ( const Func& fun ) 
      { return LoKi::max_value<TYPE2>     ( fun ) ; }
      // abs max value 
      static LoKi::FunctorFromFunctor<std::vector<TYPE2>,double>
      __max_abs_value__   ( const Func& fun ) 
      { return LoKi::max_abs_value<TYPE2>  ( fun ) ; }
      // min element 
      static LoKi::FunctorFromFunctor<std::vector<TYPE2>,TYPE2>
      __min_element__     ( const Func& fun ) 
      { return LoKi::min_element<TYPE2>( fun ) ; }
      // abs min element 
      static LoKi::FunctorFromFunctor<std::vector<TYPE2>,TYPE2>
      __min_abs_element__ ( const Func& fun ) 
      { return LoKi::min_abs_element<TYPE2>( fun ) ; }      
      // max element 
      static LoKi::FunctorFromFunctor<std::vector<TYPE2>,TYPE2>
      __max_element__     ( const Func& fun ) 
      { return LoKi::max_element<TYPE2>( fun ) ; }
      // abs min element 
      static LoKi::FunctorFromFunctor<std::vector<TYPE2>,TYPE2>
      __max_abs_element__ ( const Func& fun ) 
      { return LoKi::max_abs_element<TYPE2>( fun ) ; }
    } ;
    // ========================================================================
    /** @class CutsOps_
     *  Wrapper class for operations with predicates  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-10-07
     */
    template <class TYPE, class TYPE2=TYPE>
    class CutsOps_ 
    {
    private:
      typedef typename LoKi::BasicFunctors<TYPE>::Function               Func ;
      typedef typename LoKi::BasicFunctors<TYPE>::Predicate              Cuts ;
      typedef typename LoKi::BasicFunctors<TYPE>::FunctionFromFunction   Fun  ;
      typedef typename LoKi::BasicFunctors<TYPE>::PredicateFromPredicate Cut  ;
    public:   
      // ======================================================================
      static Cut __or__       ( const Cuts&  cut1 , 
                                const Cuts&  cut2 ) { return cut1 || cut2 ; }
      static Cut __and__      ( const Cuts&  cut1 , 
                                const Cuts&  cut2 ) { return cut1 && cut2 ; }
      static Cut __invert__   ( const Cuts&  cut  ) { return !cut ; }
      //
      static Cut __monitor__  ( const Cuts&        c        ,
                                const std::string& s = "\n" , 
                                const std::string& p = ""   ) 
      { return LoKi::print ( c , std::cout , s , p ) ; }        
      //
      static Cut __monitor__  ( const Cuts&         c , 
                                StatEntity*         e )
      { return LoKi::monitor  ( c , e ) ; }
      //
      static Fun __switch__   ( const Cuts&  cut  , 
                                const Func&  fun1 ,
                                const Func&  fun2 )
      { return LoKi::Switch<TYPE>       ( cut , fun1 , fun2 ) ; }
      static Fun __switch__   ( const Cuts&  cut  , 
                                const Func&  fun1 ,
                                const double fun2 )
      { return LoKi::Switch<TYPE>       ( cut , fun1 , fun2 ) ; }
      static Fun __switch__   ( const Cuts&  cut  , 
                                const double fun1 ,
                                const Func&  fun2 )
      { return Switch<TYPE>             ( cut , fun1 , fun2 ) ; }
      // switch 
      static Fun __switch__   ( const Cuts&  cut  , 
                                const double fun1 ,
                                const double fun2 )
      { return LoKi::SimpleSwitch<TYPE> ( cut , fun1 , fun2 ) ; }
      // switch 
      static Cut __switch__   ( const Cuts&  cut  , 
                                const Cuts&  cut1 ,
                                const Cuts&  cut2 )
      { return LoKi::Switch<TYPE,bool> ( cut , cut1 , cut2 ) ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CutsOps
     *  Wrapper class for operations with predicates  
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-10-07
     */
    template <class TYPE, class TYPE2=TYPE>
    class CutsOps : public CutsOps_<TYPE> 
    {
    private:
      typedef typename LoKi::BasicFunctors<TYPE>::Function               Func ;
      typedef typename LoKi::BasicFunctors<TYPE>::Predicate              Cuts ;
      typedef typename LoKi::BasicFunctors<TYPE>::FunctionFromFunction   Fun  ;
      typedef typename LoKi::BasicFunctors<TYPE>::PredicateFromPredicate Cut  ;
    public:   
      // ======================================================================
      // functional part 
      // ======================================================================
      // filter/select 
      static LoKi::FunctorFromFunctor<std::vector<TYPE2>,std::vector<TYPE2> >
      __select__  ( const Cuts& cut ) 
      { return LoKi::select<TYPE2>  ( cut ) ; }
      // process
      static LoKi::FunctorFromFunctor<std::vector<TYPE2>,std::vector<TYPE2> >
      __process__ ( const Cuts& cut ) 
      { return LoKi::process<TYPE2> ( cut ) ; }
      // count 
      static LoKi::FunctorFromFunctor<std::vector<TYPE2>,double >
      __count__ ( const Cuts& cut ) 
      { return LoKi::count<TYPE2,TYPE,bool> ( cut ) ; }
      // has
      static LoKi::FunctorFromFunctor<std::vector<TYPE2>,bool>
      __has__ ( const Cuts& cut ) 
      { return LoKi::has<TYPE2,TYPE,bool> ( cut ) ; }
    } ;
    // ========================================================================
    /** @class MapsOps
     *  Wrapper class for operations with mapping functors
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-11-30
     */
    template <class TYPE>
    class  MapsOps 
    {
    private:
      typedef typename LoKi::BasicFunctors<TYPE>::Map                    Map;
    public:
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<double> >
      __rshift__ 
      ( const Map& fun , const LoKi::BasicFunctors<double>::Map&    fun2 ) 
      { return fun >> fun2 ; }
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,double>
      __rshift__ 
      ( const Map& fun , const LoKi::BasicFunctors<double>::FunVal& fun2 ) 
      { return fun >> fun2 ; }
    public:
      // __rrshift__ 
      static std::vector<double>
      __rrshift__ ( const Map& fun , const std::vector<TYPE>& val ) 
      { return fun ( val ) ; }
      // __rrshift__ 
      static std::vector<double>
      __rrshift__ ( const Map& fun , 
                    const typename std::vector<TYPE>::value_type& val ) 
      { return fun ( std::vector<TYPE>( 1 , val ) ) ; }
    public:
      // __tee__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> > 
      __tee__     ( const Map& fun ) 
      { return LoKi::tee<TYPE>( fun ) ; }        
    };
    // ========================================================================
    /** @class PipeOps
     *  Wrapper class for operations with mapping functors
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-11-30
     */
    template <class TYPE, class TYPE2=TYPE>
    class  PipeOps 
    {
    private:
      typedef typename LoKi::BasicFunctors<TYPE>::Pipe          Pipe    ;
      typedef typename LoKi::BasicFunctors<TYPE>::Map           Map     ;
      typedef typename LoKi::BasicFunctors<TYPE>::Element       Element ;
      typedef typename LoKi::BasicFunctors<TYPE>::FunVal        FunVal  ;
      typedef typename LoKi::BasicFunctors<TYPE2>::Function     Func    ;
      typedef typename LoKi::BasicFunctors<TYPE2>::Predicate    Cuts    ;      
    public:
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<double> >
      __rshift__ 
      ( const Pipe& fun , const Map&       fun2 ) 
      { return fun >>                      fun2 ; }
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__ 
      ( const Pipe& fun , const Pipe&      fun2 ) 
      { return fun >>                      fun2 ; }
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,double>
      __rshift__ 
      ( const Pipe& fun , const FunVal&    fun2 ) 
      { return fun >>                      fun2 ; }
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,TYPE>
      __rshift__ 
      ( const Pipe& fun , const Element&   fun2 ) 
      { return fun >>                      fun2 ; }
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__ 
      ( const Pipe& fun , const Cuts&      fun2 ) 
      { return fun >> LoKi::filter<TYPE> ( fun2 ) ; }
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<double> >
      __rshift__ 
      ( const Pipe& fun , const Func&      fun2 ) 
      { return fun >> LoKi::yields<TYPE> ( fun2 ) ; }
    public:
      // __rrshift__ 
      static std::vector<TYPE>
      __rrshift__ ( const Pipe& fun , const std::vector<TYPE>& val ) 
      { return fun ( val ) ; }
      // __rrshift__ 
      static std::vector<TYPE>
      __rrshift__ ( const Pipe& fun , 
                    const typename std::vector<TYPE>::value_type& val ) 
      { return fun ( std::vector<TYPE>( 1 , val ) ) ; }
    public:
      // __tee__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> > 
      __tee__     ( const Pipe& fun ) 
      { return LoKi::tee<TYPE>( fun ) ; }        
    };
    // ========================================================================
    /** @class FunValOps
     *  Wrapper class for operations with 'funcion-value'-functors
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-11-30
     */
    template <class TYPE>
    class  FunValOps 
    {
    private:
      typedef typename LoKi::BasicFunctors<TYPE>::FunVal         FunVal  ;
      typedef typename LoKi::BasicFunctors<double>::Function     Func    ;
      typedef typename LoKi::BasicFunctors<double>::Predicate    Cuts    ;      
    public:
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,double>
      __rshift__ 
      ( const FunVal& fun , const Func&    fun2 )  
      { return fun >>                      fun2 ; }
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,bool>
      __rshift__ 
      ( const FunVal& fun , const Cuts&    fun2 )  
      { return fun >>                      fun2 ; }
    public:
      // __rrshift__ 
      static double 
      __rrshift__ ( const FunVal& fun , const std::vector<TYPE>& val ) 
      { return fun ( val ) ; }
      // __rrshift__ 
      static double 
      __rrshift__ ( const FunVal& fun , 
                    const typename std::vector<TYPE>::value_type& val ) 
      { return fun ( std::vector<TYPE>( 1 , val ) ) ; }
    public:
      // __tee__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> > 
      __tee__     ( const FunVal& fun ) 
      { return LoKi::tee<TYPE>( fun ) ; }        
    };
    // ========================================================================
    /** @class ElementOps
     *  Wrapper class for operations with 'element-selection'-functors
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-11-30
     */
    template <class TYPE, class TYPE2=TYPE>
    class  ElementOps 
    {
    private:
      typedef typename LoKi::BasicFunctors<TYPE>::Element       Element ;
      typedef typename LoKi::BasicFunctors<TYPE2>::Function     Func    ;
      typedef typename LoKi::BasicFunctors<TYPE2>::Predicate    Cuts    ;      
    public:
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,double>
      __rshift__ 
      ( const Element& fun , const Func&   fun2 ) 
      { return LoKi::Compose<std::vector<TYPE>,TYPE,double,TYPE2>( fun ,fun2 ) ; }
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,bool>
      __rshift__ 
      ( const Element& fun , const Cuts&   fun2 ) 
      { return LoKi::Compose<std::vector<TYPE>,TYPE,bool,TYPE2>( fun ,fun2 ) ; }
    public:
      // __rrshift__ 
      static TYPE
      __rrshift__ ( const Element& fun , const std::vector<TYPE>& val ) 
      { return fun ( val ) ; }
      // __rrshift__ 
      static TYPE
      __rrshift__ ( const Element& fun , 
                    const typename std::vector<TYPE>::value_type& val ) 
      { return fun ( std::vector<TYPE>( 1 , val ) ) ; }
    public:
      // __tee__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> > 
      __tee__     ( const Element& fun ) 
      { return LoKi::tee<TYPE>( fun ) ; }        
    };
    // ========================================================================
    /** @class SourceOps
     *  Wrapper class for operations with 'source'-functors
     *  @see LoKi::BasicFunctors 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-11-30
     */
    template <class TYPE, class TYPE2=TYPE>
    class SourceOps 
    {
    private:
      typedef typename LoKi::BasicFunctors<TYPE>::Source        Source ;
      typedef typename LoKi::BasicFunctors<TYPE>::Pipe          Pipe    ;
      typedef typename LoKi::BasicFunctors<TYPE>::Map           Map     ;
      typedef typename LoKi::BasicFunctors<TYPE>::Element       Element ;
      typedef typename LoKi::BasicFunctors<TYPE>::FunVal        FunVal  ;
      typedef typename LoKi::BasicFunctors<TYPE2>::Function     Func    ;
      typedef typename LoKi::BasicFunctors<TYPE2>::Predicate    Cuts    ;      
    public:
      // __call__
      static typename Source::result_type __call__ 
      ( const Source& fun ) { return fun() ; }
    public:
      // __rshift__
      static typename Source::result_type __rshift__ 
      ( const Source& fun , std::vector<TYPE>& res ) 
      { res = fun() ; return res ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const Source& fun , const Pipe&    fun2 ) 
      { return fun >>                      fun2 ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<double> >
      __rshift__ 
      ( const Source& fun , const Map&     fun2 ) 
      { return fun >>                      fun2 ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,double>
      __rshift__ 
      ( const Source& fun , const FunVal&  fun2 ) 
      { return fun >>                      fun2 ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const Source& fun , const Cuts&    fun2 ) 
      { return fun >> LoKi::filter<TYPE> ( fun2 ) ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<double> >
      __rshift__ 
      ( const Source& fun , const Func&    fun2 ) 
      { return fun >> LoKi::yields<TYPE> ( fun2 ) ; }
    };
    // ========================================================================
    template <class TYPE>
    class InfoOps
    {
    public:
      // ======================================================================
      // __info__ 
      static LoKi::FunctorFromFunctor<TYPE,double>
      __info__ ( const LoKi::Functor<TYPE,double>& fun , 
                 const int  index                      , 
                 const bool update                     ) 
      { return LoKi::info ( index , fun , update ) ; }
      // __info__ 
      static LoKi::FunctorFromFunctor<TYPE,double>
      __info__ ( const LoKi::Functor<TYPE,double>& fun , 
                 const int  index                      ) 
      { return LoKi::info ( index , fun          ) ; }
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Dicts
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_FUNCOPS_H
// ============================================================================
