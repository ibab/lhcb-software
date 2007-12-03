// $Id: LoKiNumbersDict.h,v 1.2 2007-12-03 12:31:08 ibelyaev Exp $
// ============================================================================
#ifndef DICT_LOKINUMBERSDICT_H 
#define DICT_LOKINUMBERSDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Dicts.h"
#include "LoKi/FuncOps.h"
#include "LoKi/BasicFunctors.h"
// ============================================================================
/** @file
 *  Simpe file to build the dictionaries for LoKi's functors, which 
 *  deal only with "plain numbers"
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-12-01
 */
// ============================================================================
namespace LoKi
{
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
      //
      static Fun::result_type __call__ 
      ( const Fun& fun  , const double           o ) { return fun ( o ) ; }
      // ======================================================================
      static std::vector<Fun::result_type> __rrshift__ 
      ( const Fun& fun  , const std::vector<double>& o ) 
      { return o >> fun  ; }
      static Fun::result_type              __rrshift__ 
      ( const Fun& fun  , const double           o ) { return fun ( o ) ; }
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
    /** the explicit full template specialzation of  LoKi::Dicts::PipeOps
     *  for the case of "std::vector<double>" argument
     *  @see LoKi::Dicts::PipeOps
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-12-01
     */
    template <>
    class  PipeOps<double,double>
    {
    private:
      typedef double TYPE  ;
      typedef double TYPE2 ;
      typedef LoKi::BasicFunctors<TYPE>::Pipe          Pipe    ;
      typedef LoKi::BasicFunctors<TYPE>::Element       Element ;
      typedef LoKi::BasicFunctors<TYPE2>::Function     Func    ;
      typedef LoKi::BasicFunctors<TYPE2>::Predicate    Cuts    ;      
    public:
      // __rshift__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> >
      __rshift__ 
      ( const Pipe& fun , const Pipe&      fun2 ) 
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
      __rrshift__ ( const Pipe& fun , const double val ) 
      { return fun ( std::vector<double>( 1 , val ) ) ; }
    public:
      // __tee__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> > 
      __tee__     ( const Pipe& fun ) 
      { return LoKi::tee<TYPE>( fun ) ; }        
    };
    // ========================================================================
    /** the explicit full template specialzation of  LoKi::Dicts::FunValOps
     *  for the case of "std::vector<double>" argument
     *  @see LoKi::Dicts::FunValOps
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-12-01
     */
    template <>
    class  FunValOps<double> 
    {
    private:
      typedef double TYPE ;
      typedef LoKi::BasicFunctors<TYPE>::FunVal         FunVal  ;
      typedef LoKi::BasicFunctors<double>::Function     Func    ;
      typedef LoKi::BasicFunctors<double>::Predicate    Cuts    ;      
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
      { return fun ( val  )  ; }
      // __rrshift__ 
      static double 
      __rrshift__ ( const FunVal& fun , 
                    const double val ) 
      { return fun ( std::vector<double>( 1 , val ) ) ; }
    public:
      // __tee__ 
      static LoKi::FunctorFromFunctor<std::vector<TYPE>,std::vector<TYPE> > 
      __tee__     ( const FunVal& fun ) 
      { return LoKi::tee<TYPE>( fun ) ; }        
    };
    // ========================================================================
  } // end of namespace LoKi::Dicts 
} // end of namespace LoKi
// ============================================================================
namespace
{
  struct _Instantiations 
  {
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
    /// fictive non-existent constructor 
    _Instantiations() ;
  };  
} // end of anonymous namespace 
// ============================================================================
// The END 
// ============================================================================
#endif // DICT_LOKINUMBERSDICT_H
// ============================================================================
