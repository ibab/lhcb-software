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
  // ===========================================================================
  /** @namespace LoKi::Adapters LoKi/FuncAdapters.h 
   *  Simple namespace with "function adapters"
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25
   */ 
  namespace Adapters
  {
    // ========================================================================
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
    template <class TYPE, class TYPE2=double>
    class FunAdapter : public LoKi::Functor<TYPE,TYPE2>
    {
    protected:
      // base type 
      typedef LoKi::Functor<TYPE,TYPE2> MyBase  ;
    public:
      /// the actual type of the function
      typedef typename MyBase::result_type (*function)
        ( typename MyBase::argument_type ) ;
      /// 
    public:
      /// contructor from the function 
      FunAdapter ( function fun ) 
        : MyBase () 
        , m_fun  ( fun ) 
      {}
      /// copy constructor 
      FunAdapter ( const FunAdapter& right ) 
        : LoKi::AuxFunBase ( right ) 
        , MyBase ( right       ) 
        , m_fun  ( right.m_fun )
      {}
      /// virtual destructor 
      virtual ~FunAdapter(){};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  FunAdapter* clone() const { return new FunAdapter( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual typename MyBase::result_type operator() 
        ( typename MyBase::argument arg ) const 
      { return m_fun ( arg ) ; }
    private:
      // default constructor is private 
      FunAdapter();
      // assignement operator is disabled 
      FunAdapter& operator=( const FunAdapter& right ) ;
    private:
      function m_fun ;
    };
    // ========================================================================    
  } // end of namespace Adapters
  // ==========================================================================
  /** helper templated function to make easier the 
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
  inline LoKi::Adapters::FunAdapter<TYPE,double> aFun ( double (*fun) ( TYPE ) ) 
  { return LoKi::Adapters::FunAdapter<TYPE,double> ( fun ) ; } 
  // ==========================================================================  
  /** helper templated function to make easier the 
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
  inline LoKi::Adapters::FunAdapter<TYPE,bool> aCut ( bool (*cut) ( TYPE ) ) 
  { return LoKi::Adapters::FunAdapter<TYPE,bool> ( cut ) ; } 
  // ==========================================================================  
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_FUNCADAPTERS_H
// ============================================================================
