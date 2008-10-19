// $Id: Power.h,v 1.11 2008-10-19 16:11:40 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_POWER_H 
#define LOKI_POWER_H 1
// ============================================================================
// Include files
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Power.h"
// ============================================================================
// LOKi
// ============================================================================
#include "LoKi/Math.h"
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
  // ==========================================================================
  /** @class Power Power.h LoKi/Power.h
   *
   *  Simple function which allows the efficient 
   *  evaluation of pow(X,N) for LoKi-functions 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr 
   *  @date   2005-04-09
   */
  // ==========================================================================
  template <class TYPE, class TYPE2=double>
  class Power : public LoKi::Functor<TYPE,TYPE2>
  {
  private:
    // ========================================================================
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /** Standard constructor
     *  @param fun function to be used in "pow"
     *  @param val power itself
     */
    Power ( const LoKi::Functor<TYPE,TYPE2>& fun , 
            const int                        val ) 
      : LoKi::Functor<TYPE,TYPE2>() 
      , m_fun ( fun ) 
      , m_val ( val ) 
    {}
    /// copy constructor 
    Power ( const Power & right ) 
      : LoKi::AuxFunBase ( right ) 
      , LoKi::Functor<TYPE,TYPE2> ( right ) 
      , m_fun ( right.m_fun ) 
      , m_val ( right.m_val ) 
    {}
    /// MANDATORY: virtual destructor 
    virtual ~Power( ) {} ;
    /// MANDATORY: clone method ("virtual constructor")
    virtual  Power* clone() const { return new Power ( *this ) ; }
    /// MANDATORY: the only one essential method 
    virtual  result_type operator() ( argument p ) const 
    {
      if ( 0 == m_val ) { return TYPE2(1) ; }
      // evaluate the function
      result_type value = m_fun.fun ( p ) ;
      // evaluate the result
      if ( 0 <  m_val ) { return Gaudi::Math::pow ( value ,  m_val ) ; }
      if ( 0 == value ) 
      { this->Error ( " ZERO in NEGATIVE power! return -1.e+9" ) ; return -1.e+9 ; }
      //
      return Gaudi::Math::pow ( 1.0/value , -m_val ) ; 
    };
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "pow("  << m_fun << "," << m_val << ")" ; };
    // ========================================================================
  private:
    // ========================================================================
    /// default constructor is disabled 
    Power() ;                                // default constructor is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2>  m_fun ;                // the functor 
    /// the power 
    int                                   m_val ;                  // the power
    // ========================================================================
  };
  // ==========================================================================
  template <class TYPE2>
  class Power<void,TYPE2> : public LoKi::Functor<void,TYPE2>
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /** Standard constructor
     *  @param fun function to be used in "pow"
     *  @param val power itself
     */
    Power ( const LoKi::Functor<TYPE,TYPE2>& fun , 
            const int                        val ) 
      : LoKi::Functor<TYPE,TYPE2>() 
      , m_fun ( fun ) 
      , m_val ( val ) 
    {}
    /// copy constructor 
    Power ( const Power & right ) 
      : LoKi::AuxFunBase ( right ) 
      , LoKi::Functor<TYPE,TYPE2> ( right ) 
      , m_fun ( right.m_fun ) 
      , m_val ( right.m_val ) 
    {}
    /// MANDATORY: virtual destructor 
    virtual ~Power( ) {} ;
    /// MANDATORY: clone method ("virtual constructor")
    virtual  Power* clone() const { return new Power ( *this ) ; }
    /// MANDATORY: the only one essential method 
    virtual  result_type operator() ( /* argument a */ ) const 
    {
      if ( 0 == m_val ) { return TYPE2(1) ; }
      // evaluate the function
      result_type value = m_fun.fun ( /* a */ ) ;
      // evaluate the result
      if ( 0 <  m_val ) { return Gaudi::Math::pow ( value ,  m_val ) ; }
      if ( 0 == value ) 
      { this->Error ( " ZERO in NEGATIVE power! return -1.e+9" ) ; return -1.e+9 ; }
      //
      return Gaudi::Math::pow ( 1.0/value , -m_val ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "pow("  << m_fun << "," << m_val << ")" ; };
    // ========================================================================
  private:
    // ========================================================================
    /// default constructor is disabled 
    Power() ;                                // default constructor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2>  m_fun ;                // the functor 
    /// the power 
    int                                   m_val ;                  // the power 
    // ========================================================================
  };
  // ==========================================================================
  /** pow for LoKi functions
   *  @see LoKi::Power 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2005-04-09
   */      
  template <class TYPE,class TYPE2> 
  inline LoKi::Power<TYPE,TYPE2>
  pow ( const LoKi::Functor<TYPE,TYPE2>& fun , 
        const int                        val ) 
  { return LoKi::Power<TYPE,TYPE2> ( fun , val ) ; }
  // ==========================================================================  
  /** pow2 for LoKi functions
   *  @see LoKi::pow
   *  @see LoKi::Power 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2005-04-09
   */      
  template <class TYPE,class TYPE2> 
  inline LoKi::Power<TYPE,TYPE2>
  pow2 ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::Power<TYPE,TYPE2> ( fun , 2 ) ; }
  // ==========================================================================  
  /** pow3 for LoKi functions
   *  @see LoKi::pow
   *  @see LoKi::Power 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2005-04-09
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::Power<TYPE,TYPE2>
  pow3 ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::Power<TYPE,TYPE2> ( fun , 3 ) ; }
  // ==========================================================================  
  /** pow4 for LoKi functions
   *  @see LoKi::pow
   *  @see LoKi::Power 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2005-04-09
   */      
  template <class TYPE,class TYPE2> 
  inline LoKi::Power<TYPE,TYPE2>
  pow4 ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::Power<TYPE,TYPE2> ( fun , 4 ) ; }
  // ==========================================================================  
  /** pow2 for LoKi functions
   *  @see LoKi::pow
   *  @see LoKi::Power 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2005-04-09
   */      
  template <class TYPE,class TYPE2> 
  inline LoKi::Power<TYPE,TYPE2>
  square ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::Power<TYPE,TYPE2> ( fun , 2 ) ; }
  // ==========================================================================
} //end of namespace LoKi
// ============================================================================
#endif // LOKI_POWER_H
// ============================================================================
// The END 
// ============================================================================
