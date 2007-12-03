// $Id: BiFunctions.h,v 1.5 2007-12-03 12:03:22 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_BIFUNCTIONS_H 
#define LOKI_BIFUNCTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Holder.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** mimic 2-argument function
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-06-08
   */
  template <class TYPE1,class TYPE2,class TYPE3>
  class Functor<LoKi::Holder<TYPE1,TYPE2>, TYPE3>
    : public    std::unary_function<const LoKi::Holder<TYPE1,TYPE2>,TYPE3> 
    , virtual   public LoKi::AuxFunBase
  {
  public:
    // parameters: argument
    typedef LoKi::Holder<TYPE1,TYPE2>  Type1 ; ///< parameters: argument
    // parameters: return value 
    typedef TYPE3                      Type2 ; ///< parameters: return value
  public:
    /// the type of the argument
    typedef LoKi::Holder<TYPE1,TYPE2> TYPE ;
    typedef LoKi::Holder<TYPE1,TYPE2> Type ;
    /// the actual type of base 
    typedef typename std::unary_function<const TYPE,TYPE3>  Base_F ;
    typedef typename std::unary_function<const TYPE,TYPE3>  Base_1 ;
    /// the result value 
    typedef typename Base_F::result_type   result_type   ;
    /// the basic argument type 
    typedef typename Base_F::argument_type argument_type ;
    /// type for the argument 
    typedef typename boost::call_traits<const TYPE>::param_type  argument;
    /// type for the first argument 
    typedef typename boost::call_traits<const TYPE1>::param_type first_argument      ;
    /// type for the first argument 
    typedef typename boost::call_traits<const TYPE1>::param_type first_argument_type ;
    /// type for the second argument 
    typedef typename boost::call_traits<const TYPE2>::param_type second_argument     ;
    /// type for the second argument 
    typedef typename boost::call_traits<const TYPE2>::param_type second_argument_type ;
  public:
    /// the native interface with 1 argument: 
    virtual result_type  operator () ( argument a ) const = 0 ;
    /// the only one essential method ("function")
    virtual result_type  evaluate    ( argument a ) const 
    { return (*this)( a ) ; }    
    /// the only one essential method ("function")
    virtual result_type  eval        ( argument a ) const 
    { return (*this)( a ) ; }
    /// the only one essential method ("function")
    virtual result_type operator () 
      ( first_argument  a1 , 
        second_argument a2 ) const { return (*this)( Type ( a1 , a2 ) ) ; }
    /// the only one essential method ("function")
    virtual result_type evaluate  
    ( first_argument  a1 , 
      second_argument a2 )   const { return (*this)( a1 , a2 ) ; }
    /// the only one essential method ("function")
    virtual result_type eval    
    ( first_argument  a1 , 
      second_argument a2 )   const { return (*this)( a1 , a2 ) ; }
    /// MANDATORY: clone method 
    virtual  Functor* clone    ()              const = 0 ;
    /// virtual destructor 
    virtual ~Functor(){} 
  public:
    // protected default constructor 
    Functor ()                       ///< protected default constructor 
      : AuxFunBase (     ) {}
    // protected copy constructor
    Functor ( const Functor& fun )   ///< protected copy constructor
      : AuxFunBase ( fun ) {}
  private:
    // assignement         is private 
    Functor& operator=( const Functor& ); ///< assignement         is private 
  } ;
} // end of namespace LoKi 
// ============================================================================
#endif // LOKI_BIFUNCTIONS_H
// ============================================================================
