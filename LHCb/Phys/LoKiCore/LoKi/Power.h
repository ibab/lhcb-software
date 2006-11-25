// $Id: Power.h,v 1.6 2006-11-25 19:12:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.6 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2006/10/05 11:52:06  ibelyaev
//  fix compilation warnings for slc4_ia32_gcc345
//
// ============================================================================
#ifndef LOKI_POWER_H 
#define LOKI_POWER_H 1
// ============================================================================
// Include files
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/cpower.h"
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
  /** @class Power Power.h LoKi/Power.h
   *
   *  Simple function which allows the efficient 
   *  evaluation of pow(X,N) for LoKi-functions 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr 
   *  @date   2005-04-09
   */
  template <class TYPE>
  class Power : public LoKi::Function<TYPE>
  {
  public:
    /// define all nesessary types 
    _LOKI_FUNCTION_TYPES_( Power , TYPE ) ;
  protected:
    typedef Power<TYPE>  _Self ;
  public: 
    /** Standard constructor
     *  @param fun function to be used in "pow"
     *  @param val power itself
     */
    Power ( const LoKi::Function<TYPE>& fun , 
            const int                   val ) 
      : LoKi::Function<TYPE>() 
      , m_fun ( fun ) 
      , m_val ( val ) 
    {};
    /** copy constructor 
     *  @param rigth, object to be copied 
     */
    Power ( const _Self & right ) 
      : LoKi::AuxFunBase ( right ) 
      , LoKi::Function<TYPE> ( right ) 
      , m_fun ( right.m_fun ) 
      , m_val ( right.m_val ) 
    {};
    /// MANDATORY: virtual destructor 
    virtual ~Power( ) {} ;
    /// MANDATORY: clone method ("virtual constructor")
    virtual _Self* clone() const { return new _Self( *this ) ; }
    /// MANDATORY: the only one essential method 
    virtual  result_type operator() ( argument p ) const 
    {
      if ( 0 == m_val ) { return 1 ; }
      // evaluate the function
      result_type value = m_fun( p ) ;
      // evaluate the result
      if ( 0 <  m_val ) { return LoKi::cpower ( value ,  m_val ) ; }
      if ( 0 == value ) 
      { this->Error( " ZERO in NEGATIVE power! return -1.e+9" ) ; return -1.e+9 ; }
      //
      return LoKi::cpower ( 1.0/value , -m_val ) ; 
    };
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "pow("  << m_fun << "," << m_val << ")" ; };
  private:
    /// default constructor is disabled 
    Power() ;
  private:
    LoKi::FunctionFromFunction<TYPE>  m_fun ;
    int                               m_val ;
  };
  
  /** @fn pow
   *  pow for LoKi functions
   *  @see LoKi::Power 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2005-04-09
   */      
  template <class TYPE> 
  inline LoKi::FunctionFromFunction<TYPE>
  pow ( const LoKi::Function<TYPE>& fun , 
        const int                   val ) 
  { return LoKi::Power<TYPE> ( fun , val ) ; }
  
  /** @fn pow2
   *  pow2 for LoKi functions
   *  @see LoKi::pow
   *  @see LoKi::Power 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2005-04-09
   */      
  template <class TYPE> 
  inline LoKi::FunctionFromFunction<TYPE>
  pow2 ( const LoKi::Function<TYPE>& fun ) 
  { return LoKi::pow ( fun , 2 ) ; }
  
  /** @fn pow3
   *  pow3 for LoKi functions
   *  @see LoKi::pow
   *  @see LoKi::Power 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2005-04-09
   */      
  template <class TYPE> 
  inline LoKi::FunctionFromFunction<TYPE>
  pow3 ( const LoKi::Function<TYPE>& fun ) 
  { return LoKi::pow ( fun , 3 ) ; }
  
  /** @fn pow4
   *  pow4 for LoKi functions
   *  @see LoKi::pow
   *  @see LoKi::Power 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2005-04-09
   */      
  template <class TYPE> 
  inline LoKi::FunctionFromFunction<TYPE>
  pow4 ( const LoKi::Function<TYPE>& fun ) 
  { return LoKi::pow ( fun , 4 ) ; }
  
  /** @fn square
   *  pow2 for LoKi functions
   *  @see LoKi::pow
   *  @see LoKi::Power 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2005-04-09
   */      
  template <class TYPE> 
  inline LoKi::FunctionFromFunction<TYPE>
  square ( const LoKi::Function<TYPE>& fun ) 
  { return LoKi::pow2 ( fun ) ; }
  
} //end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_POWER_H
// ============================================================================
