// $Id: BiFunctions.h,v 1.6 2008-11-11 16:29:57 ibelyaev Exp $
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
    // ========================================================================
    /// parameters: argument
    typedef LoKi::Holder<TYPE1,TYPE2>  Type1 ; // parameters: argument
    /// parameters: return value 
    typedef TYPE3                      Type2 ; // parameters: return value
    // ========================================================================
  public:
    // ========================================================================
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
    // ========================================================================
  public:
    // ========================================================================
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
    // ========================================================================
  public:
    // ========================================================================
    /// protected default constructor 
    Functor () : AuxFunBase ( ) {}
    /// protected copy constructor
    Functor ( const Functor& fun ) : AuxFunBase ( fun ) {}
    // ========================================================================
  private:
    // ========================================================================
    /// assignement is private 
    Functor& operator=( const Functor& );            // assignement is private 
    // ========================================================================
  } ;
  // ==========================================================================
  template <class TYPE1,class TYPE2, class TYPE3>
  class FunctorFromFunctor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>
    : public LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3>
  {
  public:
    // ========================================================================
    /// the underlying type of functor 
    typedef LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3> functor ; 
    typedef LoKi::Functor<LoKi::Holder<TYPE1,TYPE2>,TYPE3> Base    ; 
    // ========================================================================
  protected:
    // ========================================================================
    /// own type 
    typedef FunctorFromFunctor<LoKi::Holder<TYPE1,TYPE2>,TYPE3> Self ;
    // ========================================================================
  public:
    // ========================================================================
    /// constructor 
    FunctorFromFunctor ( const functor& right ) 
      : Base ()  
      , m_fun ( right.clone() ) 
    {}
    /// copy constructor (deep copy)
    FunctorFromFunctor ( const Self& right ) 
      : LoKi::AuxFunBase ( right ) 
      , Base             ( right ) 
      , m_fun ( 0 ) 
    {
      m_fun = typeid ( Self ) == typeid ( right ) ? 
        right.m_fun -> clone () : right.clone() ;
    }
    /// MANDATORY: virtual destructor 
    virtual ~FunctorFromFunctor() { delete m_fun ; }
    /// MANDATORY: clone method ("virtual constructor") 
    virtual  FunctorFromFunctor* clone() const 
    { return new FunctorFromFunctor ( *this ) ; }
    /// MANDATORY: the only one essential method 
    virtual typename functor::result_type operator() 
      ( typename functor::argument a ) const { return fun ( a ) ; }    
    /// the only one essential method ("function")
    virtual typename functor::result_type operator () 
      ( typename functor::first_argument  a , 
        typename functor::second_argument b ) const { return fun ( a , b ) ; }
    /// OPTIONAL: the basic printout method, delegate to the underlying object
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return  m_fun->fillStream( s ) ; };
    /// OPTIONAL: unique function ID, delegate to the underlying objects 
    virtual std::size_t   id () const { return m_fun->id() ; }
    /// OPTIONAL: delegate the object type
    virtual std::string   objType () const { return m_fun -> objType() ; }
  public:
    /// the assignement operator is enabled 
    FunctorFromFunctor& operator= ( const FunctorFromFunctor& right )
    {
      if ( this == &right ) { return *this ; }                   // RETURN 
      // set new pointer 
      functor* newf = 0 ;
      if ( typeid ( Self ) != typeid ( right ) ) 
      { newf = right.         clone() ; }
      else
      { newf = right.m_fun -> clone() ; }
      // delete own pointer 
      delete m_fun ; 
      // assign the new pointer 
      m_fun = newf ;
      return *this ;                                             // RETURN 
    }
    /// the assignement operator is enabled 
    FunctorFromFunctor& operator= ( const Base& right )
    {
      if ( this == &right ) { return *this ; }                   // RETURN  
      // set new pointer 
      functor* newf = right.clone() ;
      // delete own pointer 
      delete m_fun ;
      // assign the new pointer 
      m_fun = newf ;
      return *this ;                                             // RETURN 
    } 
    // ========================================================================
  public:   
    // ========================================================================
    /// evaluate the function
    inline typename functor::result_type fun 
    ( typename functor::argument a ) const { return (*m_fun) ( a ) ; }
    inline typename functor::result_type fun 
    ( typename functor::first_argument  a , 
      typename functor::second_argument b ) const 
    { return (*m_fun) ( a , b ) ; }
    /// accessor to the function 
    inline const functor& fun  () const { return *m_fun ; }
    /// accessor to the function 
    inline const functor& func () const { return *m_fun ; }
    // ========================================================================
  private:
    // ========================================================================
    /// default constructor is private 
    FunctorFromFunctor();                 // the default constructor is private 
    // ========================================================================
  private:
    // ========================================================================
    /// the underlaying function 
    const functor* m_fun ;                           // the underlaying functor
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
#endif // LOKI_BIFUNCTIONS_H
// ============================================================================
