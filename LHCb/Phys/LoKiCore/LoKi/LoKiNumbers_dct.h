// $Id$ 
// ============================================================================
#ifndef DICT_LOKINUMBER_DCT_H 
#define DICT_LOKINUMBER_DCT_H 1
// ============================================================================
// Include files
// ============================================================================
//  STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Dicts.h"
#include "LoKi/FuncOps.h"
#include "LoKi/BasicFunctors.h"
#include "LoKi/CoreTypes.h"
#include "LoKi/Random.h"
#include "LoKi/Scalers.h"
#include "LoKi/Filters.h"
#include "LoKi/Status.h"
// ============================================================================
/** @file
 *  Simple file to build the dictionaries for LoKi's functors, which 
 *  deal only with "plain numbers"
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
 *  @date   2007-12-01
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Functors 
  {
    // ========================================================================
    // the specific printout
    // ========================================================================
    template <>
    std::ostream& Empty<double>::fillStream
    ( std::ostream& s ) const { return s << "XEMPTY" ; }
    // ========================================================================
    // the specific printpout
    // ========================================================================
    template <>
    std::ostream& Size<double>::fillStream
    ( std::ostream& s ) const { return s << "XSIZE" ; }
    // ========================================================================
  } // end of namespace LoKi::Functors 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Dicts
  {
    // ========================================================================
    /** the explicit full template specialzation of  LoKi::Dicts::FunCalls
     *  for the case of "double" argument
     *  @see LoKi::Dicts::FunCalls
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-12-01
     */
    template <>
    class FunCalls<double> 
    {
    private:
      typedef double                                Type ;
      typedef LoKi::BasicFunctors<double>::Function Fun  ;
    public:
      // ======================================================================
      // __call__
      static Fun::result_type __call__ 
      ( const Fun& fun  , const double           o ) { return fun ( o ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const std::vector<double>& o ) 
      { return o >> fun  ; }
      // __rrshift__
      static Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const double           o ) { return fun ( o ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<double,double> __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,double>& o    ) { return fun >> o  ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<double,bool>   __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,bool>&   o    ) { return fun >> o  ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** the explicit full template specialzation of  LoKi::Dicts::CutCalls
     *  for the case of "double" argument
     *  @see LoKi::Dicts::CutCalls
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-12-01
     */
    template <>
    class CutCalls<double> 
    {
    private:
      typedef double                                 Type ;
      typedef LoKi::BasicFunctors<double>::Predicate Fun  ;
    public:
      //
      static Fun::result_type __call__ 
      ( const Fun& fun  , const double               o ) { return fun ( o ) ; }
      // ======================================================================
      static std::vector<double> __rrshift__ 
      ( const Fun& fun  , const std::vector<double>& o ) 
      { return o >> fun  ; }
      static Fun::result_type  __rrshift__ 
      ( const Fun& fun  , const double               o ) { return fun ( o ) ; }
      // ======================================================================
      static LoKi::FunctorFromFunctor<double,bool> __rshift__            
      ( const Fun& fun  , const Fun&                 o ) { return fun >> o  ; }
      //
    } ;
    // ========================================================================
    /** the explicit full template specialzation of  LoKi::Dicts::FunCalls
     *  for the case of "std::vector<double>" argument
     *  @see LoKi::Dicts::FunCalls
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-12-01
     */
    template <>
    class FunCalls<std::vector<double> > 
    {
    private:
      typedef std::vector<double>                 Type ;
      typedef LoKi::BasicFunctors<Type>::Function Fun  ;
    public:
      //
      static double __call__ 
      ( const Fun& fun  , const Type&  o ) { return fun ( o ) ; }
      static double __call__ 
      ( const Fun& fun  , const double o ) 
      { return fun ( std::vector<double> ( 1 , o ) ) ; }
      // ======================================================================
      static double __rrshift__ 
      ( const Fun& fun  , const std::vector<double>& o ) 
      { return fun ( o )   ; }
      static double __rrshift__ 
      ( const Fun& fun  , const double           o ) 
      { return fun ( std::vector<double> ( 1 , o ) ) ; }
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,double> __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,double>& o    ) { return fun >> o  ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<Type,bool>   __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,bool>&   o    ) { return fun >> o  ; }
      // ======================================================================
    } ;
    // ========================================================================
    /** the explicit full template specialzation of  LoKi::Dicts::CutCalls
     *  for the case of "std::vector<double>" argument
     *  @see LoKi::Dicts::CutCalls
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-12-01
     */
    template <>
    class CutCalls<std::vector<double> > 
    {
    private:
      typedef std::vector<double>                  Type ;
      typedef LoKi::BasicFunctors<Type>::Predicate Fun  ;
    public:
      //
      static bool __call__ 
      ( const Fun& fun  , const Type&  o ) { return fun ( o ) ; }
      static bool __call__ 
      ( const Fun& fun  , const double o ) 
      { return fun ( std::vector<double> ( 1 , o ) ) ; }
      // ======================================================================
      static bool __rrshift__ 
      ( const Fun& fun  , const std::vector<double>& o ) 
      { return fun ( o ) ; }
      static bool __rrshift__ 
      ( const Fun& fun  , const double           o ) 
      { return fun ( std::vector<double> ( 1 , o ) ) ; }
      // ======================================================================
      static LoKi::FunctorFromFunctor<std::vector<double>,bool> __rshift__            
      ( const Fun& fun  , const Fun&             o ) { return fun >> o  ; }
    } ;
    // ========================================================================
    /** the explicit full template specialization of  LoKi::Dicts::PipeOps
     *  for the case of "std::vector<double>" argument
     *  @see LoKi::Dicts::PipeOps
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-12-01
     */
    template <>
    class  PipeOps<double,double>
    {
    private:
      // ======================================================================
      typedef double TYPE  ;
      typedef double TYPE2 ;
      typedef LoKi::BasicFunctors<TYPE>::Pipe          Pipe    ;
      typedef LoKi::BasicFunctors<TYPE>::CutVal        CutVal  ;
      typedef LoKi::BasicFunctors<TYPE2>::Function     Func    ;
      typedef LoKi::BasicFunctors<TYPE2>::Predicate    Cuts    ;      
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__ 
      ( const Pipe& fun , const Pipe&      fun2 ) 
      { return fun >>                      fun2 ; }
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,bool>
      __rshift__ 
      ( const Pipe& fun , const CutVal&    fun2 ) 
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
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__ 
      static std::vector<TYPE>
      __rrshift__ ( const Pipe& fun , const std::vector<TYPE>& val ) 
      { return fun ( val ) ; }
      // __rrshift__ 
      static std::vector<TYPE>
      __rrshift__ ( const Pipe& fun , const double val ) 
      { return fun ( std::vector<double>( 1 , val ) ) ; }
      // ======================================================================
    public : // add the dumps
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__ 
      ( const  Pipe& fun , const LoKi::Dump& dump  ) 
      { return fun >> LoKi::Functors::Dump_<TYPE> ( dump ) ; }
      // ======================================================================
    public : // get the first N
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__ 
      ( const  Pipe& fun , const LoKi::FirstN& fn  ) 
      { return fun >> LoKi::Functors::FirstN_<TYPE> ( fn ) ; }
      // ======================================================================
    public : // reverse 
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__ 
      ( const  Pipe& fun , const LoKi::Reverse& /* fn */ ) 
      { return fun >> LoKi::Functors::Reverse_<TYPE> () ; }
      // ======================================================================
    public:
      // ======================================================================
      // __tee__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> > 
      __tee__     ( const Pipe& fun ) 
      { return LoKi::tee<TYPE>( fun ) ; }        
      // ======================================================================
    public:
      // ======================================================================
      // _union_ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __union__          ( const Pipe& fun  , const Pipe& fun2 ) 
      { return LoKi::Functors::Union         <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
      // _union_ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __union__          ( const Pipe& fun  , const Cuts& fun2 ) 
      { return LoKi::Functors::Union         <std::vector<TYPE>,TYPE>
          ( fun , LoKi::filter<TYPE> ( fun2 ) ) ; }
      // _intersection_ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __intersection__   ( const Pipe& fun  , const Pipe& fun2 ) 
      { return LoKi::Functors::Intersection  <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
      // _intersection_ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __intersection__   ( const Pipe& fun  , const Cuts& fun2 ) 
      { return LoKi::Functors::Intersection  <std::vector<TYPE>,TYPE>
          ( fun , LoKi::filter<TYPE> ( fun2 ) ) ; }
      // _difference_ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __difference__     ( const Pipe& fun  , const Pipe& fun2 ) 
      { return LoKi::Functors::Difference    <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
      // _difference_ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __difference__     ( const Pipe& fun  , const Cuts& fun2 ) 
      { return LoKi::Functors::Difference    <std::vector<TYPE>,TYPE>
          ( fun , LoKi::filter<TYPE> ( fun2 ) ) ; }
      // _sym_difference_ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __sym_difference__ ( const Pipe& fun  , const Pipe& fun2 ) 
      { return LoKi::Functors::SymDifference <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
      // _sym_difference_ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __sym_difference__ ( const Pipe& fun  , const Cuts& fun2 ) 
      { return LoKi::Functors::SymDifference <std::vector<TYPE>,TYPE>
          ( fun , LoKi::filter<TYPE> ( fun2 ) ) ; }
      // _includes_ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,bool>
      __includes__       ( const Pipe& fun  , const Pipe& fun2 ) 
      { return LoKi::Functors::Includes <std::vector<TYPE>,TYPE>( fun , fun2 ) ; }
      // _includes_ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,bool>
      __includes__       ( const Pipe& fun  , const Cuts& fun2 ) 
      { return LoKi::Functors::Includes <std::vector<TYPE>,TYPE>
          ( fun , LoKi::filter<TYPE>  ( fun2 ) ) ; }
      // ======================================================================
    };
    // ========================================================================
    /** the explicit full template specialization of  LoKi::Dicts::FunValOps
     *  for the case of "std::vector<double>" argument
     *  @see LoKi::Dicts::FunValOps
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-12-01
     */
    template <>
    class  FunValOps<double> 
    {
    private:
      // ======================================================================
      typedef double TYPE ;
      typedef LoKi::BasicFunctors<TYPE>::FunVal         FunVal  ;
      typedef LoKi::BasicFunctors<TYPE>::Function       Func    ;
      typedef LoKi::BasicFunctors<TYPE>::Predicate      Cuts    ;      
      // ======================================================================
    public:
      // ======================================================================
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
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__ 
      static double 
      __rrshift__ ( const FunVal& fun , const std::vector<TYPE>& val ) 
      { return fun ( val  )  ; }
      // __rrshift__ 
      static double 
      __rrshift__ ( const FunVal& fun , 
                    const double val ) 
      { return fun ( std::vector<double>( 1 , val ) ) ; }
      // ======================================================================
    public:
      // ======================================================================
      // __tee__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> > 
      __tee__     ( const FunVal& fun ) 
      { return LoKi::tee<TYPE>( fun ) ; }        
      // ======================================================================
    };
    // ========================================================================
    /** @class SourceOps
     *  Wrapper class for operations with 'source'-functors
     *  @see LoKi::BasicFunctors 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-11-30
     */
    template <>
    class SourceOps<double,double> 
    {
    private:      
      // ======================================================================
      typedef double TYPE  ;
      typedef double TYPE2 ;
      typedef LoKi::BasicFunctors<TYPE>::Source        Source ;
      typedef LoKi::BasicFunctors<TYPE>::Pipe          Pipe    ;
      typedef LoKi::BasicFunctors<TYPE>::Map           Map     ;
      typedef LoKi::BasicFunctors<TYPE>::FunVal        FunVal  ;
      typedef LoKi::BasicFunctors<TYPE>::CutVal        CutVal  ;
      typedef LoKi::BasicFunctors<TYPE2>::Function     Func    ;
      typedef LoKi::BasicFunctors<TYPE2>::Predicate    Cuts    ;      
      // ======================================================================
    public:
      // ======================================================================
      // __call__
      static Source::result_type __call__ 
      ( const Source& fun ) { return fun() ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__
      static Source::result_type __rshift__ 
      ( const Source& fun , std::vector<TYPE>& res ) 
      { res = fun() ; return res ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const Source& fun , const Pipe&    fun2 ) 
      { return fun >>                      fun2 ; }
      // degenerated: __rshift__
      // static LoKi::FunctorFromFunctor<void,std::vector<double> >
      //__rshift__ 
      //( const Source& fun , const Map&     fun2 ) 
      //{ return fun >>                      fun2 ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,double>
      __rshift__ 
      ( const Source& fun , const FunVal&  fun2 ) 
      { return fun >>                      fun2 ; }
      // __rshift__
      static LoKi::FunctorFromFunctor<void,bool>
      __rshift__ 
      ( const Source& fun , const CutVal&  fun2 ) 
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
      // ======================================================================
    public : // add the dumps
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const  Source& fun , const LoKi::Dump& dump  ) 
      { return fun >> LoKi::Functors::Dump_<TYPE> ( dump ) ; }
      // ======================================================================
    public : // get the first N
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const  Source& fun , const LoKi::FirstN& fn  ) 
      { return fun >> LoKi::Functors::FirstN_<TYPE> ( fn ) ; }
      // ======================================================================
    public : // reverse 
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __rshift__ 
      ( const  Source& fun , const LoKi::Reverse& /* fn  */ ) 
      { return fun >> LoKi::Functors::Reverse_<TYPE> () ; }
      // ======================================================================
    public:
      // ======================================================================
      // _union_ 
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __union__          ( const Source& fun  , const Source& fun2 ) 
      { return LoKi::Functors::Union         <void,TYPE>( fun , fun2 ) ; }
      // _intersection_ 
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __intersection__   ( const Source& fun  , const Source& fun2 ) 
      { return LoKi::Functors::Intersection  <void,TYPE>( fun , fun2 ) ; }
      // _difference_ 
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __difference__     ( const Source& fun  , const Source& fun2 ) 
      { return LoKi::Functors::Difference    <void,TYPE>( fun , fun2 ) ; }
      // _sym_difference_ 
      static LoKi::FunctorFromFunctor<void,std::vector<TYPE> >
      __sym_difference__ ( const Source& fun  , const Source& fun2 ) 
      { return LoKi::Functors::SymDifference <void,TYPE>( fun , fun2 ) ; }
      // _includes_ 
      static LoKi::FunctorFromFunctor<void,bool>
      __includes__       ( const Source& fun  , const Source& fun2 ) 
      { return LoKi::Functors::Includes      <void,TYPE>( fun , fun2 ) ; }
      // ======================================================================
    };    
    // ========================================================================
    /** the explicit full template specialzation of  LoKi::Dicts::FunCalls
     *  for the case of "void" argument
     *  @see LoKi::Dicts::FunCalls
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-12-01
     */
    template <>
    class FunCalls<void> 
    {
    private:
      typedef LoKi::BasicFunctors<void>::Function Fun  ;
    public:
      // ======================================================================
      // __call__
      static Fun::result_type __call__ ( const Fun& fun  ) { return fun () ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<void,double> __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,double>& o    ) { return fun >> o  ; }
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<void,bool> __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<double,bool>&   o    ) { return fun >> o  ; }
      // ======================================================================
      // __rshift__
      static std::vector<double>& __rshift__            
      ( const Fun&           fun    , 
        std::vector<double>& result ) 
      {
        std::generate  
          ( result.begin () , 
            result.end   () , 
            Apply<void,double> ( &fun ) ) ;
        return result ;
      }
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__
      static std::vector<double> __rrshift__ 
      ( const Fun&         fun , 
        const unsigned int num ) 
      {
        std::vector<double> result ( num ) ;
        std::generate  
          ( result.begin () , 
            result.end   () , 
            Apply<void,double> ( &fun ) ) ;
        return result ;
      }
      // ======================================================================
    } ;
    // ========================================================================
    /** the explicit full template specialization of  LoKi::Dicts::CutCalls
     *  for the case of "void" argument
     *  @see LoKi::Dicts::CutCalls
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-12-01
     */
    template <>
    class CutCalls<void> 
    {
    private:
      // ======================================================================
      typedef LoKi::BasicFunctors<void>::Predicate Fun  ;
      // ======================================================================
    public:
      // ======================================================================
      // __call__
      static Fun::result_type __call__ ( const Fun& fun  ) { return fun () ; }
      // ======================================================================
    public:
      // ======================================================================
      // __rshift__
      static LoKi::FunctorFromFunctor<void,bool> __rshift__            
      ( const Fun&                          fun  , 
        const LoKi::Functor<void,bool>&     o    ) { return fun >> o  ; }
      // __rshift__
      static std::vector<bool>& __rshift__            
      ( const Fun&         fun    , 
        std::vector<bool>& result ) 
      {
        std::generate  
          ( result.begin () , 
            result.end   () , 
            Apply<void,bool>( &fun) ) ;
        return result ;
      }
      // ======================================================================
    public:
      // ======================================================================
      // __rrshift__
      static std::vector<bool> __rrshift__ 
      ( const Fun&         fun , 
        const unsigned int num ) 
      {
        std::vector<bool> result ( num ) ;
        std::generate  
          ( result.begin () , 
            result.end   () , 
            Apply<void,bool> ( &fun ) ) ;
        return result ;
      }
      // ======================================================================      
    } ;
    // ========================================================================
    template <>
    class FuncOps<void,void>: public FuncOps_<void> {} ;
    // ========================================================================
    template <>
    class CutsOps<void,void>: public CutsOps_<void> {} ;
    // ========================================================================
  } // end of namespace LoKi::Dicts 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
/// specify the proper printout for identity 
// ============================================================================
namespace
{
  // ==========================================================================
  struct _Instantiations 
  {
    // ========================================================================
    /// function 
    LoKi::FunctorFromFunctor<double,double>                            m_001 ;
    /// predicate 
    LoKi::FunctorFromFunctor<double,bool>                              m_002 ;    
    /// degeneracy: map and pipe 
    LoKi::FunctorFromFunctor<std::vector<double>,std::vector<double> > m_003 ;
    /// degeneracy: element and fun-value 
    LoKi::FunctorFromFunctor<std::vector<double>,double>               m_004 ;    
    /// unknown
    LoKi::FunctorFromFunctor<std::vector<double>,bool>                 m_005 ;    
    ///
    LoKi::Dicts::FuncOps<double>                                       m_op01 ;
    LoKi::Dicts::CutsOps<double>                                       m_op02 ;
    LoKi::Dicts::FuncOps<std::vector<double> >                         m_op03 ;
    LoKi::Dicts::CutsOps<std::vector<double> >                         m_op04 ;
    ///
    LoKi::Dicts::PipeOps<double,double>                                m_ex01 ;
    LoKi::Dicts::FunValOps<double>                                     m_ex02 ;
    LoKi::Dicts::CutValOps<double>                                     m_ex03 ;
    LoKi::Dicts::SourceOps<double,double>                              m_ex04 ;
    ///
    LoKi::Dicts::FunCalls<double>                                      m_cs01 ;
    LoKi::Dicts::CutCalls<double>                                      m_cs02 ;
    LoKi::Dicts::FunCalls<std::vector<double> >                        m_cs03 ;
    LoKi::Dicts::CutCalls<std::vector<double> >                        m_cs04 ;
    ///
    LoKi::Constant<double,double>                                      m_co01 ;
    LoKi::Constant<double,bool>                                        m_co02 ;
    LoKi::Constant<std::vector<double>,double>                         m_co03 ;
    LoKi::Constant<std::vector<double>,bool>                           m_co04 ;
    /// 
    LoKi::Identity<double>                                             m_id01 ;
    LoKi::Identity<std::vector<double> >                               m_id02 ;
    // trivia 
    LoKi::Functors::Empty<double>                                      m_e    ;
    LoKi::Functors::Size<double>                                       m_s    ;
    LoKi::Functors::FakeSource<double>                                 m_f    ;
    // ========================================================================
    // function& predicate  
    LoKi::FunctorFromFunctor<void,double>                              m_101  ;
    LoKi::FunctorFromFunctor<void,bool>                                m_102  ;    
    // constant 
    LoKi::Constant<void,double>                                        m_co11 ;
    LoKi::Constant<void,bool>                                          m_co12 ;    
    // operations:
    LoKi::Dicts::FuncOps<void>                                         m_op11 ;
    LoKi::Dicts::CutsOps<void>                                         m_op12 ;
    // calls 
    LoKi::Dicts::FunCalls<void>                                        m_cs11 ;
    LoKi::Dicts::CutCalls<void>                                        m_cs12 ;    
    // ========================================================================
    /// fictive non-existent constructor 
    _Instantiations() ;
    // ========================================================================
  }; 
  // ==========================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // DICT_LOKINUMBERS_DCT_H
// ============================================================================
