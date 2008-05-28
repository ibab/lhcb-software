// $Id: Primitives.h,v 1.8 2008-05-28 13:40:29 cattanem Exp $
// ============================================================================
#ifndef LOKI_PRIMITIVES_H 
#define LOKI_PRIMITIVES_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================
// LHCb 
// ============================================================================
#include "LHCbMath/LHCbMath.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/BiFunctions.h"
#include "LoKi/Reference.h"
#include "LoKi/Field.h"
#include "LoKi/valid.h"
#include "LoKi/same.h"
#include "LoKi/apply.h"
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
  /** @class TwoFunctors 
   *  helper class to keep two functors of the same type
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-01 
   */
  template <class TYPE,class TYPE2>
  class TwoFunctors
  {
  protected:
    /// the actual type of underlying functor 
    typedef LoKi::Functor<TYPE,TYPE2> functor ;
  public:
    TwoFunctors ( const functor& f1 , 
                  const functor& f2 ) 
      : m_fun1  ( f1 ) 
      , m_fun2  ( f2 ) 
    {}
    /// destructor 
    virtual ~TwoFunctors() {}
  public:
    /// evaluate the first functor 
    typename functor::result_type fun1 
    ( typename functor::argument a ) const { return m_fun1.fun ( a ) ; }
    /// evaluate the first functor 
    typename functor::result_type fun2 
    ( typename functor::argument a ) const { return m_fun2.fun ( a ) ; }
  public:
    /// get the first functor 
    const functor& fun1 ()           const { return m_fun1.fun (   ) ; }
    /// get the second functor 
    const functor& fun2 ()           const { return m_fun2.fun (   ) ; }
  private:
    // no default constructor 
    TwoFunctors(); ///< no default constructor 
  private:
    // the first functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun1 ; ///< the first functor 
    // the second functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun2 ; ///< the first functor 
  } ;
  // ==========================================================================
  /** @class And 
   *  helper function to implement logical AND of 2 predicates 
   *  
   *  It is used by LoKi for implementation of logical 
   *  binary <tt>&&</tt> operator for predicates
   *
   *  @code 
   *
   *  typedef Functor<SomeType,bool>            Pred ;
   *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
   *
   *  void func( const Pred& A , const Pred& B )
   *  {
   *    PfP p1 = A && B             ; // operator form
   *    PfP p2 = LoKi::And( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE, class TYPE2=bool> 
  class And : public LoKi::Functor<TYPE,bool> 
  {
  private:
    /// the actual type of functor
    typedef LoKi::Functor<TYPE,TYPE2> functor ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
  public:
    /// constructor from two functors 
    And ( const functor& f1 , 
          const functor& f2 ) 
      : LoKi::Functor<TYPE,bool>()
        , m_two ( f1 , f2 ) 
    {}
    /// virtual constructor
    virtual ~And(){}
  public:
    /// MANDATORY: clone method ("")
    virtual And* clone() const { return new And(*this) ; }
    /// MANDATORY: the only one essential method ("function")      
    virtual result_type operator() ( argument a ) const 
    { return m_two.fun1( a ) ? m_two.fun2 ( a )  : false ; }  
    /// OPTIONAL: the nice printout 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.fun1() << "&" << m_two.fun2() << ")" ; }
  private:
    // the default contructor is disabled 
    And() ; ///< the default contructor is disabled 
  private:
    // the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; ///< the storage of twho fucntors 
  };
  // ==========================================================================
  /** @struct Or 
   *  helper function to implement logical OR of 2 predicates 
   *  
   *  It is used by LoKi for implementation of logical 
   *  binary <tt>||</tt> operator for predicates
   *
   *  @code 
   *
   *  typedef Functor<SomeType,bool>            Pred ;
   *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
   *
   *  void func( const Pred& A , const Pred& B )
   *  {
   *    PfP p1 = A || B            ;  // operator form
   *    PfP p2 = LoKi::Or( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  // ==========================================================================
  template<class TYPE, class TYPE2=bool> 
  class Or : public LoKi::Functor<TYPE,bool> 
  {
  private:
    /// the actual type of functor
    typedef LoKi::Functor<TYPE,TYPE2> functor ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
  public:
    /// constructor from twho functors 
    Or ( const functor& f1 , 
         const functor& f2 ) 
      : LoKi::Functor<TYPE,bool>()
      , m_two ( f1 , f2 ) 
    {}
    /// virtual constructor
    virtual ~Or(){}
  public:
    /// MANDATORY: clone method ("")
    virtual Or* clone() const { return new Or(*this) ; }
    /// MANDATORY: the only one essential method ("function")      
    virtual result_type operator() ( argument a ) const 
    { return m_two.fun1 ( a ) ? true : m_two.fun2 ( a ) ; }  
    /// OPTIONAL: the nice printout 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.fun1() << "|" << m_two.fun2() << ")" ; }
  private:
    // the default contructor is disabled 
    Or() ; ///< the default contructor is disabled 
  private:
    // the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; ///< the storage of twho fucntors 
  };
  // ==========================================================================
  /** @class Not
   *
   *  The helper function to implement logical negation 
   *  of the predicate
   *
   *  @code 
   *
   *  typedef Functor<SomeType,bool>            Pred ;
   *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
   *
   *  void func( const Pred& A )
   *  {
   *    PfP p2 = LoKi::Not( A ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE, class TYPE2=bool> 
  class Not : public Functor<TYPE,bool>
  {
  private:
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
  public:
    /// constructor from the functor  
    Not ( const Functor<TYPE,TYPE2>& fun ) 
      : Functor<TYPE,bool>() 
      , m_fun ( fun ) 
    {} 
    /// virtual destructor 
    virtual ~Not() {}
    /// clone method (mandatory)
    virtual  Not* clone() const { return new Not( *this ); }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument a ) const 
    { 
      std::logical_not<TYPE2> lnot ;
      return lnot ( m_fun.fun ( a ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << "(!"  << m_fun << ")" ; };
  private:
    // the default constrictor is disabled
    Not() ; ///< the default constrictor is disabled 
  private:
    // the functor to be negated 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun ; ///< the functor to be negated 
  };  
  // ==========================================================================
  /** @class Negate
   *
   *  The helper function to implement unary negate 
   *  of the function
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   *
   *  void func( const Func& A )
   *  {
   *    FfF p1 = -A                ; // operator form
   *    FfF p2 = LoKi::Negate( A ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE,class TYPE2=double> 
  class Negate : public LoKi::Functor<TYPE,TYPE2>
  {
  private:
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
  public:
    /// constructor from the functor  
    Negate ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
      : LoKi::Functor<TYPE,TYPE2>() 
      , m_fun ( fun ) 
    {} 
    /// virtual destructor 
    virtual ~Negate() {}
    /// clone method (mandatory)
    virtual  Negate* clone() const { return new Negate ( *this ) ; }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument a ) const 
    { 
      std::negate<TYPE2> negator ;
      return negator ( m_fun.fun ( a ) ) ;
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(-"  << m_fun << ")" ; };
  private:
    // the default constrictor is disabled
    Negate() ; ///< the default constrictor is disabled 
  private:
    // the functor to be negated 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun ; ///< the functor to be negated 
  };  
  // ==========================================================================
  /** @class Less 
   *  The helper function to implement Less of 2 functions 
   *
   *  It is used by LoKi for implementation of comparison 
   *  operator for functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,bool>   PfP  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    PfP p1 = A < B               ;  // operator form
   *    PfP p2 = LoKi::Less( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FuctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE, class TYPE2=double> 
  class Less : public LoKi::Functor<TYPE,bool> 
  {
  private:
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
  public:
    /// constructor from two functors 
    Less ( const LoKi::Functor<TYPE,TYPE2>& f1 , 
           const LoKi::Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::Functor<TYPE,bool>()
      , m_two ( f1 , f2 ) 
    {}
    /// constructor from the functor and constant 
    Less ( const LoKi::Functor<TYPE,TYPE2>& f1 , T2 f2 ) 
      : LoKi::Functor<TYPE,bool>()
      , m_two ( f1 , LoKi::Constant<TYPE,TYPE2>( f2 )  ) 
    {}
    /// constructor from the functor and constant 
    Less ( T2 f1 , const LoKi::Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::Functor<TYPE,bool>()
      , m_two ( LoKi::Constant<TYPE,TYPE2>( f1 ) , f2  ) 
    {}
    /// virtual destructor 
    virtual ~Less() {}
    /// clone method (mandatory)
    virtual  Less* clone() const { return new Less( *this ); }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument a ) const 
    { 
      std::less<TYPE2> cmp ;
      return cmp ( m_two.fun1 ( a ) , m_two.fun2 ( a ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.fun1() << "<" << m_two.fun2() << ")" ; }
  private:
    // the default constrictor is disabled
    Less() ; ///< the default constrictor is disabled 
  private:
    // the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; ///< the storage of two functors 
  };
  // ==========================================================================
  /** @class Equal 
   *  The helper function to implement Equal of 2 functions 
   *
   *  It is used by LoKi for implementation of comparison 
   *  operator for functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,bool>   PfP   ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    PfP p1 = A == B               ; // operator form
   *    PfP p2 = LoKi::Equal( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE, class TYPE2=double> 
  class Equal : public LoKi::Functor<TYPE,bool> 
  {
  private:
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    typedef typename boost::call_traits<const TYPE2>::param_type T2 ;
  public:
    /// constructor from two functors 
    Equal ( const LoKi::Functor<TYPE,TYPE2>& f1          , 
            const LoKi::Functor<TYPE,TYPE2>& f2          , 
            T2                               eps = 1.e-6 ) 
      : LoKi::Functor<TYPE,bool>()
      , m_two ( f1 , f2 ) 
      , m_eps ( eps     )
    {}
    /// virtual destructor 
    virtual ~Equal() {}
    /// clone method (mandatory)
    virtual  Equal* clone() const { return new Equal ( *this ) ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument a ) const 
    { return equal ( a ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.fun1() << "==" << m_two.fun2() << ")" ; }
  public:
    /// the actual comparison:
    inline result_type equal ( argument a ) const 
    { 
      //std::equal_to<TYPE2> cmp ;
      LHCb::Math::Equal_To<TYPE2> cmp  ( m_eps ) ;
      return cmp ( m_two.fun1 ( a ) , m_two.fun2 ( a ) ) ; 
    }
  public:
    /// get the helper structure 
    const  LoKi::TwoFunctors<TYPE,TYPE2>& functor () const { return m_two ; }
  private:
    // the default constrictor is disabled
    Equal() ; ///< the default constrictor is disabled 
  private:
    // the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; ///< the storage of two functors 
    // the precision 
    TYPE2 m_eps ; // the precision 
  };
  // ==========================================================================
  /** @struct LessOrEqual 
   *  The helper function to implement Less of 2 functions 
   *
   *  It is used by LoKi for implementation of comparison 
   *  operator for functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,bool>   PfP  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    PfP p1 = A <= B                ;  // operator form
   *    PfP p2 = LoKi::LessOrEqual( A , B ) ; // explicit form
   *  }
   *  @endcode 
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor

   *  @see LoKi::Function
   *  @see LoKi::PredicateFromPredicate
   *  @see LoKi::PredicateFromTwoFunctions
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE, class TYPE2=double> 
  class LessOrEqual : public Functor<TYPE,bool> 
  {
  private:
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
  public:
    /// constructor from twho functors 
    LessOrEqual ( const Functor<TYPE,TYPE2>& f1 , 
                  const Functor<TYPE,TYPE2>& f2 ) 
      : Functor<TYPE,bool>()
      , m_two ( f1 , f2 ) 
    {}
    /// constructor from the functor and constant 
    LessOrEqual ( const LoKi::Functor<TYPE,TYPE2>& f1 , T2 f2 ) 
      : LoKi::Functor<TYPE,bool>()
      , m_two ( f1 , LoKi::Constant<TYPE,TYPE2>( f2 )  ) 
    {}
    /// constructor from the functor and constant 
    LessOrEqual ( T2 f1 , const LoKi::Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::Functor<TYPE,bool>()
      , m_two ( LoKi::Constant<TYPE,TYPE2>( f1 ) , f2  ) 
    {}
    /// virtual destructor 
    virtual ~LessOrEqual() {}
    /// clone method (mandatory)
    virtual  LessOrEqual* clone() const { return new LessOrEqual ( *this ) ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument a ) const 
    { 
      std::less_equal<TYPE2> cmp ;
      return cmp ( m_two.fun1( a ) , m_two.fun2( a ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.fun1() << "<=" << m_two.fun2() << ")" ; }
  private:
    // the default constrictor is disabled
    LessOrEqual() ; ///< the default constrictor is disabled 
  private:
    // the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; ///< the storage of two functors 
  };
  // ==========================================================================
  /** @class NotEqual 
   *  The helper function to implement NotEqual of 2 functions 
   *
   *  It is used by LoKi for implementation of comparison 
   *  operator for functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,bool>   PfP  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    PfP p1 = A != B                  ; // operator form
   *    PfP p2 = LoKi::NotEqual( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE, class TYPE2=double> 
  class NotEqual : public LoKi::Functor<TYPE,bool> 
  {
  private:
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    typedef typename boost::call_traits<const TYPE2>::param_type T2 ;
  public:
    /// constructor from twho functors 
    NotEqual ( const LoKi::Functor<TYPE,TYPE2>& f1 , 
               const LoKi::Functor<TYPE,TYPE2>& f2 , 
               T2 eps = 1.e-6 ) 
      : LoKi::Functor<TYPE,bool>()
      , m_equal ( f1 , f2 , eps ) 
    {}
    /// virtual destructor 
    virtual ~NotEqual() {}
    /// clone method (mandatory)
    virtual  NotEqual* clone() const { return new NotEqual ( *this ) ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument a ) const 
    { return ! ( m_equal.equal ( a ) )  ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "("  << m_equal.functor().fun1() 
               << "!=" << m_equal.functor().fun2() << ")" ; }
  private:
    // the default constrictor is disabled
    NotEqual() ; ///< the default constrictor is disabled 
  private:
    // the underlying functor 
    LoKi::Equal<TYPE,TYPE2> m_equal ; ///< the underlying functor 
  };
  // ==========================================================================
  /** @class Plus 
   *  The helper function to implement addition of 2 function 
   *
   *  It is used by LoKi for implementation of addition of  
   *  2 functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = A  + B              ; // operator form
   *    FfF f2 = LoKi::Plus( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE, class TYPE2=double> 
  class Plus : public LoKi::Functor<TYPE,TYPE2> 
  {
  private:
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
  public:
    /// constructor from two functors 
    Plus ( const LoKi::Functor<TYPE,TYPE2>& f1 , 
           const LoKi::Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( f1 , f2 ) 
    {}
    /// constructor from the functor and constant 
    Plus ( const LoKi::Functor<TYPE,TYPE2>& f1 , T2 f2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( f1 , LoKi::Constant<TYPE,TYPE2> ( f2 )  ) 
    {}
    /// constructor from the functor and constant 
    Plus ( T2 f1 , const LoKi::Functor<TYPE,TYPE2>& f2) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( LoKi::Constant<TYPE,TYPE2> ( f1 ) , f2 ) 
    {}
    /// virtual destructor 
    virtual ~Plus() {}
    /// clone method (mandatory)
    virtual  Plus* clone() const { return new Plus ( *this ) ; }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument a ) const 
    { 
      std::plus<TYPE2> oper ;
      return oper ( m_two.fun1( a ) , m_two.fun2( a ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.fun1() << "+" << m_two.fun2() << ")" ; }
  private:
    // the default constrictor is disabled
    Plus() ; ///< the default constrictor is disabled 
  private:
    // the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; ///< the storage of two functors 
  };
  // ==========================================================================
  /** @class Minus 
   *  The helper function to implement subtraction of 2 function 
   *
   *  It is used by LoKi for implementation of subtraction of  
   *  2 functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = A - B                ; // operator form
   *    FfF f2 = LoKi::Minus( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE, class TYPE2=double> 
  class Minus : public LoKi::Functor<TYPE,TYPE2> 
  {
  private:
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
  public:
    /// constructor from two functors 
    Minus ( const LoKi::Functor<TYPE,TYPE2>& f1 , 
            const LoKi::Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( f1 , f2 ) 
    {}
    /// constructor from the functor and constant 
    Minus ( const LoKi::Functor<TYPE,TYPE2>& f1 , T2 f2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( f1 , LoKi::Constant<TYPE,TYPE2> ( f2 )  ) 
    {}
    /// constructor from the functor and constant 
    Minus ( T2 f1 , const LoKi::Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( LoKi::Constant<TYPE,TYPE2> ( f1 )  , f2 ) 
    {}
    /// virtual destructor 
    virtual ~Minus() {}
    /// clone method (mandatory)
    virtual  Minus* clone() const { return new Minus ( *this ) ; }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument a ) const 
    { 
      std::minus<TYPE2> oper ;
      return oper ( m_two.fun1 ( a ) , m_two.fun2 ( a ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.fun1() << "-" << m_two.fun2() << ")" ; }
  private:
    // the default constrictor is disabled
    Minus() ; ///< the default constrictor is disabled 
  private:
    // the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; ///< the storage of two functors 
  };
  // ==========================================================================
  /** @class Divide
   *  The helper function to implement division of 2 function 
   *
   *  It is used by LoKi for implementation of division of  
   *  2 functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = A / B                 ; // operator form
   *    FfF f2 = LoKi::Divide( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE, class TYPE2=double> 
  class Divide : public LoKi::Functor<TYPE,TYPE2> 
  { 
  private:
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
  public:
    /// constructor from two functors 
    Divide ( const LoKi::Functor<TYPE,TYPE2>& f1 , 
             const LoKi::Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( f1 , f2 ) 
    {}
    /// constructor from the functor adn the constant 
    Divide ( const LoKi::Functor<TYPE,TYPE2>& f1 , T2 f2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( f1 , LoKi::Constant<TYPE,TYPE2> ( f2 ) ) 
    {}
    /// constructor from the functor adn the constant 
    Divide ( T2 f1 , const LoKi::Functor<TYPE,TYPE2>& f2) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( LoKi::Constant<TYPE,TYPE2> ( f1 ) , f2 ) 
    {}
    /// virtual destructor 
    virtual ~Divide() {}
    /// clone method (mandatory)
    virtual  Divide* clone() const { return new Divide ( *this ) ; }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument a ) const 
    { 
      std::divides<TYPE2> oper ;
      return oper ( m_two.fun1 ( a ) , m_two.fun2 ( a ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.fun1() << "/" << m_two.fun2() << ")" ; }
  private:
    // the default constrictor is disabled
    Divide() ; ///< the default constrictor is disabled 
  private:
    // the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; ///< the storage of two functors 
  };
  // ==========================================================================
  /** @class Multiply
   *  The helper function to implement multiplication of 2 functions 
   *
   *  It is used by LoKi for implementation of multiplication of  
   *  2 functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = A * B                   ; // operator form
   *    FfF f2 = LoKi::Multiply( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE, class TYPE2=double> 
  class Multiply : public Functor<TYPE,TYPE2> 
  {
  private:
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
  public:
    /// constructor from two functors 
    Multiply ( const LoKi::Functor<TYPE,TYPE2>& f1 , 
               const LoKi::Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( f1 , f2 ) 
    {}
    /// constructor from the functor and constant 
    Multiply ( const LoKi::Functor<TYPE,TYPE2>& f1 , T2 f2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( f1 , LoKi::Constant<TYPE,TYPE2> ( f2 ) ) 
    {}
    /// constructor from the functor and constant 
    Multiply ( T2 f1 , const LoKi::Functor<TYPE,TYPE2>& f2) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( LoKi::Constant<TYPE,TYPE2> ( f1 ) , f2 ) 
    {}
    /// virtual destructor 
    virtual ~Multiply() {}
    /// clone method (mandatory)
    virtual  Multiply* clone() const { return new Multiply ( *this ) ; }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument a ) const 
    { 
      std::multiplies<TYPE2> oper ;
      return oper ( m_two.fun1 ( a ) , m_two.fun2 ( a ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.fun1() << "*" << m_two.fun2() << ")" ; }
  private:
    // the default constrictor is disabled
    Multiply() ; ///< the default constrictor is disabled 
  private:
    // the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; ///< the storage of two functors 
  };
  // ==========================================================================
  /** @class Min 
   *
   *  Simple functor to find the inimum for the functors 
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   * 
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f2 = LoKi::Min( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFomrFunctor
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-01
   */
  template<class TYPE, class TYPE2=double> 
  class Min : public LoKi::Functor<TYPE,TYPE2>
  {
  private:
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
  public:    
    // ========================================================================
    /** constructor from 2 functions 
     *  @param fun1 the first  function
     *  @param fun2 the second function
     */
    Min ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
          const LoKi::Functor<TYPE,TYPE2>& fun2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( fun1 , fun2 ) 
    {}
    /// constructor from the function and constant 
    Min ( const LoKi::Functor<TYPE,TYPE2>& fun1 , T2 fun2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( fun1 , LoKi::Constant<TYPE,TYPE2> ( fun2 )  ) 
    {}
    /// constructor from the function and constant 
    Min ( T2 fun1 , const LoKi::Functor<TYPE,TYPE2>& fun2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( LoKi::Constant<TYPE,TYPE2> ( fun1 ) , fun2 ) 
    {}
    /** constructor from 3 functions 
     *  @param fun1 the first  function
     *  @param fun2 the second function
     *  @param fun3 the third  function
     */
    Min ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
          const LoKi::Functor<TYPE,TYPE2>& fun2 ,
          const LoKi::Functor<TYPE,TYPE2>& fun3 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( Min ( fun1 , fun2 ) , fun3 ) 
    {}
    /** constructor from 4 functions 
     *  @param fun1 the first function
     *  @param fun2 the second function
     *  @param fun3 the third function
     *  @param fun4 the fourth function
     */
    Min ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
          const LoKi::Functor<TYPE,TYPE2>& fun2 ,
          const LoKi::Functor<TYPE,TYPE2>& fun3 , 
          const LoKi::Functor<TYPE,TYPE2>& fun4 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( Min ( Min ( fun1 , fun2 ) , fun3 ) , fun4 ) 
    {}
    /// virtual destructor 
    virtual ~Min() {}
    /// clone method (mandatory)
    virtual  Min* clone() const { return new Min ( *this ) ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument a ) const 
    { 
      const result_type val1 = m_two . fun1 ( a ) ;
      const result_type val2 = m_two . fun2 ( a ) ;      
      return std::min ( val1 , val2 , std::less<TYPE2>() ) ;
    }    
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "min(" << m_two.fun1() << "," << m_two.fun2() << ")" ; }
  private:
    // the default constructor is disabled 
    Min () ; ///< the default constructor is disabled
  private:
    // the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; ///< the storage of two functors
  };
  // ==========================================================================
  /** @class Max 
   *  Return  the maximum from functions 
   *
   *  Simple functor to find the inimum for the functors 
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   * 
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f2 = LoKi::Max( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFomrFunctor
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-01
   */
  template<class TYPE, class TYPE2=double> 
  class Max : public LoKi::Functor<TYPE,TYPE2>
  {
  private:
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
  public:    
    // ========================================================================
    /** constructor from 2 functions 
     *  @param fun1 the first  function
     *  @param fun2 the second function
     */
    Max ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
          const LoKi::Functor<TYPE,TYPE2>& fun2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( fun1 , fun2 ) 
    {}
    /// constructor from the function and constant 
    Max ( const LoKi::Functor<TYPE,TYPE2>& fun1 , T2 fun2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( fun1 , LoKi::Constant<TYPE,TYPE2> ( fun2 )  ) 
    {}
    /// constructor from the function and constant 
    Max ( T2 fun1 , const LoKi::Functor<TYPE,TYPE2>& fun2 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( LoKi::Constant<TYPE,TYPE2> ( fun1 ) , fun2 ) 
    {}
    /** constructor from 3 functions 
     *  @param fun1 the first  function
     *  @param fun2 the second function
     *  @param fun3 the third  function
     */
    Max ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
          const LoKi::Functor<TYPE,TYPE2>& fun2 ,
          const LoKi::Functor<TYPE,TYPE2>& fun3 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( Max ( fun1 , fun2 ) , fun3 ) 
    {}
    /** constructor from 4 functions 
     *  @param fun1 the first function
     *  @param fun2 the second function
     *  @param fun3 the third function
     *  @param fun4 the fourth function
     */
    Max ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
          const LoKi::Functor<TYPE,TYPE2>& fun2 ,
          const LoKi::Functor<TYPE,TYPE2>& fun3 , 
          const LoKi::Functor<TYPE,TYPE2>& fun4 ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_two ( Max ( Max( fun1 , fun2 ) , fun3 ) , fun4 ) 
    {}
    /// virtual destructor 
    virtual ~Max() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  Max* clone() const { return new Max ( *this ) ; }
    /// MANDATORY: the only one essential method ("function")      
    virtual  result_type operator() ( argument a ) const 
    { 
      const result_type val1 = m_two . fun1 ( a ) ;
      const result_type val2 = m_two . fun2 ( a ) ;      
      return std::max ( val1 , val2 , std::less<TYPE2>() ) ;
    }    
    /// OPTIONAL: the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "max(" << m_two.fun1() << "," << m_two.fun2() << ")" ; }
  private:
    // the default constructor is disabled 
    Max () ; ///< the default constructor is disabled
  private:
    // the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; ///< the storage of two functors
  };
  // ==========================================================================
  /** @class SimpleSwitch
   *  It is a function with acts similar to switch:
   *  
   *  result = condition ? value1 : value2 ;
   *  
   *  In particular it is useful for "conversion" 
   *   of "predicates" into "functions"
   * 
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date 2005-02-11
   */
  template <class TYPE, class TYPE2=double>
  class SimpleSwitch : public LoKi::Functor<TYPE,TYPE2>
  {
  private:
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
  public:
    /** constructor from the predicate and 2 constants 
     *
     *  The function return a value of the first constant  
     *  if predicate evaluated to "true", otherwise the 
     *  second constant is returned 
     *
     *  @param cut predicate for branch
     *  @param val1 the first constant 
     *  @param val2 the second constant 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-11
     */
    SimpleSwitch
    ( const LoKi::Functor<TYPE,bool>& cut  , T2 val1 , T2 val2 )
      : LoKi::Functor<TYPE,TYPE2> (      ) 
      , m_cut  ( cut  ) 
      , m_val1 ( val1 )
      , m_val2 ( val2 )
    {}
    /// copy constructor 
    SimpleSwitch ( const SimpleSwitch& right ) 
      : LoKi::AuxFunBase          ( right )
      , LoKi::Functor<TYPE,TYPE2> ( right ) 
      , m_cut               ( right.m_cut  ) 
      , m_val1              ( right.m_val1 ) 
      , m_val2              ( right.m_val2 ) 
    {}
    /// destructor 
    virtual ~SimpleSwitch() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  SimpleSwitch* clone() const { return new SimpleSwitch ( *this ) ; }
    /// MANDATORY: the only one essential method:
    virtual result_type operator() ( argument object ) const 
    { return m_cut( object ) ? m_val1 : m_val2 ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "switch("  << m_cut << ","  << m_val1 << "," << m_val2 << ")" ; }
  private:
    // the default constructor is disabled    
    SimpleSwitch() ; // the default constructor is disabled    
  private:
    // the condiftion 
    LoKi::FunctorFromFunctor <TYPE,bool> m_cut  ; ///< the condiftion 
    // the first value 
    TYPE2                                m_val1 ; ///< the first value 
    // the second value 
    TYPE2                                m_val2 ; ///< the second value 
  };  
  // ==========================================================================
  /** @class Switch
   *  
   *  It is a bit advances version fo SimpleSwitch function 
   *   with acts similar to switch:
   *  
   *  result = condition ? function1 : function2 ;
   * 
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date 2005-02-11
   */
  template<class TYPE, class TYPE2=double>
  class Switch : public LoKi::Functor<TYPE,TYPE2>
  {
  private:
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
  public:
    /** constructor from predicate and 2 functions 
     *
     *  The function return a value of the first function 
     *  if predicate evaluates to "true", otherwise the 
     *  value of second function is returned 
     *
     *  @param cut predicate for branch
     *  @param fun1 the first  fuction
     *  @param fun2 the second function
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-11
     */
    Switch
    ( const LoKi::Functor<TYPE,bool>&   cut  , 
      const LoKi::Functor<TYPE,TYPE2>&  fun1 , 
      const LoKi::Functor<TYPE,TYPE2>&  fun2 )
      : Functor<TYPE,TYPE2>  (      ) 
      , m_cut  ( cut  ) 
      , m_two  ( fun1 , fun2 )  
    {}    
    /** constructor from predicate ,function and constant
     *
     *  The function return a value of the function 
     *  if predicate evaluates to "true", otherwise the 
     *  value of constant is returned 
     *
     *  @param cut predicate for branch
     *  @param fun1 the function
     *  @param fun2 the constant 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-11
     */
    Switch
    ( const LoKi::Functor<TYPE,bool>&   cut  , 
      const LoKi::Functor<TYPE,TYPE2>&  fun1 , T2 fun2 )
      : LoKi::Functor<TYPE,TYPE2>  (      ) 
      , m_cut  ( cut  ) 
      , m_two  ( fun1 , LoKi::Constant<TYPE,TYPE2>( fun2 ) )  
    {}    
    /** constructor from predicate ,function and constant 
     *
     *  The function return a value of the constant  
     *  if predicate evaluates to "true", otherwise the 
     *  value of function is returned 
     *
     *  @param cut predicate for branch
     *  @param fun1 the constant 
     *  @param fun2 the function
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-11
     */
    Switch
    ( const LoKi::Functor<TYPE,bool>& cut  , 
      T2                              fun1 , 
      const Functor<TYPE,TYPE2>&      fun2 ) 
      : LoKi::Functor<TYPE,TYPE2>  (      ) 
      , m_cut  ( cut  ) 
      , m_two  ( LoKi::Constant<TYPE,TYPE2>( fun1 ) , fun2 )  
    {}
    /** constructor from predicate and 2 constants 
     *
     *  The function return a value of the first constant  
     *  if predicate evaluates to "true", otherwise the 
     *  second constant is returned 
     *
     *  The constructed fuction object has the same 
     *  functionality as SimpleSwitch, but a bit 
     *  less performant. 
     *  @see SimpleSwitch 
     *  
     *  @param cut predicate for branch
     *  @param fun1 the first constant 
     *  @param fun2 the second constant 
     *
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-11
     */
    Switch
    ( const LoKi::Functor<TYPE,bool>&   cut  , T2 fun1 , T2 fun2 ) 
      : LoKi::Functor<TYPE,TYPE2>  (      ) 
      , m_cut  ( cut  ) 
      , m_two  ( LoKi::Constant<TYPE,TYPE2>( fun1 ) ,
                 LoKi::Constant<TYPE,TYPE2>( fun2 ) )
    {}
    /// copy constructor 
    Switch ( const Switch& right ) 
      : LoKi::AuxFunBase          ( right ) 
      , LoKi::Functor<TYPE,TYPE2> ( right ) 
      , m_cut               ( right.m_cut ) 
      , m_two               ( right.m_two ) 
    {}
    /// destructor 
    virtual ~Switch() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  Switch* clone() const { return new Switch ( *this ) ; }
    /// MANDATORY: the only one essential method:
    virtual  result_type operator() ( argument a ) const 
    { return m_cut.fun ( a ) ? m_two.fun1 ( a ) : m_two.fun2 ( a ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "switch("     
               << m_cut         << "," 
               << m_two.fun1()  << "," 
               << m_two.fun2()  << ")" ; }
  private:
    // the default contructor is disabled 
    Switch () ; ///< the defautl contructor is disabled 
  private:
    // the condition 
    LoKi::FunctorFromFunctor<TYPE,bool> m_cut  ; ///< the condition 
    // the actual storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2>       m_two ; /// the storage of functors 
  };
  // ==========================================================================
  /** @class ComposeFunction
   *  The helper class to implement function of function
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = sin( A )                         ; // short form
   *    FfF f2 = LoKi::ComposeFunction( sin , A ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFronFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE, class TYPE2=double>
  class ComposeFunction : public LoKi::Functor<TYPE,TYPE2> 
  {
  private:
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
  protected:
    /// the actual type for the function
    typedef double (*Func) ( double ) ;
  public:
    /// constructor 
    ComposeFunction ( Func                             func           ,  
                      const LoKi::Functor<TYPE,TYPE2>& fun            , 
                      const std::string&               desc = "'fun'" ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_fun  ( fun  ) 
      , m_func ( func ) 
      , m_desc ( desc ) 
    {}     
    /// copy contructor
    ComposeFunction ( const ComposeFunction& right ) 
      : LoKi::AuxFunBase          ( right )      
      , LoKi::Functor<TYPE,TYPE2> ( right )
      , m_fun                     ( right.m_fun  ) 
      , m_func                    ( right.m_func ) 
      , m_desc                    ( right.m_desc ) 
    {}
    /// virtual destructor 
    virtual ~ComposeFunction(){}
    /// clone method (mandatory!)
    virtual ComposeFunction*  clone () const 
    { return new ComposeFunction ( *this ) ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument a ) const 
    { return (*m_func) ( m_fun . fun ( a ) ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << m_desc << "("  << m_fun << ")" ; };
  private:
    // the default constructor is disabled 
    ComposeFunction() ; ///< the default constructor is disabled
  private:
    // the functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun  ; ///< the functor 
    // the function:
    Func                                 m_func ; ///< the function:
    // the function description:
    std::string                          m_desc ; ///< description 
  };
  // ==========================================================================
  /** @class ComposeFunction2
   *
   *  The helper structure to implement function of function
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<Sometype,double> FfF ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = pow( A , B )                          ; // short form
   *    FfF f2 = LoKi::ComposeFunction2 ( pow , A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE, class TYPE2=double>
  class ComposeFunction2 : public LoKi::Functor<TYPE,TYPE2> 
  {
  private:
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    /// the actual type of the function 
    typedef double (*Func) ( double , double )      ;
    /// constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
  public:
    /// constructor 
    ComposeFunction2 
    ( Func                             func           ,  
      const LoKi::Functor<TYPE,TYPE2>& fun1           , 
      const LoKi::Functor<TYPE,TYPE2>& fun2           , 
      const std::string&               desc = "'fun'" )
      : LoKi::Functor<TYPE,TYPE2>() 
      , m_func ( func ) 
      , m_two  ( fun1 ,fun2 )
      , m_desc ( desc ) 
    {}
    /// constructor 
    ComposeFunction2 
    ( Func                             func            ,  
      const LoKi::Functor<TYPE,TYPE2>& fun1            , 
      T2                               val2            , 
      const std::string&               desc  = "'fun'" )
      : LoKi::Functor<TYPE,TYPE2>() 
      , m_func ( func ) 
      , m_two  ( fun1 , LoKi::Constant<TYPE,TYPE2> ( val2 ) )
      , m_desc ( desc ) 
    {}
    /// constructor 
    ComposeFunction2 
    ( Func                             func            ,  
      T2                               val1            , 
      const LoKi::Functor<TYPE,TYPE2>& fun2            , 
      const std::string&               desc  = "'fun'" )
      : LoKi::Functor<TYPE,TYPE2>() 
      , m_func ( func ) 
      , m_two  ( LoKi::Constant<TYPE,TYPE2> ( val1 ) , fun2 )
      , m_desc ( desc ) 
    {}
    /// constructor 
    ComposeFunction2 
    ( Func                             func            ,  
      T2                               val1            , 
      T2                               val2            , 
      const std::string&               desc  = "'fun'" )
      : LoKi::Functor<TYPE,TYPE2>() 
      , m_func ( func ) 
      , m_two  ( LoKi::Constant<TYPE,TYPE2> ( val1 ) ,
                 LoKi::Constant<TYPE,TYPE2> ( val2 ) )
      , m_desc ( desc ) 
    {}
    /// copy constructor 
    ComposeFunction2 ( const ComposeFunction2& right ) 
      : LoKi::AuxFunBase ( right ) 
      , LoKi::Functor<TYPE,TYPE2>( right ) 
      , m_func ( right.m_func ) 
      , m_two  ( right.m_two  ) 
      , m_desc ( right.m_desc ) 
    {}
    /// virtual destructor 
    virtual ~ComposeFunction2() {}
    /// clone method (mandatory!)
    virtual ComposeFunction2*  clone   () const 
    { return new ComposeFunction2( *this ); }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument a ) const 
    { return (*m_func) ( m_two. fun1 ( a ) , m_two.fun2( a ) ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << m_desc        << "("  
               << m_two.fun1 () << "," 
               << m_two.fun2 () << ")" ; }
  private:
    // the function itself 
    Func m_func        ; ///< the function itself 
    // the storage of functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; ///< the storage of functors 
    // the funtion descrition
    std::string m_desc ; ///< the funtion descrition
  };  
  // ==========================================================================
  /** @class Compose 
   *  the general case of fun2(fun1) function:
   */  
  template <class TYPE,class TYPE1, class TYPE2, class TYPE3=TYPE1>
  class Compose : public LoKi::Functor<TYPE,TYPE2>
  {
  private:
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
  public:
    /// contructor
    Compose
    ( const LoKi::Functor<TYPE,TYPE1>&  fun1 , 
      const LoKi::Functor<TYPE3,TYPE2>& fun2 )
      : LoKi::Functor<TYPE,TYPE2> () 
      , m_fun1 ( fun1 ) 
      , m_fun2 ( fun2 )
    {}
    /// copy constructor
    Compose ( const Compose& right ) 
      : LoKi::AuxFunBase ( right ) 
      , LoKi::Functor<TYPE,TYPE2> ( right ) 
      , m_fun1 ( right.m_fun1 ) 
      , m_fun2 ( right.m_fun2 )
    {}
    /// MANDATORY: virtual destructor
    virtual ~Compose() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  Compose* clone() const { return new Compose ( *this ) ; }    
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument a ) const 
    { 
      const LoKi::Apply<TYPE,TYPE1>  f1 ( &m_fun1.fun() ) ;
      const LoKi::Apply<TYPE3,TYPE2> f2 ( &m_fun2.fun() ) ;
      return f2.eval ( f1.eval ( a ) ) ;
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "(" << m_fun1 << ">>" << m_fun2  << ")" ; }
    public:
    // the first functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE1>  m_fun1  ; ///< the first functor 
    // the second functor 
    LoKi::FunctorFromFunctor<TYPE3,TYPE2> m_fun2 ; ///< the second functor 
  } ;
  // ==========================================================================
  template <class TYPE1, class TYPE2, class TYPE3>
  class Compose<void,TYPE1,TYPE2,TYPE3> : public LoKi::Functor<void,TYPE2>
  {
  private:
    /// result type 
    typedef typename LoKi::Functor<void,TYPE2>::result_type result_type ; 
  public:
    /// contructor
    Compose
    ( const LoKi::Functor<void,TYPE1>&  fun1 , 
      const LoKi::Functor<TYPE3,TYPE2>& fun2 )
      : LoKi::Functor<void,TYPE2> () 
      , m_fun1 ( fun1 ) 
      , m_fun2 ( fun2 )
    {}
    /// copy constructor
    Compose ( const Compose& right ) 
      : LoKi::AuxFunBase ( right ) 
      , LoKi::Functor<void,TYPE2> ( right ) 
      , m_fun1 ( right.m_fun1 ) 
      , m_fun2 ( right.m_fun2 )
    {}
    /// MANDATORY: virtual destructor
    virtual ~Compose() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  Compose* clone() const { return new Compose ( *this ) ; }    
    /// the only one essential method ("function")      
    virtual  result_type operator() () const 
    { 
      const LoKi::Apply<TYPE3,TYPE2> f2 ( &m_fun2.fun() ) ;
      return f2.eval ( m_fun1.fun() ) ;
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_fun1 << ">>" << m_fun2  << ")" ; }
  public:
    // the first functor 
    LoKi::FunctorFromFunctor<void,TYPE1>  m_fun1  ; ///< the first functor 
    // the second functor 
    LoKi::FunctorFromFunctor<TYPE3,TYPE2> m_fun2 ; ///< the second functor 
  } ;
  // ==========================================================================
  /** @class Valid 
   *  The trivial predicate to check the validity of argument.
   *  It is OK for any pointer-like types or for types with
   *  implemented implicit conversion to "bool" 
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date 2004-02-11
   */
  template <class TYPE>
  class Valid : public LoKi::Functor<TYPE,bool>
  {
  private:
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
  public:
    /// constructor 
    Valid() 
      : Functor<TYPE,bool> () 
    {} 
    /// copy constructor 
    Valid( const Valid& right ) 
      : LoKi::AuxFunBase   ( right ) 
      , Functor<TYPE,bool> ( right ) 
    {}
    /// virtual destructor 
    virtual ~Valid() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  Valid* clone() const { return new Valid( *this ) ; }
    /// MANDATORY: the only one essential method 
    virtual result_type operator() ( argument a ) const 
    { return LoKi::valid ( a ) ? true : false  ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(Valid?)"; }
  };
  // ==========================================================================
  /** @class TheSame 
   *  Trivial predicate to check if the argument 
   *  is equal to some predefined value. 
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date 2004-02-11
   */
  template <class TYPE>
  class TheSame : public LoKi::Functor<TYPE,bool>
  {
  private:
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
  public:
    /// constructor form the value 
    TheSame ( argument value ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_value ( value ) 
    {}
    /// copy constructor 
    TheSame
    ( const TheSame& right ) 
      : LoKi::AuxFunBase         ( right ) 
      , LoKi::Functor<TYPE,bool> ( right )
      , m_value                  ( right.m_value )
    {}
    /// virtual destructor 
    virtual ~TheSame() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  TheSame* clone() const { return new TheSame( *this ) ; }
    /// MANDATORY: the only one essential method 
    virtual result_type operator() ( argument object ) const 
    { return LoKi::same ( m_value , object ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(SAME?)"; }
  private :
    // the default contructor is disabled
    TheSame();
  private:
    // the predefined value 
    TYPE m_value ;
  };
  // ==========================================================================
  /** @class EqualToValue 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-04-07
   */
  template <class TYPE, class TYPE2=double>
  class EqualToValue : public LoKi::Functor<TYPE,bool>
  {
  private:
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
  public:
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     */
    EqualToValue 
    ( const LoKi::Functor<TYPE,TYPE2>&  fun          , 
      T2                                val          , 
      T2                                eps = 1.0e-6 ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun ) 
      , m_val ( val ) 
      , m_eps ( eps ) 
    {}
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     */
    EqualToValue 
    ( T2 val , const LoKi::Functor<TYPE,TYPE2>&  fun , T2 eps = 1.0e-6 ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun ) 
      , m_val ( val ) 
      , m_eps ( eps ) 
    {}
    /// copy constructor 
    EqualToValue 
    ( const EqualToValue& right )
      : LoKi::AuxFunBase          ( right ) 
      , LoKi::Functor<TYPE,bool>  ( right )
      , m_fun ( right.m_fun ) 
      , m_val ( right.m_val )
      , m_eps ( right.m_eps )
    {}
    /// MANDATORY: virtual destructor 
    virtual ~EqualToValue(){} ;
    /// MANDATORY: clone method ("virtual construcor")
    virtual  EqualToValue* clone() const { return new EqualToValue(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument a ) const
    { return equal_to ( a ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << "(" << m_fun << "==" << m_val << ")" ; }
  public:
    inline result_type equal_to ( argument a ) const
    {
      //std::equal_to<TYPE2> cmp ;
      LHCb::Math::Equal_To<TYPE2> cmp ( m_eps ) ;
      return cmp ( m_fun.fun ( a ) , m_val ) ;
    }    
  public:
    const LoKi::Functor<TYPE,TYPE2>& fun () const { return m_fun.fun() ; }
    T2                               val () const { return m_val       ; }
  private:
    /// The default constructor is disabled 
    EqualToValue();
  private:
    // the functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun ; ///< the functor 
    // the value 
    TYPE2  m_val ; ///< the value 
    // the precision 
    TYPE2  m_eps ; /// the precision 
  };  
  // ==========================================================================
  /** @class NotEqualToValue 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-04-07
   */
  template <class TYPE, class TYPE2=double>
  class NotEqualToValue : public LoKi::Functor<TYPE,bool>
  {
  private:
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
  public:
    /** constructor for the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     */
    NotEqualToValue
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , T2 val , T2 eps = 1.e-6 ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_equal ( fun , val , eps ) 
    {}
    /** constructor from the function and the value 
     *  @param val the reference value 
     */
    NotEqualToValue
    ( T2 val , const LoKi::Functor<TYPE,TYPE2>&  fun , T2 eps = 1.e-6 ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_equal ( fun , val , eps ) 
    {}
    /// copy contructor 
    NotEqualToValue 
    ( const NotEqualToValue& right )
      : LoKi::AuxFunBase         ( right ) 
      , LoKi::Functor<TYPE,bool> ( right )
      , m_equal ( right.m_equal )
    {}
    /// MANDATORY: virtual destructor 
    virtual ~NotEqualToValue(){} ;
    /// MANDATORY: clone method ("virtual constructor")
    virtual  NotEqualToValue* clone() const { return new NotEqualToValue(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument a ) const
    {
      std::logical_not<result_type> cmp ;
      return cmp ( m_equal.equal_to ( a ) ) ;
    }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << "("  << m_equal.fun () 
               << "!=" << m_equal.val () << ")" ; }
  private:
    /// The default constructor is disabled 
    NotEqualToValue();
  private:
    LoKi::EqualToValue<TYPE,TYPE2> m_equal ;
  };
  // ==========================================================================
  /** compare 2 objects using the comparison criteria CMP , 
   *  applied to functions:
   *  <c> cmp( f1(a) , f2(b) ) </c>
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */ 
  template<class TYPE , class CMP=std::less<double> , class TYPE2=double>
  class Compare : public std::binary_function<const TYPE,const TYPE,bool>
  {
  public:
    /// typedef for actual function 
    typedef LoKi::Functor<TYPE,TYPE2>   function ;
    // typedef for comparison criteria 
    typedef const CMP            compare  ;
  public:
    /** constructor 
     *  @param fun1 the first functor 
     *  @param fun2 the second functor 
     */
    Compare ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
              const LoKi::Functor<TYPE,TYPE2>& fun2 ,              
              const compare&  cmp  = compare() )
      : m_two ( fun1 , fun2 ) 
      , m_cmp ( cmp ) 
    {}
    /// copy constructor 
    Compare ( const Compare& right ) 
      : m_two ( right.m_two ) 
      , m_cmp ( right.m_cmp ) 
    {}
    /// destructor 
    virtual ~Compare() {}
    /// the only one essential method 
    bool operator() 
      ( typename LoKi::Functor<TYPE,TYPE2>::argument a1 , 
        typename LoKi::Functor<TYPE,TYPE2>::argument a2 ) const
    { return m_cmp ( m_two.fun1 ( a1 ) , m_two.fun2 ( a2 ) ) ; }
  private:
    // no default constructor 
    Compare(){}
  private:
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ;
    compare   m_cmp  ;
  };
  // ==========================================================================
  /** @class Identity
   *  the simple trivial functor 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-04-07
   */
  template <class TYPE,class TYPE2=TYPE>
  class Identity : public LoKi::Functor<TYPE,TYPE2>
  {
  public :
    /// MANDATORY: virtual destrcutor 
    virtual ~Identity(){}
    /// MANDATORY: clone mehtod ("virtual constructor")
    virtual  Identity* clone () const { return new Identity(*this) ; }
    /// MANDATORY": the only one essential method 
    virtual  typename LoKi::Functor<TYPE,TYPE2>::result_type operator () 
      ( typename LoKi::Functor<TYPE,TYPE2>::argument a ) const { return a ; }    
  private:
  } ;
  // ==========================================================================
  /** @class PrintOut
   *  the simple functor, which "converts" the objects to strings 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-04-07
   */
  template <class TYPE>
  class PrintOut : public LoKi::Functor<TYPE,std::string>
  {
  public:
    /// MANDATORY: virtual destrcutor 
    virtual ~PrintOut(){}
    /// MANDATORY: clone mehtod ("virtual constructor")
    virtual  PrintOut* clone () const { return new PrintOut ( *this ) ; }
    /// MANDATORY": the only one essential method 
    virtual  typename LoKi::Functor<TYPE,std::string>::result_type operator () 
      ( typename LoKi::Functor<TYPE,std::string>::argument a ) const 
    { 
      return Gaudi::Utils::toString ( a  ) ; 
    }    
  };
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PRIMITIVES_H
// ============================================================================
