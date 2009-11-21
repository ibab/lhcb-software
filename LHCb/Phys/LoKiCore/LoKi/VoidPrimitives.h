// $Id: VoidPrimitives.h,v 1.6 2009-11-21 12:39:37 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_VOIDPRIMITIVES_H 
#define LOKI_VOIDPRIMITIVES_H 1
// ============================================================================
// Include files
// ============================================================================
/** @file 
 *  The template specialization of basic primitives for "void" arguments 
 *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
 *  @date 2008-10-17
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  template <class TYPE2>
  class TwoFunctors<void,TYPE2>
  {
  private:
    // ========================================================================
    /// the actual type of underlying functor 
    typedef LoKi::Functor<void,TYPE2> functor ;
    // ========================================================================
  public:
    // ========================================================================
    TwoFunctors ( const functor& f1 , 
                  const functor& f2 ) 
      : m_fun1  ( f1 ) 
      , m_fun2  ( f2 ) 
    {}
    /// destructor 
    virtual ~TwoFunctors() {}
    // ========================================================================
  public:
    // ========================================================================
    /// evaluate the first functor 
    typename functor::result_type fun1 () const { return m_fun1.fun ( /* a */ ) ; }
    /// evaluate the first functor 
    typename functor::result_type fun2 () const { return m_fun2.fun ( /* a */ ) ; }
    // ========================================================================
  public:
    // ========================================================================
    /// get the first functor 
    const functor& func1 ()           const { return m_fun1.func () ; }
    /// get the second functor 
    const functor& func2 ()           const { return m_fun2.func () ; }
    // ========================================================================
  private:
    // ========================================================================
    /// no default constructor 
    TwoFunctors () ;                                  // no default constructor 
    // ========================================================================
  private:
    // ========================================================================
    /// the first functor 
    LoKi::FunctorFromFunctor<void,TYPE2> m_fun1 ;          // the first functor 
    /// the second functor 
    LoKi::FunctorFromFunctor<void,TYPE2> m_fun2 ;         // the second functor 
    // ========================================================================
  } ;
  // ==========================================================================  
  template<class TYPE2> 
  class And<void,TYPE2> : public LoKi::Functor<void,bool> 
  {
  private:
    // ========================================================================
    typedef void TYPE  ;
    /// the actual type of functor
    typedef LoKi::Functor<TYPE,TYPE2> functor ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from two functors 
    And ( const functor& f1 , 
          const functor& f2 ) 
      : LoKi::Functor<TYPE,bool>()
      , m_two ( f1 , f2 ) 
    {}
    /// virtual constructor
    virtual ~And(){}
    // ========================================================================
  public:
    // ========================================================================
    /// MANDATORY: clone method ("")
    virtual And* clone() const { return new And(*this) ; }
    /// MANDATORY: the only one essential method ("function")      
    virtual result_type operator() ( /* argument a */ ) const 
    { return m_two.fun1 ( /* a */ ) ? m_two.fun2 ( /* a */ )  : false ; }  
    /// OPTIONAL: the nice printout 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.func1() << "&" << m_two.func2() << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default contructor is disabled 
    And () ; // the default contructor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; // the storage of twho fucntors 
    // ========================================================================
  };
  // ==========================================================================
  template<class TYPE2> 
  class Or<void,TYPE2> : public LoKi::Functor<void,bool> 
  {
  private:
    // ========================================================================
    typedef void TYPE ;    
    /// the actual type of functor
    typedef LoKi::Functor<TYPE,TYPE2> functor ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
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
    virtual result_type operator() ( /* argument a */ ) const 
    { return m_two.fun1 ( /* a */ ) ? true : m_two.fun2 ( /* a */ ) ; }  
    /// OPTIONAL: the nice printout 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.func1() << "|" << m_two.func2() << ")" ; }
  private:
    // ========================================================================
    /// the default contructor is disabled 
    Or() ; // the default contructor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; // the storage of twho fucntors 
    // ========================================================================
  };
  // ==========================================================================
  template<class TYPE2> 
  class Not<void,TYPE2> : public Functor<void,bool>
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
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
    virtual  result_type operator() ( /* argument a */ ) const 
    { 
      std::logical_not<TYPE2> lnot ;
      return lnot ( m_fun.fun ( /* a */ ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << "(!"  << m_fun << ")" ; };
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    Not() ;                              // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the functor to be negated 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun ; // the functor to be negated 
    // ========================================================================
  };  
  // ==========================================================================
  template<class TYPE2> 
  class Negate<void,TYPE2> : public LoKi::Functor<void,TYPE2>
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
    virtual  result_type operator() ( /* argument  a */ ) const 
    { 
      std::negate<TYPE2> negator ;
      return negator ( m_fun.fun ( /* a */ ) ) ;
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(-"  << m_fun << ")" ; };
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    Negate() ; // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the functor to be negated 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun ; // the functor to be negated 
    // ========================================================================
  };
  // ==========================================================================
  template<class TYPE2> 
  class Less<void,TYPE2> : public LoKi::Functor<void,bool> 
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
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
    virtual result_type operator() ( /* argument  a */ ) const 
    { 
      std::less<TYPE2> cmp ;
      return cmp ( m_two.fun1 ( /* a */ ) , m_two.fun2 ( /* a */ ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.func1() << "<" << m_two.func2() << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constrictor is disabled
    Less() ; // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; // the storage of two functors 
    // ========================================================================
  };
  // ==========================================================================
  template<class TYPE2> 
  class Equal<void,TYPE2> : public LoKi::Functor<void,bool> 
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    typedef typename boost::call_traits<const TYPE2>::param_type T2 ;
    // ========================================================================
  public:
    // ========================================================================
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
    virtual result_type operator() ( /* argument a */ ) const 
    { return equal ( /* a */ ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.func1() << "==" << m_two.func2() << ")" ; }
    // ========================================================================
  public:
    // ========================================================================
    /// the actual comparison:
    inline result_type equal ( /* argument a */ ) const 
    { 
      LHCb::Math::Equal_To<TYPE2> cmp  ( m_eps ) ;
      return cmp ( m_two.fun1 ( /* a */ ) , m_two.fun2 ( /* a */ ) ) ; 
    }
    // ========================================================================
  public:
    // ========================================================================
    /// get the helper structure 
    const  LoKi::TwoFunctors<TYPE,TYPE2>& functor () const { return m_two ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constrictor is disabled
    Equal() ;                            // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ;        // the storage of two functors 
    /// the precision 
    TYPE2 m_eps ;                                              // the precision 
    // ========================================================================
  };
  // ==========================================================================
  template<class TYPE2> 
  class LessOrEqual<void,TYPE2> : public Functor<void,bool> 
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
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
    virtual result_type operator() ( /* argument a */ ) const 
    { 
      std::less_equal<TYPE2> cmp ;
      return cmp ( m_two.fun1 ( /* a */ ) , m_two.fun2 ( /* a */ ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.func1() << "<=" << m_two.func2() << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    LessOrEqual() ; // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; // the storage of two functors 
    // ========================================================================
  };
  // ==========================================================================
  template<class TYPE2> 
  class NotEqual<void,TYPE2> : public LoKi::Functor<void,bool> 
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    typedef typename boost::call_traits<const TYPE2>::param_type T2 ;
    // ========================================================================
  public:
    // ========================================================================
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
    virtual result_type operator() ( /* argument  a */ ) const 
    { return ! ( m_equal.equal ( /* a */ ) )  ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "("  << m_equal.functor().func1() 
               << "!=" << m_equal.functor().func2() << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    NotEqual() ; // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the underlying functor 
    LoKi::Equal<TYPE,TYPE2> m_equal ; // the underlying functor 
    // ========================================================================
  };
  // ==========================================================================
  template<> 
  class Plus<void,double> : public LoKi::Functor<void,double> 
  {
  private:
    // ========================================================================
    typedef void   TYPE ;
    typedef double TYPE2 ;
    /// the constant type 
    typedef LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
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
    virtual  result_type operator() ( /* argument a */ ) const 
    { 
      std::plus<TYPE2> oper ;
      return oper ( m_two.fun1 ( /* a */ ) , m_two.fun2 ( /* a */ ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.func1() << "+" << m_two.func2() << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constrictor is disabled
    Plus() ; // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; // the storage of two functors 
    // ========================================================================
  };
  // ==========================================================================
  template<> 
  class Minus<void,double> : public LoKi::Functor<void,double> 
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    typedef double TYPE2 ;
    /// the constant type 
    typedef LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
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
    virtual  result_type operator() ( /* argument  a */ ) const 
    { 
      std::minus<TYPE2> oper ;
      return oper ( m_two.fun1 ( /* a */ ) , m_two.fun2 ( /* a */ ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.func1() << "-" << m_two.func2() << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constrictor is disabled
    Minus() ; // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; // the storage of two functors 
    // ========================================================================
  };
  // ==========================================================================
  template<> 
  class Divide<void,double> : public LoKi::Functor<void,double> 
  { 
  private:
    // ========================================================================
    typedef void   TYPE ;
    typedef double TYPE2 ;
    /// the constant type 
    typedef LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
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
    virtual  result_type operator() ( /* argument  a */ ) const 
    { 
      std::divides<TYPE2> oper ;
      return oper ( m_two.fun1 ( /* a */ ) , m_two.fun2 ( /* a */ ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.func1() << "/" << m_two.func2() << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constrictor is disabled
    Divide() ; // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; // the storage of two functors 
    // ========================================================================
  } ;
  // ==========================================================================
  template<> 
  class Multiply<void,double> : public Functor<void,double> 
  {
  private:
    // ========================================================================
    typedef void   TYPE  ;
    typedef double TYPE2 ;
    /// the constant type 
    typedef LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
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
    virtual  result_type operator() ( /* argument a */ ) const 
    { 
      std::multiplies<TYPE2> oper ;
      return oper ( m_two.fun1 ( /* a */ ) , m_two.fun2 ( /* a */ ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_two.func1() << "*" << m_two.func2() << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constrictor is disabled
    Multiply() ; // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; // the storage of two functors 
    // ========================================================================
  };
  // ==========================================================================
  template<class TYPE2> 
  class Min<void,TYPE2> : public LoKi::Functor<void,TYPE2>
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
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
    virtual result_type operator() ( /* argument  a */ ) const 
    { 
      const result_type val1 = m_two . fun1 ( /* a */ ) ;
      const result_type val2 = m_two . fun2 ( /* a */ ) ;      
      return std::min ( val1 , val2 , std::less<TYPE2>() ) ;
    }    
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "min(" << m_two.func1() << "," << m_two.func2() << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Min () ; // the default constructor is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; // the storage of two functors
    // ========================================================================
  };
  // ==========================================================================
  template<class TYPE2> 
  class Max<void,TYPE2> : public LoKi::Functor<void,TYPE2>
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
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
    virtual  result_type operator() ( /* argument a */ ) const 
    { 
      const result_type val1 = m_two . fun1 ( /* a */ ) ;
      const result_type val2 = m_two . fun2 ( /* a */ ) ;      
      return std::max ( val1 , val2 , std::less<TYPE2>() ) ;
    }    
    /// OPTIONAL: the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "max(" << m_two.func1() << "," << m_two.func2() << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Max () ;                            // the default constructor is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ;       // the storage of two functors
    // ========================================================================
  };
  // ==========================================================================
  template <class TYPE2>
  class SimpleSwitch<void,TYPE2> : public LoKi::Functor<void,TYPE2>
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
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
    virtual result_type operator() ( /* argument a */ ) const 
    { return m_cut ( /* a */ ) ? m_val1 : m_val2 ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "switch("  << m_cut << ","  << m_val1 << "," << m_val2 << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled    
    SimpleSwitch() ; // the default constructor is disabled    
    // ========================================================================
  private:
    // ========================================================================
    /// the condition 
    LoKi::FunctorFromFunctor <TYPE,bool> m_cut  ; // the condiftion 
    /// the first value 
    TYPE2                                m_val1 ; // the first value 
    /// the second value 
    TYPE2                                m_val2 ; // the second value 
    // ========================================================================
  };  
  // ==========================================================================
  template<class TYPE2>
  class Switch<void,TYPE2> : public LoKi::Functor<void,TYPE2>
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
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
    virtual  result_type operator() ( /* argument a */ ) const 
    { return m_cut.fun  ( /* a */ ) ? 
        m_two.fun1 ( /* a */ ) : m_two.fun2 ( /* a */ ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "switch("     
               << m_cut          << "," 
               << m_two.func1()  << "," 
               << m_two.func2()  << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default contructor is disabled 
    Switch () ; // the defautl contructor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the condition 
    LoKi::FunctorFromFunctor<TYPE,bool> m_cut  ;              // the condition 
    /// the actual storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2>       m_two ;      // the storage of functors 
    // ========================================================================
  };
  // ==========================================================================
  template <class TYPE2>
  class ComposeFunction<void,TYPE2> : public LoKi::Functor<void,TYPE2> 
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  protected:
    // ========================================================================
    /// the actual type for the function
    typedef double (*Func) ( double ) ;
    // ========================================================================
  public:
    // ========================================================================
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
    virtual result_type operator() ( /* argument a */ ) const 
    { return (*m_func) ( m_fun . fun ( /* a */ ) ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << m_desc << "("  << m_fun << ")" ; };
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    ComposeFunction() ; // the default constructor is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun  ; // the functor 
    /// the function:
    Func                                 m_func ; // the function:
    /// the function description:
    std::string                          m_desc ; // description 
    // ========================================================================
  };
  // ==========================================================================
  template <class TYPE2>
  class ComposeFunction2<void,TYPE2> : public LoKi::Functor<void,TYPE2> 
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    /// the actual type of the function 
    typedef double (*Func) ( double , double )      ;
    /// constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
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
    virtual result_type operator() ( /* argument a */ ) const 
    { return (*m_func) ( m_two. fun1 ( /* a */ ) , m_two.fun2( /* a */ ) ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << m_desc         << "("  
               << m_two.func1 () << "," 
               << m_two.func2 () << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the function itself 
    Func m_func        ; // the function itself 
    /// the storage of functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; // the storage of functors 
    /// the funtion descrition
    std::string m_desc ; // the funtion descrition
    // ========================================================================
  };
  // ==========================================================================
  template <class TYPE2>
  class EqualToValue<void,TYPE2> : public LoKi::Functor<void,bool>
  {
  private:
    // ========================================================================
    typedef  void TYPE ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
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
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    EqualToValue 
    ( T2 val , const LoKi::Functor<TYPE,TYPE2>&  fun , T2 eps = 1.0e-6 ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun ) 
      , m_val ( val ) 
      , m_eps ( eps ) 
    {}
    // ========================================================================
    /// copy constructor 
    EqualToValue 
    ( const EqualToValue& right )
      : LoKi::AuxFunBase          ( right ) 
      , LoKi::Functor<TYPE,bool>  ( right )
      , m_fun ( right.m_fun ) 
      , m_val ( right.m_val )
      , m_eps ( right.m_eps )
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~EqualToValue(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  EqualToValue* clone() const { return new EqualToValue(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( /* argument a */ ) const
    { return equal_to ( /* a */ ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << "(" << m_fun << "==" << m_val << ")" ; }
    // ========================================================================
  public:
    // ========================================================================
    inline result_type equal_to ( /* argument a */ ) const
    {
      //std::equal_to<TYPE2> cmp ;
      LHCb::Math::Equal_To<TYPE2> cmp ( m_eps ) ;
      return cmp ( m_fun.fun ( /* a */ ) , m_val ) ;
    }    
    // ========================================================================
  public:
    // ========================================================================
    const LoKi::Functor<TYPE,TYPE2>& func () const { return m_fun.func() ; }
    const TYPE2&                     val  () const { return m_val        ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    EqualToValue();
    // ========================================================================
  private:
    // ========================================================================
    /// the functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun ;                 // the functor 
    /// the value 
    TYPE2  m_val ;                                                 // the value 
    /// the precision 
    TYPE2  m_eps ;                                             // the precision 
    // ========================================================================
  };
  // ==========================================================================
  template <class TYPE2>
  class NotEqualToValue<void,TYPE2> : public LoKi::Functor<void,bool>
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor for the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision 
     */
    NotEqualToValue
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , T2 val , T2 eps = 1.e-6 ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_equal ( fun , val , eps ) 
    {}
    /** constructor from the function and the value 
     *  @param val the reference value 
     *  @param fun the function
     *  @param eps the relative precision 
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
    virtual  result_type operator() ( /* argument a */ ) const
    {
      std::logical_not<result_type> cmp ;
      return cmp ( m_equal.equal_to ( /* a */ ) ) ;
    }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << "("  << m_equal.func () 
               << "!=" << m_equal.val  () << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    NotEqualToValue();
    // ========================================================================
  private:
    // ========================================================================
    LoKi::EqualToValue<TYPE,TYPE2> m_equal ;
    // ========================================================================
  };
  // ==========================================================================
  template <class TYPE2>
  class EqualToInt<void,TYPE2>: public LoKi::Functor<void,bool>
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    EqualToInt
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , 
      const int                         val ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun ) 
      , m_val ( val ) 
    {}
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    EqualToInt
    ( const int val , const LoKi::Functor<TYPE,TYPE2>&  fun ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun ) 
      , m_val ( val ) 
    {}
    // ========================================================================
    /// copy constructor 
    EqualToInt
    ( const EqualToInt& right )
      : LoKi::AuxFunBase          ( right ) 
      , LoKi::Functor<TYPE,bool>  ( right )
      , m_fun ( right.m_fun ) 
      , m_val ( right.m_val )
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~EqualToInt(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  EqualToInt* clone() const { return new EqualToInt(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( /* argument a */ ) const
    { return equal_to ( /* a */ ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << "(" << m_val << "==" << m_fun << ")" ; }
    // ========================================================================
  public:
    // ========================================================================
    inline result_type equal_to ( /* argument a */ ) const
    {
      // evaluate the function 
      const typename LoKi::Functor<TYPE,TYPE2>::result_type r = 
        m_fun.fun ( /* a */ ) ;
      return LHCb::Math::equal_to_int ( r , m_val ) ;
    }    
    // ========================================================================
  public:
    // ========================================================================
    const LoKi::Functor<TYPE,TYPE2>& func () const { return m_fun.func() ; }
    int                              val  () const { return m_val        ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    EqualToInt();
    // ========================================================================
  private:
    // ========================================================================
    /// the functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun ;                 // the functor 
    /// the value 
    int  m_val ;                                                   // the value 
    // ========================================================================
  };
  // ==========================================================================
  template <class TYPE2>
  class NotEqualToInt<void,TYPE2> : public LoKi::Functor<void,bool>
  {
  private:
    // ========================================================================
    typedef  void TYPE ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    NotEqualToInt
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , 
      const int                         val ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun , val )  
    {}
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    NotEqualToInt
    ( const int val , const LoKi::Functor<TYPE,TYPE2>&  fun ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun , val ) 
    {}
    // ========================================================================
    /// copy constructor 
    NotEqualToInt
    ( const NotEqualToInt& right )
      : LoKi::AuxFunBase          ( right ) 
      , LoKi::Functor<TYPE,bool>  ( right )
      , m_fun ( right.m_fun ) 
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~NotEqualToInt(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  NotEqualToInt* clone() const { return new NotEqualToInt(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( /* argument a */ ) const
    { return !m_fun.equal_to ( /* a */ ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << "(" << m_fun.val() << "!=" << m_fun.func() << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    NotEqualToInt();
    // ========================================================================
  private:
    // ========================================================================
    // the functor 
    LoKi::EqualToInt<TYPE,TYPE2> m_fun ; ///< the functor 
    // ========================================================================
  };
  // ==========================================================================
  template <class TYPE2>
  class EqualToUInt<void,TYPE2> : public LoKi::Functor<void,bool>
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    EqualToUInt
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , 
      const unsigned int                val ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun ) 
      , m_val ( val ) 
    {}
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    EqualToUInt
    ( const unsigned int val , const LoKi::Functor<TYPE,TYPE2>&  fun ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun ) 
      , m_val ( val ) 
    {}
    // ========================================================================
    /// copy constructor 
    EqualToUInt
    ( const EqualToUInt& right )
      : LoKi::AuxFunBase          ( right ) 
      , LoKi::Functor<TYPE,bool>  ( right )
      , m_fun ( right.m_fun ) 
      , m_val ( right.m_val )
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~EqualToUInt(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  EqualToUInt* clone() const { return new EqualToUInt(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( /* argument a */ ) const
    { return equal_to ( /* a */ ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << "(" << m_val << "==" << m_fun << ")" ; }
    // ========================================================================
  public:
    // ========================================================================
    inline result_type equal_to ( /* argument a */ ) const
    {
      // evaluate the function 
      const typename LoKi::Functor<TYPE,TYPE2>::result_type r = 
        m_fun.fun ( /* a */ ) ;
      return LHCb::Math::equal_to_uint ( r , m_val ) ;
    }    
    // ========================================================================
  public:
    // ========================================================================
    const LoKi::Functor<TYPE,TYPE2>& func () const { return m_fun.func() ; }
    unsigned int                     val  () const { return m_val        ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    EqualToUInt();
    // ========================================================================
  private:
    // ========================================================================
    // the functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun ; ///< the functor 
    // the value 
    unsigned int  m_val ; ///< the value 
    // ========================================================================
  };
  // ==========================================================================
  template <class TYPE2>
  class NotEqualToUInt<void,TYPE2> : public LoKi::Functor<void,bool>
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    NotEqualToUInt
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , 
      const unsigned int                val ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun , val ) 
    {}
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    NotEqualToUInt
    ( const unsigned int val , const LoKi::Functor<TYPE,TYPE2>&  fun ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun , val ) 
    {}
    // ========================================================================
    /// copy constructor 
    NotEqualToUInt
    ( const NotEqualToUInt& right )
      : LoKi::AuxFunBase          ( right ) 
      , LoKi::Functor<TYPE,bool>  ( right )
      , m_fun ( right.m_fun )  
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~NotEqualToUInt(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  NotEqualToUInt* clone() const { return new NotEqualToUInt(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( /* argument a */ ) const
    { return !m_fun.equal_to ( /* a */ ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << "(" << m_fun.val() << "!=" << m_fun.func() << ")" ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    NotEqualToUInt();
    // ========================================================================
  private:
    // ========================================================================
    /// the functor 
    LoKi::EqualToUInt<TYPE,TYPE2> m_fun ;                        // the functor 
  };
  // ==========================================================================
  template <class TYPE1, class TYPE2, class TYPE3>
  class Compose<void,TYPE1,TYPE2,TYPE3> : public LoKi::Functor<void,TYPE2>
  {
  private:
    // ========================================================================
    /// result type 
    typedef typename LoKi::Functor<void,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
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
    virtual  result_type operator() ( /* argument */ ) const 
    { 
      const LoKi::Apply<TYPE3,TYPE2> f2 ( &m_fun2.func() ) ;
      return f2.eval ( m_fun1.fun() ) ;
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(" << m_fun1 << ">>" << m_fun2  << ")" ; }
    // ========================================================================
  public:
    // ========================================================================
    /// the first functor 
    LoKi::FunctorFromFunctor<void,TYPE1>  m_fun1 ;         // the first functor 
    /// the second functor 
    LoKi::FunctorFromFunctor<TYPE3,TYPE2> m_fun2 ;        // the second functor 
    // ========================================================================
  } ;
  // ==========================================================================
  template<> 
  class InRange<void,double>: public LoKi::Functor<void,bool> 
  {
  public:
    // ========================================================================
    /// type for limit 
    typedef double                                                      TYPE2 ;
    /// type for limit 
    typedef boost::call_traits<TYPE2>::param_type                       Type2 ;
    // ========================================================================
  private:
    // ========================================================================
    /// result type 
    typedef LoKi::Functor<void,bool>::result_type                 result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the functor and edges 
     *  @param fun the functor 
     *  @param low the low edge  
     *  @param high the high edge 
     */
    InRange
    ( const LoKi::Functor<void,TYPE2>& fun  , 
      Type2                            low  , 
      Type2                            high )
      : LoKi::Functor<void,bool> () 
      , m_fun  ( fun  ) 
      , m_low  ( low  ) 
      , m_high ( high ) 
    {}
    /// MANDATORY: virtual destructor 
    virtual ~InRange() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  InRange* clone() const { return new InRange ( *this ) ; }
    /// MANDATORY: the only one essential method 
    virtual result_type operator() () const 
    {
      LoKi::Functor<void,TYPE2>::result_type r = m_fun.fun() ;
      return m_low <= r && r <= m_high ;
    }
    /// OPTIONAL: the nice printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    {
      return s << " inRange( " << m_fun 
               << " , "        << m_low 
               << " , "        << m_high << " )" ;
      
    }
    // ========================================================================
  private:
    // ========================================================================
    /// the default contructor is disabled 
    InRange() ;                           // the default contructor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the functor itself 
    LoKi::FunctorFromFunctor<void,TYPE2> m_fun  ;         // the functor itself
    /// the low edge 
    TYPE2                                m_low  ;         //       the low edge 
    /// the high edge 
    TYPE2                                m_high ;         //       the low edge 
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VOIDPRIMITIVES_H
// ============================================================================
