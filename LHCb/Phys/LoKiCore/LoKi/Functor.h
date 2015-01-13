// ============================================================================
#ifndef LOKI_FUNCTOR_H
#define LOKI_FUNCTOR_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <functional>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AuxFunBase.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/call_traits.hpp"
// ============================================================================
// ROOT
// ============================================================================
#include "RVersion.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class Functor Functor.h LoKi/Functor.h
   *  The most generic LoKi-functor, needed for implementation
   *  of numerious brilliant ideas by Gerhard "The Great".
   *
   *  This file is a part of LoKi project -
   *  ``C++ ToolKit  for Smart and Friendly Physics Analysis''
   *
   *  The package has been designed with the kind help from
   *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
   *  contributions and advices from G.Raven, J.van Tilburg,
   *  A.Golutvin, P.Koppenburg have been used in the design
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-10-31
   */
  template <class TYPE,class TYPE2>
  class Functor : public virtual LoKi::AuxFunBase
  {
  public:
    /// =======================================================================
    /// parameters: argument
    typedef TYPE  Type1 ;                               // parameters: argument
    /// parameters: return value
    typedef TYPE2 Type2 ;                           // parameters: return value
    /// =======================================================================
  private : // fake STL bases
    // ========================================================================
    /// STD signature (fake base)
    typedef typename std::unary_function<TYPE,TYPE2>                   Base_1 ;
    // ========================================================================
  public:
    // ========================================================================
    /// the type of the argument
    typedef TYPE                                                Type          ;
    // ========================================================================
  public:  // STD (fake) signature
    // ========================================================================
    /// STL: the result value
    typedef typename Base_1::result_type                        result_type   ;
    /// STL: the basic argument type
    typedef typename Base_1::argument_type                      argument_type ;
    // ========================================================================
  public:  // the actual signature
    // ========================================================================
    /// type for the argument
    typedef typename boost::call_traits<const TYPE>::param_type argument      ;
    // ========================================================================
  public:
    // ========================================================================
    /// the only one essential method ("function")
    virtual result_type operator () ( argument    ) const = 0 ;
    /// the only one essential method ("function")
    virtual result_type evaluate    ( argument  a ) const
    { return (*this)( a ) ; }
    /// the only one essential method ("function")
    virtual result_type eval        ( argument  a ) const
    { return (*this)( a ) ; }
    /// clone method
    virtual  Functor* clone    ()                   const = 0 ;
    /// virtual destructor
    virtual ~Functor(){}
    // ========================================================================
  protected:
    // ========================================================================
    /// protected default constructor
    Functor() : AuxFunBase (     ) {}
    /// protected copy constructor
    Functor ( const Functor& fun ) : AuxFunBase ( fun ) {}
    // ========================================================================
  private:
    // ========================================================================
    /// the assignement operator is disabled
    Functor& operator=( const Functor& );        // the assignement is disabled
    // ========================================================================
  };
  // ==========================================================================
  /** @class FunctorFromFunctor
   *  the most trivial implementation of LoKi::Functor interface,
   *  It just delegates all actions to the underlying functor
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-10-30
   */
  template <class TYPE,class TYPE2>
  class FunctorFromFunctor : public LoKi::Functor<TYPE,TYPE2>
  {
  public:
    // ========================================================================
    /// the underlying type of functor
    typedef LoKi::Functor<TYPE,TYPE2>   functor ;               // functor type
    // ========================================================================
  protected:
    // ========================================================================
    /// own type
    typedef FunctorFromFunctor<TYPE,TYPE2> Self ;               // the own type
    // ========================================================================
  public:
    // ========================================================================
    /// constructor
    FunctorFromFunctor ( const Functor<TYPE,TYPE2>& right )
      : LoKi::Functor<TYPE,TYPE2> ()
      , m_fun ( right.clone() )
    {}
    /// copy constructor (deep copy)
    FunctorFromFunctor ( const FunctorFromFunctor& right )
      : LoKi::AuxFunBase          ( right )
      , LoKi::Functor<TYPE,TYPE2> ( right )
      , m_fun ( 0 )
    {
      m_fun = typeid ( Self ) == typeid ( right ) ?
        right.m_fun -> clone () : right.clone() ;
    }
#ifndef __GCCXML__
    /// move constructor (avoid cloning)
    FunctorFromFunctor ( FunctorFromFunctor&& right )
      : LoKi::AuxFunBase          ( right )
      , LoKi::Functor<TYPE,TYPE2> ( right )
      , m_fun ( right.m_fun )
    {
      right.m_fun = 0 ; // ATTENTION!
    }
#endif
    /// MANDATORY: virtual destructor
    virtual ~FunctorFromFunctor() { delete m_fun ; }
    // ========================================================================
  public:
    // ========================================================================
    /// MANDATORY: clone method ("virtual constructor")
    virtual  FunctorFromFunctor* clone() const
    { return new FunctorFromFunctor ( *this ) ; }
    /// MANDATORY: the only one essential method
    virtual typename functor::result_type operator()
      ( typename functor::argument a ) const
    { return fun ( a ) ; }
    /// OPTIONAL: the basic printout method, delegate to the underlying object
    virtual std::ostream& fillStream( std::ostream& s ) const
    { return  m_fun->fillStream( s ) ; };
    /// OPTIONAL: unique function ID, delegate to the underlying objects
    virtual std::size_t   id () const { return m_fun->id() ; }
    /// OPTIONAL: delegate the object type
    virtual std::string   objType () const { return m_fun -> objType() ; }
    /// C++ printout: delegate
    virtual std::string   toCpp   () const { return m_fun -> toCpp  () ; }
    // ========================================================================
  public:
    // ========================================================================
    /// the assignement operator is enabled
    FunctorFromFunctor& operator= ( const FunctorFromFunctor& right )
    {
      if ( this == &right ) { return *this ; }                        // RETURN
      // set new pointer
      LoKi::Functor<TYPE,TYPE2>* newf = 0 ;
      if ( typeid ( Self ) != typeid ( right ) )
      { newf = right.         clone() ; }                              // CLONE!
      else
      { newf = right.m_fun -> clone() ; }                              // CLONE!
      // delete own pointer
      delete m_fun ;
      // assign the new pointer
      m_fun = newf ;
      return *this ;                                                  // RETURN
    }
    /// the assignement operator is enabled
    FunctorFromFunctor& operator= ( const Functor<TYPE,TYPE2>& right )
    {
      if ( this == &right ) { return *this ; }                        // RETURN
      // set new pointer
      LoKi::Functor<TYPE,TYPE2>* newf = right.clone() ;
      // delete own pointer
      delete m_fun ;
      // assign the new pointer
      m_fun = newf ;
      return *this ;                                                  // RETURN
    }
    // ========================================================================
  public:
    // ========================================================================
    /// evaluate the function
    inline typename functor::result_type fun
    ( typename functor::argument a ) const { return (*m_fun) ( a ) ; }
    // accessor to the function
    inline const functor& func () const { return *m_fun ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    FunctorFromFunctor();                // the default constructor is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the actual underlaying function
    const functor* m_fun ;                           // the underlaying functor
    // ========================================================================
  } ;
  // ==========================================================================
  /** @class Constant
   *  The helper concrete implementation of the simplest
   *  function ("constant")
   *  @see LoKi::Functor
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE,class TYPE2>
  class Constant : public LoKi::Functor<TYPE,TYPE2>
  {
  public:
    // ========================================================================
    /// argument type
    typedef typename boost::call_traits<TYPE2>::param_type T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /// constructor
    Constant ( T2 value )
      : LoKi::AuxFunBase ( std::tie( value ) ) 
      , LoKi::Functor<TYPE,TYPE2>()
      , m_value ( value )
    {}
    /// copy constructor
    Constant ( const Constant& right )
      : LoKi::AuxFunBase          ( right )
      , LoKi::Functor<TYPE,TYPE2> ( right )
      , m_value                   ( right.m_value )
    {}
    /// destructor
    virtual ~Constant(){}
    // ========================================================================
  public:
    // ========================================================================
    /// assignement
    Constant& operator=( const Constant& right )
    { m_value = right.m_value ; return *this ; }
    /// assignement
    Constant& operator=( T2 right )
    { m_value = right         ; return *this ; }
    /// clone method (mandatory)
    virtual Constant* clone   () const { return new Constant( *this ) ; }
    /// the only one essential method ("function")
    virtual typename LoKi::Functor<TYPE,TYPE2>::result_type operator()
      ( typename LoKi::Functor<TYPE,TYPE2>::argument ) const { return m_value ; }
    /// the basic printout method
    virtual std::ostream& fillStream( std::ostream& s ) const ;
    // ========================================================================
  private:
    // ========================================================================
    /// no default constructor
    Constant() ;                                      // no default constructor
    // ========================================================================
  private:
    // ========================================================================
    /// the constant itself
    TYPE2 m_value ;                                      // the constant itself
    // ========================================================================
  } ;
  // ==========================================================================
  /// specialiation for void-argument
  template <class TYPE2>
  class Functor<void,TYPE2> : public virtual LoKi::AuxFunBase
  {
  public:
    /// =======================================================================
    /// parameters: argument
    typedef void  Type1 ;                               // parameters: argument
    /// parameters: return value
    typedef TYPE2 Type2 ;                           // parameters: return value
    /// =======================================================================
  private:
    // ========================================================================
    /// STD signature (fake base)
    typedef typename std::unary_function<void,TYPE2>                   Base_1 ;
    // ========================================================================
  public:
    // ========================================================================
    /// the type of the argument
    typedef void                                                Type          ;
    /// the result value
    typedef typename Base_1::result_type                        result_type   ;
    /// the basic argument type
    typedef void                                                argument_type ;
    /// type for the argument
    typedef void                                                argument      ;
    // ========================================================================
  public:
    // ========================================================================
    /// the only one essential method ("function")
    virtual result_type operator () ( /* argument */ ) const = 0 ;
    /// the only one essential method ("function")
    virtual result_type evaluate    ( /* argument */ ) const
    { return (*this) ( /* */ ) ; }
    /// the only one essential method ("function")
    virtual result_type eval        ( /* argument */ ) const
    { return (*this) ( /* */ ) ; }
    /// clone method
    virtual  Functor* clone    ()                   const = 0 ;
    /// virtual destructor
    virtual ~Functor(){}
    // ========================================================================
  protected:
    // ========================================================================
    /// protected default constructor
    Functor() : AuxFunBase (     ) {}          // protected default constructor
    /// protected copy constructor
    Functor ( const Functor& fun ) : AuxFunBase ( fun ) {}    // protected copy
    // ========================================================================
  private:
    // ========================================================================
    // the assignement operator is disabled
    Functor& operator=( const Functor& );             // assignement is private
    // ========================================================================
  };
  // ==========================================================================
  /// the specialization for "void"-argument
  template <class TYPE2>
  class FunctorFromFunctor<void,TYPE2>: public LoKi::Functor<void,TYPE2>
  {
  public:
    // ========================================================================
    /// the underlying type of functor
    typedef LoKi::Functor<void,TYPE2>   functor ;     // the underlying functor
    // ========================================================================
  protected:
    // ========================================================================
    /// own type
    typedef FunctorFromFunctor<void,TYPE2> Self ;               // the own type
    // ========================================================================
  public:
    // ========================================================================
    /// constructor
    FunctorFromFunctor ( const Functor<void,TYPE2>& right )
      : LoKi::Functor<void,TYPE2> ()
        , m_fun ( right.clone() )
    {}
    /// copy constructor (deep copy)
    FunctorFromFunctor ( const FunctorFromFunctor& right )
      : LoKi::AuxFunBase          ( right )
      , LoKi::Functor<void,TYPE2> ( right )
      , m_fun ( 0 )
    {
      m_fun = typeid ( Self ) == typeid ( right ) ?
        right.m_fun -> clone () : right.clone() ;
    }
#ifndef __GCCXML__
    /// move constructor (avoid cloning)
    FunctorFromFunctor ( FunctorFromFunctor&& right )
      : LoKi::AuxFunBase          ( right )
      , LoKi::Functor<void,TYPE2> ( right )
      , m_fun ( right.m_fun )
    {
      right.m_fun = 0 ; // ATTENTION!
    }
#endif
    /// MANDATORY: virtual destructor
    virtual ~FunctorFromFunctor() { delete m_fun ; }
    // ========================================================================
  public:
    // ========================================================================
    /// MANDATORY: clone method ("virtual constructor")
    virtual  FunctorFromFunctor* clone() const
    { return new FunctorFromFunctor ( *this ) ; }
    /// MANDATORY: the only one essential method
    virtual typename functor::result_type operator()
      ( /* typename functor::argument a */ ) const { return fun ( /* a */ ) ; }
    /// OPTIONAL: the basic printout method, delegate to the underlying object
    virtual std::ostream& fillStream( std::ostream& s ) const
    { return  m_fun->fillStream( s ) ; };
    /// OPTIONAL: unique function ID, delegate to the underlying objects
    virtual std::size_t   id () const { return m_fun->id() ; }
    /// OPTIONAL: delegate the object type
    virtual std::string   objType () const { return m_fun -> objType() ; }
    /// C++ printout: delegate
    virtual std::string   toCpp   () const { return m_fun -> toCpp  () ; }
    // ========================================================================
  public:
    // ========================================================================
    /// the assignement operator is enabled
    FunctorFromFunctor& operator= ( const FunctorFromFunctor& right )
    {
      if ( this == &right ) { return *this ; }                        // RETURN
      // set new pointer
      LoKi::Functor<void,TYPE2>* newf = 0 ;
      if ( typeid ( Self ) != typeid ( right ) )
      { newf = right.         clone() ; }                             // CLONE!
      else
      { newf = right.m_fun -> clone() ; }                             // CLONE!
      // delete own pointer
      delete m_fun ;
      // assign the new pointer
      m_fun = newf ;
      return *this ;                                                  // RETURN
    }
    /// the assignement operator is enabled
    FunctorFromFunctor& operator= ( const Functor<void,TYPE2>& right )
    {
      if ( this == &right ) { return *this ; }                        // RETURN
      // set new pointer
      LoKi::Functor<void,TYPE2>* newf = right.clone() ;               // CLONE!
      // delete own pointer
      delete m_fun ;
      // assign the new pointer
      m_fun = newf ;
      return *this ;                                                  // RETURN
    }
    // ========================================================================
  public:
    // ========================================================================
    /// evaluate the function
    inline typename functor::result_type fun
    ( /* typename functor::argument a */ ) const { return (*m_fun) ( /* a */ ) ; }
    // accessor to the function
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
  /// specialization for "void"
  template <class TYPE2>
  class Constant<void,TYPE2> : public LoKi::Functor<void,TYPE2>
  {
  public:
    // ========================================================================
    /// argument type
    typedef typename boost::call_traits<TYPE2>::param_type T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /// constructor
    Constant ( T2 value )
      : LoKi::AuxFunBase ( std::tie( value ) ) 
      , LoKi::Functor<void,TYPE2>()
      , m_value ( value )
    {}
    /// copy constructor
    Constant ( const Constant& right )
      : LoKi::AuxFunBase          ( right )
      , LoKi::Functor<void,TYPE2> ( right )
      , m_value                   ( right.m_value )
    {}
    /// destructor
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5, 99, 0)) && defined(ROOT_5721_WORKAROUND)
    // workaround for https://sft.its.cern.ch/jira/browse/ROOT-5721
    virtual ~Constant() throw() {}
