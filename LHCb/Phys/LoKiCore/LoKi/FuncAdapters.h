// $Id: FuncAdapters.h,v 1.6 2006-11-25 19:12:55 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2006/10/05 11:52:05  ibelyaev
//  fix compilation warnings for slc4_ia32_gcc345
//
// Revision 1.4  2006/05/02 14:29:09  ibelyaev
//  censored
//
// ============================================================================
#ifndef LOKI_FUNCADAPTERS_H 
#define LOKI_FUNCADAPTERS_H 1
// ============================================================================
//  include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
// ============================================================================

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
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi 
{
  /** @namespace LoKi::Adapters LoKi/FuncAdapters.h 
   *  Simple namespace with "function adapters"
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25
   */ 
  namespace Adapters
  {
    /** @class FunAdapter 
     *  The generic templated adapter for the functions 
     * 
     *  @code 
     *
     *  // FUNCTION
     *  double myFun ( const Particle* p ) ;
     *  ... 
     *  
     *  typedef LoKi::Adapters::FunAdapter<const Particle*> AFun ;
     *
     *  // create the function itself 
     *  Fun fun = AFun( &myFun ) 
     * 
     *  const Particle* p = ... ;
     *  // use the function/functor 
     *  const double result = fun( p ) ;
     * 
     *  @endcode 
     *  
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-27
     */
    template <class TYPE>
    class FunAdapter : public LoKi::Function<TYPE>
    {
    protected:
      /// self-type 
      typedef FunAdapter<TYPE>     MyType ;
      // base type 
      typedef LoKi::Function<TYPE> MyBase  ;
    public:
      /// the actual type of the function
      typedef typename MyBase::result_type (*function)
        ( typename MyBase::argument_type ) ;
      /// 
    public:
      /** contructor from the function 
       *  @param fun the function itself 
       */
      FunAdapter ( function fun ) 
        : MyBase () 
        , m_fun  ( fun ) 
      {};
      /** copy constructor 
       *  @param rigth object to be copied 
       */
      FunAdapter ( const MyType& right ) 
        : LoKi::AuxFunBase ( right ) 
        , MyBase ( right       ) 
        , m_fun  ( right.m_fun )
      {};
      /// virtual destructor 
      virtual ~FunAdapter(){};
      /// MANDATORY: clone method ("virtual constructor")
      virtual MyType* clone() const 
      { return new MyType( *this ) ;}
      /// MANDATORY: the only one essential method 
      virtual typename MyBase::result_type operator() 
        ( typename MyBase::argument arg ) const 
      { return m_fun( arg ) ; }
    private:
      // default constructor is private 
      FunAdapter();
      // assignement operator is disabled 
      MyType& operator=( const MyType& right ) ;
    private:
      function m_fun ;
    };
    
    /** @class CutAdapter 
     *  The generic templated adapter for the predicates
     * 
     *  @code 
     *
     *  // PREDICATE
     *  bool myCut ( const Particle* p ) ;
     *  ... 
     *  
     *  typedef LoKi::Adapters::CutAdapter<const Particle*> ACut ;
     *
     *  // create the predicate  itself 
     *  Cut cut = ACut( &myFun ) 
     * 
     *  const Particle* p = ... ;
     *  // use the predicate/functor 
     *  const bool result = cut( p ) ;
     * 
     *  @endcode 
     *       
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-27
     */
    template <class TYPE>
    class CutAdapter : public LoKi::Predicate<TYPE>
    {
    protected:
      /// self-type 
      typedef CutAdapter<TYPE>      MyType ;
      // base type 
      typedef LoKi::Predicate<TYPE> MyBase  ;
    public:
      /// the actual type of the predicate
      typedef typename MyBase::result_type (*predicate) 
        ( typename MyBase::argument_type ) ;
      /// 
    public:
      /** contructor from the predicate
       *  @param cut the function itself 
       */
      CutAdapter ( predicate cut ) 
        : MyBase () 
        , m_cut  ( cut ) 
      {};
      /** copy constructor 
       *  @param rigth object to be copied 
       */
      CutAdapter ( const MyType& right ) 
        : LoKi::AuxFunBase ( right ) 
        , MyBase ( right       )
        , m_cut  ( right.m_cut )
      {};
      /// virtual destructor 
      virtual ~CutAdapter(){};
      /// MANDATORY: clone method ("virtual constructor")
      virtual MyType* clone() const 
      { return new MyType( *this ) ;}
      /// MANDATORY: the only one essential method 
      virtual typename MyBase::result_type operator() 
        ( typename MyBase::argument arg ) const 
      { return m_cut( arg ) ; }
    private:
      // default constructor is private 
      CutAdapter();
      // assignement operator is disabled 
      MyType& operator=( const MyType& right ) ;
    private:
      predicate m_cut ;
    };
    
  } // end of namespace Adapters
  
  /** @fn aFun 
   *  helper templated function to make easier the 
   *  creation of adapter-functors:
   *
   *  @code 
   *
   *  // FUNCTION
   *  double myCun ( const Particle* p ) ;
   *  ... 
   *
   *  // create the function itself 
   *  Fun fun = aFun( &myFun ) 
   * 
   *  const Particle* p = ... ;
   *  // use the function/functor 
   *  const double result = fun( p ) ;
   * 
   *  @endcode 
   *
   *  @see LoKi::Adapters::FunAdapter 
   *
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
   *  @date 2005-03-27
   */
  template <class TYPE>
  inline LoKi::Adapters::FunAdapter<TYPE> aFun ( double (*fun) ( TYPE )  ) 
  { return LoKi::Adapters::FunAdapter<TYPE> ( fun ) ; } ;
  
  /** @fn aCut 
   *  helper templated function to make easier the 
   *  creation of adapter-functors:
   *
   *  @code 
   *
   *  // PREDICATE
   *  double myCut ( const Particle* p ) ;
   *  ... 
   *
   *  // create the function itself 
   *  Cut cut = aCut( &myCut ) 
   * 
   *  const Particle* p = ... ;
   *  // use the predicate/functor 
   *  const bool result = cut( p ) ;
   * 
   *  @endcode 
   *
   *  @see LoKi::Adapters::CutAdapter 
   * 
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
   *  @date 2005-03-27
   */
  template <class TYPE>
  inline LoKi::Adapters::CutAdapter<TYPE> aCut ( bool (*cut) ( TYPE )  ) 
  { return LoKi::Adapters::CutAdapter<TYPE> ( cut ) ; } ;
  
} // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_FUNCADAPTERS_H
// ============================================================================