#else
    virtual ~Constant() {}
#endif
    // ========================================================================
  public:
    // ========================================================================
    /// assignement
    Constant& operator=( const Constant& right )
    { m_value = right.m_value ; return *this ; }
    /// assignement
    Constant& operator=( T2 right )
    { m_value = right         ; return *this ; }
    /// clone method (mandatory)
    virtual Constant* clone   () const { return new Constant( *this ) ; }
    /// the only one essential method ("function")
    virtual typename LoKi::Functor<void,TYPE2>::result_type operator()
      ( /* typename LoKi::Functor<void,TYPE2>::argument a */ ) const
    { return m_value ; }
    /// the basic printout method
    virtual std::ostream& fillStream( std::ostream& s ) const ;
    // ========================================================================
  private:
    // ========================================================================
    /// no default constructor
    Constant() ;                                      // no default constructor
    // ========================================================================
  private:
    // ========================================================================
    /// the constant itself
    TYPE2 m_value ;                                      // the constant itself
    // ========================================================================
  } ;
  // ==========================================================================
  // the generic printout method
  // ==========================================================================
  template <class TYPE,class TYPE2>
  inline std::ostream&
  Constant<TYPE,TYPE2>::fillStream( std::ostream& s ) const
  { return Gaudi::Utils::toStream ( this->m_value , s ) ; }
  // ==========================================================================
  // the basic printout method
  // ==========================================================================
  template <class TYPE2>
  inline std::ostream&
  Constant<void,TYPE2>::fillStream( std::ostream& s ) const
  { return  Gaudi::Utils::toStream ( this->m_value , s ) ; }
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_FUNCTOR_H
// ============================================================================
