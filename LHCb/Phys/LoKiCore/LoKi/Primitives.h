// $Id: Primitives.h,v 1.1 2007-06-10 19:54:06 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PRIMITIVES_H 
#define LOKI_PRIMITIVES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/BiFunctions.h"
#include "LoKi/Reference.h"
#include "LoKi/Field.h"
#include "LoKi/valid.h"
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
  /** @struct Constant 
   *  The helper concrete implementation of the simplest 
   *  function ("constant")
   *  @see LoKi::Function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE>
  struct Constant : public Function<TYPE>
  {
    /// define all needed types 
    _LOKI_FUNCTION_TYPES_( Constant , TYPE ) ;
    /// constructor 
    Constant ( const double value = 0 )
      : FunB() ,                       m_value ( value         ) {} ;
    /// copy
    Constant ( const Self& right      )
      : AuxFunBase ( right ) 
      , FunB       ( right ) 
      , m_value    ( right.m_value ) {} ;
    /// assignement  
    Self& operator=( const Self& right ) 
    { m_value = right.m_value ; return *this ; }
    /// clone method (mandatory) 
    virtual Self* clone   ()              const { return new Self( *this ) ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument ) const { return m_value ; }
    /// the basic printout method  
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return  s << "[" << m_value << "]" ; };
  private:
    double m_value ;
  };  
  // ==========================================================================
  /** @struct BooleanConstant 
   *  The helper concrete simplest predicate ("constant")
   *  @see LoKi::Predicate
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE>
  struct BooleanConstant : public Predicate<TYPE>
  {
    /// define all needed types 
    _LOKI_PREDICATE_TYPES_( BooleanConstant , TYPE ) ;
    /// constructor 
    BooleanConstant( const bool value  = true ) 
      : FunB() , m_value( value         ) {};
    /// copy 
    BooleanConstant( const Self& right        ) 
      : AuxFunBase ( right ) 
      , FunB       ( right ) 
      , m_value    ( right.m_value ) {};
    /// assignement  
    Self& operator=( const Self& right ) 
    { m_value = right.m_value ; return *this ; }
    /// clone method 
    virtual Self*  clone   ()              const { return new Self( *this ) ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument ) const { return m_value ; }
    /// the basic printout method  
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return  s << "(" << ( m_value ? "True" : "False" ) << ")" ; };
  private:
    bool m_value ;
  };  
  // ==========================================================================
  /** @struct PredicateFromPredicate
   *  The helper structure to implement 
   *  predicates from predicates
   *
   *  It is useful for construction of complex predicates 
   *  from the simpler components:
   *
   *  @code 
   *
   *  typedef Predicate<SomeType>              Pred ;
   *  typedef PredicateFromPredicate<SomeType> PfP  ;
   *  
   *  void func( const Pred& A , const Pred& B ) 
   *  {
   *    // construct different predicates from othe rpredicates  
   *    PfP p1 = A && B   ;
   *    PfP p2 = A || B   ;
   *    PfP p3 = !A       ;
   *    PfP p4 = ( A || B ) || !( A && B ) ;
   *  }
   *
   *  @endcode 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE>
  class PredicateFromPredicate : public Predicate<TYPE> 
  {
  public :
    /// define all needed types 
    _LOKI_PREDICATE_TYPES_( PredicateFromPredicate , TYPE ) ;
    /// constructor 
    PredicateFromPredicate ( const FunB& pr ) 
      : FunB () , m_pr( pr.        clone () ) {} ;
    /// deep copy  
    PredicateFromPredicate ( const Self& pr ) 
      : AuxFunBase ( pr ) 
      , FunB       ( pr ) 
      , m_pr       ( 0  )
    {
      m_pr = typeid ( Self ) == typeid ( pr ) ? 
        pr.m_pr -> clone () : pr.clone() ;
    };
    /// destructor 
    virtual ~PredicateFromPredicate()             { delete  m_pr             ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument p ) const { return (*m_pr)( p ); }
    /// clone method 
    virtual Self* clone   ()                const { return new Self( *this ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return  m_pr->fillStream( s ) ; };
    /// unique function ID (hash) 
    virtual std::size_t   id () const { return m_pr->id() ; }
 public:
    /// the assignement operator is enabled now 
    Self& operator= ( const Self& right )
    {
      if ( this == &right ) { return *this ; } 
      /// set new pointer
      FunB* newf = typeid ( Self ) == typeid ( right ) ? 
        right.m_pr -> clone () : (FunB*) right.clone() ;
      // delete own pointer 
      delete m_pr  ; 
      m_pr = newf  ;
      return *this ;
    };
    /// the assignement operator is enabled now 
    Self& operator= ( const FunB& right )
    {
      if ( this == &right ) { return *this ; } 
      /// set new pointer 
      FunB* newf = right.clone() ;
      /// delete own pointer 
      delete m_pr   ; 
      m_pr = newf   ;
      return *this ;
    };  
  protected:
    // accessor to the underlying predicate 
    inline const FunB& pr () const     { return *m_pr ; }
    // evaluate  the underlying predicate
    inline const result_type pr ( argument p ) const { return (*m_pr) ( p )  ; }
  private:
    /// default constructor is private 
    PredicateFromPredicate();
  private:
    const FunB* m_pr ;
  };
  // ==========================================================================  
  /** @struct PredicateFromTwoPredicates
   *  The helper structure to implement predicates 
   *  from 2 predicates
   *
   *  It is used by LoKi for implementation of logical operations 
   *  between 2 predicates
   *  @see LoKi::And 
   *  @see LoKi::Or 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE>
  struct PredicateFromTwoPredicates : public Predicate<TYPE> 
  {
  protected:
    /// define all needed types 
    _LOKI_PREDICATE_TYPES_( PredicateFromTwoPredicates , TYPE ) ;
    /// constructor 
    PredicateFromTwoPredicates ( const FunB& pr1     ,  
                                 const FunB& pr2     )
      : FunB ()
        , m_pr1 ( pr1.           clone ()  ) 
        , m_pr2 ( pr2.           clone ()  ) {};
    /// deep copy  
    PredicateFromTwoPredicates ( const Self& right   )
      : AuxFunBase ( right )
      , FunB       ( right )
      , m_pr1 ( right.m_pr1 -> clone ()  )
      , m_pr2 ( right.m_pr2 -> clone ()  ) {};
    /// destructor 
    virtual ~PredicateFromTwoPredicates() { delete  m_pr1 ; delete m_pr2 ;}
  protected:
    /// accessor to the first  predicate 
    inline const FunB& pr1() const        { return *m_pr1 ; }
    /// evaluate    the first predicate 
    inline       result_type  pr1( argument p ) const { return (*m_pr1)( p ) ; }
    /// accessor to the second predicate 
    inline const FunB& pr2() const        { return *m_pr2 ; }    
    /// evaluate    the second predicate 
    inline       result_type  pr2( argument p ) const { return (*m_pr2)( p ) ; }
  private:
    /// default constructor is private 
    PredicateFromTwoPredicates();
    /// assignement is private
    Self& operator=( const Self& );
  private:
    const FunB* m_pr1 ;
    const FunB* m_pr2 ;    
  };
  // ==========================================================================  
  /** @struct PredicateFromTwoFunctions
   *  The helper structure to implement predicates 
   *  from 2 functions
   *
   *  It is used by LoKi for implementation of comparison 
   *  operations between 2 functions
   *  @see LoKi::Less 
   *  @see LoKi::Equal
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE>
  struct PredicateFromTwoFunctions : public Predicate<TYPE> 
  {
  protected:
    /// define all nesessary types 
    _LOKI_PREDICATE_TYPES_( PredicateFromTwoFunctions , TYPE ) ;    
    typedef Function<TYPE>                  function ;
    /// constructor 
    PredicateFromTwoFunctions ( const function& fun1     ,  
                                const function& fun2     )
      : FunB () 
      , m_fun1 ( fun1.           clone ()  ) 
      , m_fun2 ( fun2.           clone ()  ) {};
    /// deep copy  
    PredicateFromTwoFunctions ( const Self& right   )
      : AuxFunBase ( right )
      , FunB       ( right ) 
      , m_fun1 ( right.m_fun1 -> clone ()  )
      , m_fun2 ( right.m_fun2 -> clone ()  ) {};
    /// destructor 
    virtual ~PredicateFromTwoFunctions() { delete m_fun1 ; delete m_fun2 ;}
  protected:
    /// accessor to the first  function 
    inline const function& fun1() const { return *m_fun1 ; }
    /// evaluate    the first function 
    inline       double    fun1( argument p ) const { return (*m_fun1)( p ) ; }
    /// accessor to the second function 
    inline const function& fun2() const { return *m_fun2 ; }    
    /// evaluate    the second function 
    inline       double    fun2( argument p ) const { return (*m_fun2)( p ) ; }
  private:
    /// default constructor is private 
    PredicateFromTwoFunctions();
    /// assignement is private
    Self& operator=( const Self& );
  private:
    const function* m_fun1 ;
    const function* m_fun2 ;    
  };
  // ==========================================================================  
  /** @class FunctionFromTwoFunctions
   *  The helper structure to implement function 
   *  from from 2 functions
   *
   *  It is used by LoKi for implementation of basic ariphmetic  
   *  operations between 2 functions
   *  @see LoKi::Plus
   *  @see LoKi::Minus
   *  @see LoKi::Divide
   *  @see LoKi::Multiply
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE>
  class FunctionFromTwoFunctions : public Function<TYPE> 
  {
  protected:
    /// define all nesessary types 
    _LOKI_FUNCTION_TYPES_( FunctionFromTwoFunctions , TYPE ) ;    
    typedef FunB                  function ;
    /// constructor 
    FunctionFromTwoFunctions ( const function& fun1     ,  
                               const function& fun2     )
      : FunB () 
      , m_fun1 ( fun1.           clone ()  ) 
      , m_fun2 ( fun2.           clone ()  ) {};
    /// deep copy  
    FunctionFromTwoFunctions ( const Self& right   )
      : AuxFunBase ( right )
      , FunB       ( right )
      , m_fun1 ( right.m_fun1 -> clone ()  )
      , m_fun2 ( right.m_fun2 -> clone ()  ) {};
    /// destructor 
    virtual ~FunctionFromTwoFunctions () { delete m_fun1 ; delete m_fun2 ;}
  protected:
    /// accessor to the first  function 
    inline const function& fun1() const { return *m_fun1 ; }
    /// evaluate    the first function 
    inline       result_type fun1 ( argument p ) const 
    { return (*m_fun1)( p ) ; }
    /// accessor to the second function 
    inline const function& fun2() const { return *m_fun2 ; }
    /// evaluate    the second function 
    inline       result_type fun2 ( argument p ) const 
    { return (*m_fun2)( p ) ; }
  private:
    /// default constructor is private 
    FunctionFromTwoFunctions();
    /// assignement is private
    Self& operator=( const Self& );
  private:
    const function* m_fun1 ;
    const function* m_fun2 ;    
  };  
  // ==========================================================================
  /** @struct FunctionFromFunction
   *  helper structure to implement function from function
   *
   *  It is used by LoKi for implementation of compose 
   *  functions, e.g. <tt>sin(Fun)</tt>, where <tt>Fun</tt> 
   *  is a <tt>LoKi::Function</tt>
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE>
  class FunctionFromFunction : public Function<TYPE> 
  {
  public:
    /// define all nesessary types 
    _LOKI_FUNCTION_TYPES_( FunctionFromFunction , TYPE ) ;    
    typedef FunB                  function ;
    /// constructor 
    FunctionFromFunction ( const function& fun      )
      : FunB  () , m_fun ( fun.           clone ()  ) {};
    /// deep 'copy'  
    FunctionFromFunction ( const Self& right        )
      : AuxFunBase ( right )
      , FunB       ( right ) 
      , m_fun ( 0  ) 
    {
      m_fun = typeid ( Self ) == typeid ( right ) ? 
        right.m_fun -> clone () : right.clone() ;
    };
    /// destructor 
    virtual ~FunctionFromFunction () { delete m_fun ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument p ) const { return fun( p ) ; }
    /// clone method 
    virtual Self* clone    () const { return new Self( *this ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return  m_fun->fillStream( s ) ; };
    /// unique function ID
    virtual std::size_t   id () const { return m_fun->id() ; }
  public:
    /// the assignement operator is enabled now 
    Self& operator= ( const Self& right )
    {
      if ( this == &right ) { return *this ; } 
      // set new pointer 
      FunB* newf = typeid ( Self ) == typeid ( right ) ? 
        right.m_fun -> clone () : (FunB*) right.clone() ;
      // delete own pointer 
      delete m_fun ; 
      m_fun = newf ;
      return *this ;
    };
    /// the assignement operator is enabled now 
    Self& operator= ( const FunB& right )
    {
      if ( this == &right ) { return *this ; } 
      /// set new pointer 
      FunB* newf = right.clone() ;
      /// delete own pointer 
      delete m_fun ;
      m_fun = newf ;
      return *this ;
    };  
  public:
    /// evaluate the function
    inline       result_type fun ( argument p ) const 
    { return (*m_fun) ( p )  ; }
    /// accessor to the function 
    inline const function&   fun ()             const { return *m_fun ; }
  private:
    /// default constructor is private 
    FunctionFromFunction();
  private:
    const function* m_fun        ;
  };
  // ==========================================================================  
  /** @struct PredicateFromFunctionAndValue
   *  The helper structure to implement predicates 
   *  from the function and 2 values 
   *
   *  It is used by LoKi for implementation of comparison 
   *  operations between 2 functions
   *  @see LoKi::Less 
   *  @see LoKi::Equal
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE>
  struct PredicateFromFunctionAndValue 
    : public Predicate<TYPE> 
  {
  public:
    /// define all nesessary types 
    typedef           LoKi::Predicate<TYPE>   _Predicate ;
    typedef           LoKi::Function<TYPE>     _Function ;
    typedef typename _Predicate::result_type result_type ;
    typedef typename _Predicate::argument       argument ;
    typedef typename _Function::result_type   value_type ;
  public:
    /// constructor 
    PredicateFromFunctionAndValue
    ( const _Function&  fun ,  
      const value_type& val )
      : _Predicate() 
      , m_fun   ( fun ) 
      , m_value ( val ) 
    {};
    /// deep copy  
    PredicateFromFunctionAndValue
    ( const PredicateFromFunctionAndValue& right   )
      : AuxFunBase ( right )
      , _Predicate ( right ) 
      , m_fun   ( right.m_fun   )
      , m_value ( right.m_value )
    {};
    /// MANDATORY: virtual destructor 
    virtual ~PredicateFromFunctionAndValue() {}
  protected:
    /// accessor to the function 
    const _Function& fun   ()             const { return m_fun.fun (   ) ; }
    /// evaluate the value of the function 
    value_type       fun   ( argument a ) const { return m_fun.fun ( a ) ; }
    /// get the value 
    value_type       value ()             const { return m_value         ; }
  private:
    /// default constructor is disabled 
    PredicateFromFunctionAndValue();
    /// assignement is disabled
    PredicateFromFunctionAndValue& operator=
    ( const PredicateFromFunctionAndValue& );
  private:
    LoKi::FunctionFromFunction<TYPE> m_fun   ;
    value_type                       m_value ;
  } ;
  // ==========================================================================
  /** @struct And 
   *  helper function to implement logical AND of 2 predicates 
   *  
   *  It is used by LoKi for implementation of logical 
   *  binary <tt>&&</tt> operator for predicates
   *  @code 
   *  typedef Predicate<SomeType>     Pred ;
   *  typedef PredicateFromPredicate  PfP  ;
   *
   *  void func( const Pred& A , const Pred& B )
   *  {
   *    PfP p1 = A && B             ; // operator form
   *    PfP p2 = LoKi::And( A , B ) ; // explicit form
   *  }
   *  @endcode 
   *  @see LoKi/Operators.h
   *  @see LoKi::Predicate
   *  @see LoKi::PredicateFromPredicate
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE> 
  struct And : public PredicateFromTwoPredicates<TYPE> 
  {
    /// define all nesessary types 
    _LOKI_PREDICATE_TYPES_( And , TYPE ) ;    
    /// constructor 
    And ( const FunB& pr1 , const FunB& pr2 ) 
      : PredicateFromTwoPredicates<TYPE>( pr1 , pr2 ) {} ;
    /// clone method (mandatory!)
    virtual Self* clone() const { return new Self( *this ); }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument object ) const 
    { return this->pr1( object ) ? this->pr2( object ) : false ;}  
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(("  << this->pr1() << ")&(" << this->pr2() << "))" ; };
  };  
  // ==========================================================================
  /** @struct Or 
   *  helper function to implement logical OR of 2 predicates 
   *  
   *  It is used by LoKi for implementation of logical 
   *  binary <tt>||</tt> operator for predicates
   *  @code 
   *  typedef Predicate<SomeType>     Pred ;
   *  typedef PredicateFromPredicate  PfP  ;
   *
   *  void func( const Pred& A , const Pred& B )
   *  {
   *    PfP p1 = A || B            ;  // operator form
   *    PfP p2 = LoKi::Or( A , B ) ; // explicit form
   *  }
   *  @endcode 
   *  @see LoKi/Operators.h
   *  @see LoKi::Predicate
   *  @see LoKi::PredicateFromPredicate
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  // ==========================================================================
  template<class TYPE> 
  struct Or : public PredicateFromTwoPredicates<TYPE> 
  {
    /// define all nesessary types 
    _LOKI_PREDICATE_TYPES_( Or , TYPE ) ;    
    /// constructor 
    Or( const FunB& pr1 , const FunB& pr2 ) 
      : PredicateFromTwoPredicates<TYPE>( pr1 , pr2 ) {} ;
    /// clone method (mandatory)
    virtual Self* clone() const { return new Self( *this ); }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument object ) const 
    { return this->pr1( object )  ? true : this->pr2( object ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(("  << this->pr1() << ")|(" << this->pr2() << "))" ; };
  };  
  // ==========================================================================
  /** @struct Not
   *
   *  The helper function to implement logical negation 
   *  of the predicate
   *
   *  @code 
   *
   *  typedef Predicate<SomeType>     Pred ;
   *  typedef PredicateFromPredicate  PfP  ;
   *
   *  void func( const Pred& A )
   *  {
   *    PfP p2 = LoKi::Not( A ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi::Predicate
   *  @see LoKi::PredicateFromPredicate
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE> 
  class Not : public Predicate<TYPE>
  {
  public:
    /// define all nesessary types 
    _LOKI_PREDICATE_TYPES_( Not , TYPE ) ;    
    /// constructor 
    Not ( const FunB& pr    ) : m_pr( pr ) {} ;
    /// clone method (mandatory)
    virtual Self* clone() const { return new Self( *this ); }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument object ) const 
    { return ! m_pr( object ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(!"  << m_pr << ")" ; };
  protected: 
    PredicateFromPredicate<TYPE> m_pr ;
  };  
  // ==========================================================================
  /** @struct Less 
   *  The helper function to implement Less of 2 functions 
   *
   *  It is used by LoKi for implementation of comparison 
   *  operator for functions:
   *  @code 
   *  typedef Function<SomeType>      Func ;
   *  typedef PredicateFromPredicate  PfP  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    PfP p1 = A < B               ;  // operator form
   *    PfP p2 = LoKi::Less( A , B ) ; // explicit form
   *  }
   *  @endcode 
   *  @see LoKi/Operators.h
   *  @see LoKi::Function
   *  @see LoKi::PredicateFromPredicate
   *  @see LoKi::PredicateFromTwoFunctions
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE> 
  struct Less : public PredicateFromTwoFunctions<TYPE> 
  {
    typedef PredicateFromTwoFunctions<TYPE> _B ;
    /// define all nesessary types 
    _LOKI_PREDICATE_TYPES_( Less , TYPE ) ;    
    typedef Function<TYPE> func ;
    /// constructor 
    Less( const func& fun1 , const func& fun2 )
      : PredicateFromTwoFunctions<TYPE> ( fun1 , fun2 ) {}
    /// clone method (mandatory)
    virtual Self* clone() const { return new Self( *this ); }     
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument object ) const 
    { return _B::fun1( object ) < _B::fun2( object ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "("  << _B::fun1() << "<" << _B::fun2() << ")" ; };
  };  
  // ==========================================================================
  /** @struct Equal 
   *  The helper function to implement Equal of 2 functions 
   *
   *  It is used by LoKi for implementation of comparison 
   *  operator for functions:
   *  @code 
   *  typedef Function<SomeType>      Func ;
   *  typedef PredicateFromPredicate  PfP  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    PfP p1 = A == B               ; // operator form
   *    PfP p2 = LoKi::Equal( A , B ) ; // explicit form
   *  }
   *  @endcode 
   *  @see LoKi/Operators.h
   *  @see LoKi::Function
   *  @see LoKi::PredicateFromPredicate
   *  @see LoKi::PredicateFromTwoFunctions
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE> 
  struct Equal : public PredicateFromTwoFunctions<TYPE> 
  {
    /// define all nesessary types 
    _LOKI_PREDICATE_TYPES_( Equal , TYPE ) ;    
    typedef Function<TYPE> func ;
    /// constructor 
    Equal( const func& fun1 , 
           const func& fun2 )
      : PredicateFromTwoFunctions<TYPE>( fun1 , fun2 ) {};
    /// clone method (mandatory!)
    virtual Self* clone() const { return new Self( *this ); }    
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument object ) const 
    { return this->fun1( object ) == this->fun2( object ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "("  << this->fun1() << "==" << this->fun2() << ")" ; };
  };
  // ==========================================================================
  /** @struct LessOrEqual 
   *  The helper function to implement Less of 2 functions 
   *
   *  It is used by LoKi for implementation of comparison 
   *  operator for functions:
   *  @code 
   *  typedef Function<SomeType>      Func ;
   *  typedef PredicateFromPredicate  PfP  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    PfP p1 = A <= B                ;  // operator form
   *    PfP p2 = LoKi::LessOrEqual( A , B ) ; // explicit form
   *  }
   *  @endcode 
   *  @see LoKi/Operators.h
   *  @see LoKi::Function
   *  @see LoKi::PredicateFromPredicate
   *  @see LoKi::PredicateFromTwoFunctions
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE> 
  struct LessOrEqual : public PredicateFromTwoFunctions<TYPE> 
  {
    /// define all nesessary types 
    _LOKI_PREDICATE_TYPES_( LessOrEqual , TYPE ) ;    
    typedef Function<TYPE> func ;
    /// constructor 
    LessOrEqual( const func& fun1 , const func& fun2 )
      : PredicateFromTwoFunctions<TYPE> ( fun1 , fun2 ) {}
    /// clone method (mandatory)
    virtual Self* clone() const { return new Self( *this ); }     
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument object ) const 
    { return this->fun1( object ) <= this->fun2( object ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "("  << this->fun1() << "<=" << this->fun2() << ")" ; };
  };  
  // ==========================================================================
  /** @class NotEqual 
   *  The helper function to implement NotEqual of 2 functions 
   *
   *  It is used by LoKi for implementation of comparison 
   *  operator for functions:
   *  @code 
   *  typedef Function<SomeType>      Func ;
   *  typedef PredicateFromPredicate  PfP  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    PfP p1 = A != B                  ; // operator form
   *    PfP p2 = LoKi::NotEqual( A , B ) ; // explicit form
   *  }
   *  @endcode 
   *  @see LoKi/Operators.h
   *  @see LoKi::Function
   *  @see LoKi::PredicateFromPredicate
   *  @see LoKi::PredicateFromTwoFunctions
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE> 
  class NotEqual : public PredicateFromTwoFunctions<TYPE> 
  {
  public:
    /// define all nesessary types 
    _LOKI_PREDICATE_TYPES_( NotEqual , TYPE ) ;    
    typedef Function<TYPE> func ;
    /// constructor 
    NotEqual ( const func& fun1 , 
               const func& fun2 )
      : PredicateFromTwoFunctions<TYPE>( fun1 , fun2 ) {};
    virtual ~NotEqual(){}
    /// clone method (mandatory!)
    virtual Self* clone() const { return new Self( *this ); }    
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument object ) const 
    { return this->fun1( object ) != this->fun2( object ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "("  << this->fun1() << "!=" << this->fun2() << ")" ; };
  private:
    NotEqual();
  };  
  // ==========================================================================
  /** @struct Plus 
   *  The helper function to implement addition of 2 function 
   *
   *  It is used by LoKi for implementation of addition of  
   *  2 functions:
   *  @code 
   *  typedef Function<SomeType>    Func ;
   *  typedef FunctionFromFunction  FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = A  + B              ; // operator form
   *    FfF f2 = LoKi::Plus( A , B ) ; // explicit form
   *  }
   *  @endcode 
   *  @see LoKi/Operators.h
   *  @see LoKi::Function
   *  @see LoKi::FunctionFromFunction
   *  @see LoKi::PredicateFromTwoFunctions
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE> 
  struct Plus : public FunctionFromTwoFunctions<TYPE> 
  {
    /// define all nesessary types 
    _LOKI_FUNCTION_TYPES_( Plus , TYPE ) ;    
    /// constructor 
    Plus( const FunB& fun1 , const FunB& fun2 )
      : FunctionFromTwoFunctions<TYPE> ( fun1 , fun2 ) {};
    /// clone method (mandatory!)
    virtual Self* clone() const { return new Self( *this ); }    
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument object ) const 
    { return this->fun1( object ) + this->fun2( object ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "("  << this->fun1() << "+" << this->fun2() << ")" ; };
  };  
  // ==========================================================================
  /** @struct Minus 
   *  The helper function to implement subtraction of 2 function 
   *
   *  It is used by LoKi for implementation of subtraction of  
   *  2 functions:
   *  @code 
   *  typedef Function<SomeType>    Func ;
   *  typedef FunctionFromFunction  FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = A - B                ; // operator form
   *    FfF f2 = LoKi::Minus( A , B ) ; // explicit form
   *  }
   *  @endcode 
   *  @see LoKi/Operators.h
   *  @see LoKi::Function
   *  @see LoKi::FunctionFromFunction
   *  @see LoKi::PredicateFromTwoFunctions
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE> 
  struct Minus : public FunctionFromTwoFunctions<TYPE> 
  {
    /// define all nesessary types 
    _LOKI_FUNCTION_TYPES_( Minus , TYPE ) ;    
    /// constructor 
    Minus( const FunB& fun1 , 
           const FunB& fun2 ) 
      : FunctionFromTwoFunctions<TYPE> ( fun1 , fun2 ) {};
    /// clone method (mandatory)
    virtual Self* clone() const { return new Self( *this ); }    
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument object ) const 
    { return this->fun1( object ) - this->fun2( object ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "("  << this->fun1() << "-" << this->fun2() << ")" ; };
  };  
  // ==========================================================================
  /** @struct Divide
   *  The helper function to implement division of 2 function 
   *
   *  It is used by LoKi for implementation of division of  
   *  2 functions:
   *  @code 
   *  typedef Function<SomeType>    Func ;
   *  typedef FunctionFromFunction  FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = A / B                 ; // operator form
   *    FfF f2 = LoKi::Divide( A , B ) ; // explicit form
   *  }
   *  @endcode 
   *  @see LoKi/Operators.h
   *  @see LoKi::Function
   *  @see LoKi::FunctionFromFunction
   *  @see LoKi::PredicateFromTwoFunctions
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE> 
  struct Divide : public FunctionFromTwoFunctions<TYPE> 
  {
    /// define all nesessary types 
    _LOKI_FUNCTION_TYPES_( Divide , TYPE ) ;    
    /// constructor 
    Divide( const FunB& fun1 , 
            const FunB& fun2 ) 
      : FunctionFromTwoFunctions<TYPE> ( fun1 , fun2 ) {};
    /// clone method (mandatory)
    virtual Self* clone() const { return new Self( *this ); }    
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument object ) const 
    { return this->fun1( object ) / this->fun2( object ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "("  << this->fun1() << "/" << this->fun2() << ")" ; };
  };  
  // ==========================================================================
  /** @struct Multiply
   *  The helper function to implement multiplication of 2 functions 
   *
   *  It is used by LoKi for implementation of multiplication of  
   *  2 functions:
   *  @code 
   *  typedef Function<SomeType>    Func ;
   *  typedef FunctionFromFunction  FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = A * B                   ; // operator form
   *    FfF f2 = LoKi::Multiply( A , B ) ; // explicit form
   *  }
   *  @endcode 
   *  @see LoKi/Operators.h
   *  @see LoKi::Function
   *  @see LoKi::FunctionFromFunction
   *  @see LoKi::PredicateFromTwoFunctions
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template<class TYPE> 
  struct Multiply : public FunctionFromTwoFunctions<TYPE> 
  {
    /// define all nesessary types 
    _LOKI_FUNCTION_TYPES_( Multiply , TYPE ) ;    
    /// constructor 
    Multiply( const FunB& fun1 , 
              const FunB& fun2 ) 
      : FunctionFromTwoFunctions<TYPE> ( fun1 , fun2 ) {};
    /// clone method (mandatory)
    virtual Self* clone() const { return new Self( *this ); }    
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument object ) const 
    { return this->fun1( object ) * this->fun2( object ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "("  << this->fun1() << "*" << this->fun2() << ")" ; };
  };
  // ==========================================================================
  /** @class Min 
   *  Return  the minunum from functions 
   */
  template<class TYPE> 
  class Min : public FunctionFromTwoFunctions<TYPE>
  {
  public:
    /// define all nesessary types 
    _LOKI_FUNCTION_TYPES_( Min , TYPE ) ;    
    
    /** constructor from 2 functions 
     *  @param fun1 the first  function
     *  @param fun2 the second function
     */
    Min( const FunB& fun1 , 
         const FunB& fun2 ) 
      : FunctionFromTwoFunctions<TYPE>( fun1 , fun2 ) {}
    /** constructor from 3 functions 
     *  @param fun1 the first functio
     *  @param fun2 the first functio
     *  @param fun3 the first functio
     */
    Min( const FunB& fun1 , 
         const FunB& fun2 , 
         const FunB& fun3 ) 
      : FunctionFromTwoFunctions<TYPE>( Self( fun1 , fun2 ) , fun3 ) {}    
    /** constructor from 4 functions 
     *  @param fun1 the first functio
     *  @param fun2 the first functio
     *  @param fun3 the first functio
     */
    Min( const FunB& fun1 , 
         const FunB& fun2 , 
         const FunB& fun3 , 
         const FunB& fun4 ) 
      : FunctionFromTwoFunctions<TYPE>( Self( fun1 , fun2 , fun3 ) , fun4 ) {}
    /// virtual destructor 
    virtual ~Min() {}
    /// clone method (mandatory)
    virtual Self* clone() const { return new Min(*this) ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument object ) const 
    { 
      const result_type val1 = this->fun1 ( object ) ;
      const result_type val2 = this->fun2 ( object ) ;      
      return val1 < val2 ? val1 : val2 ;
    }    
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "min("  << this->fun1() << "," << this->fun2() << ")" ; };
  private:
    Min () ;
  };
  // ==========================================================================
  /** @class Max
   *  Return  the maximum from functions 
   */
  template<class TYPE> 
  class Max : public FunctionFromTwoFunctions<TYPE>
  {
  public:
    /// define all nesessary types 
    _LOKI_FUNCTION_TYPES_( Max , TYPE ) ;    

    /** constructor from 2 functions 
     *  @param fun1 the first  function
     *  @param fun2 the second function
     */
    Max ( const FunB& fun1 , 
          const FunB& fun2 ) 
      : FunctionFromTwoFunctions<TYPE>( fun1 , fun2 ) {}
    /** constructor from 3 functions 
     *  @param fun1 the first  function
     *  @param fun2 the second function
     *  @param fun3 the third  function
     */
    Max ( const FunB& fun1 , 
          const FunB& fun2 ,
          const FunB& fun3 ) 
      : FunctionFromTwoFunctions<TYPE>( Self( fun1 , fun2 ) , fun3 ) {}
    /** constructor from 4 functions 
     *  @param fun1 the first  function
     *  @param fun2 the second function
     *  @param fun3 the third  function
     *  @param fun4 the fourth  function
     */
    Max ( const FunB& fun1 , 
          const FunB& fun2 ,
          const FunB& fun3 ,
          const FunB& fun4 ) 
      : FunctionFromTwoFunctions<TYPE>( Self( fun1 , fun2 , fun3 ) , fun4 ) {}
    /// virtual destructor 
    virtual ~Max() {}
    /// clone method (mandatory)
    virtual Self* clone() const { return new Max(*this) ; }
    /// the only one essential method ("function")      
    virtual double operator() ( argument object ) const 
    { 
      const double val1 = this->fun1 ( object ) ;
      const double val2 = this->fun2 ( object ) ;      
      return val1 < val2 ? val2 : val1 ;
    }    
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "max("  << this->fun1() << "," << this->fun2() << ")" ; };
  private:
    Max () ;
  };
  // ==========================================================================
  /** @struct ComposeFunction
   *  The helper structure to implement function of function
   *
   *  @code 
   *
   *  typedef Function<SomeType>    Func ;
   *  typedef FunctionFromFunction  FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = sin( A )                         ; // short form
   *    FfF f2 = LoKi::ComposeFunction( sin , A ) ; // explicit form
   *  }
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Function
   *  @see LoKi::FunctionFromFunction
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE>
  class ComposeFunction : public Function<TYPE> 
  {
  public:
    /// define all nesessary types 
    _LOKI_FUNCTION_TYPES_( ComposeFunction , TYPE ) ;    
    typedef FunB           function   ;
    typedef double (*Func) ( double ) ;
    /// constructor 
    ComposeFunction ( Func               func           ,  
                      const function&    fun            , 
                      const std::string& desc = "'fun'" ) 
      : m_fun    ( fun ) , m_func   ( func ) , m_desc ( desc ) {} ;    
    /// virtual destructor 
    virtual ~ComposeFunction(){}
    /// clone method (mandatory!)
    virtual Self*  clone   () const { return new Self( *this ); }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument object ) const 
    { return (*m_func) ( fun( object ) ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << m_desc << "("  << fun() << ")" ; };
  protected:
    inline const function& fun(            ) const { return m_fun        ; }
    inline double          fun( argument a ) const { return m_fun( a )  ; }
  private:
    LoKi::FunctionFromFunction<TYPE> m_fun  ;
    Func                             m_func ;
    std::string                      m_desc ;
  };
  // ==========================================================================
  /** @struct ComposeFunction2
   *
   *  The helper structure to implement function of function
   *
   *  @code 
   *  typedef Function<SomeType>        Func ;
   *  typedef FunctionFromTwoFunctions  Ff2F ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    Ff2F f1 = pow( A , B )                          ; // short form
   *    Ff2F f2 = LoKi::ComposeFunction2( sin , A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *
   *  @see LoKi::Function
   *  @see LoKi::FunctionFromTwoFunction
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE>
  struct ComposeFunction2: public FunctionFromTwoFunctions<TYPE> 
  {
    /// define all nesessary types 
    _LOKI_FUNCTION_TYPES_( ComposeFunction2 , TYPE ) ;    
    typedef FunB           function ;
    typedef Constant<TYPE> constant ;
    typedef double (*Func) ( double , double )      ;
    /// constructor 
    ComposeFunction2 ( Func               func           ,  
                       const function&    fun1           , 
                       const function&    fun2           , 
                       const std::string& desc = "'fun'" )
      : FunctionFromTwoFunctions<TYPE> ( fun1 , fun2 ) 
      , m_func ( func ) , m_desc ( desc ) {};    
    /// constructor 
    ComposeFunction2 ( Func               func            ,  
                       const function&    fun1            , 
                       const double       val2            , 
                       const std::string& desc  = "'fun'" )
      : FunctionFromTwoFunctions<TYPE> ( fun1 ,  constant( val2 ) ) 
        , m_func ( func ) , m_desc ( desc ) {};    
    /// constructor 
    ComposeFunction2 ( Func               func            ,  
                       const double       val1            , 
                       const function&    fun2            ,
                       const std::string& desc  = "'fun'" )
      : FunctionFromTwoFunctions<TYPE> ( constant( val1 ) , fun2   ) 
        , m_func ( func ) , m_desc ( desc ) {};    
    /// constructor 
    ComposeFunction2 ( Func               func            ,  
                       const double       val1            , 
                       const double       val2            ,
                       const std::string& desc  = "'fun'" ) 
      : FunctionFromTwoFunctions<TYPE> ( constant( val1 ) ,  constant( val2 ) ) 
        , m_func ( func ) , m_desc ( desc ) {};    
    /// clone method (mandatory!)
    virtual Self*  clone   () const { return new Self( *this ); }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument object ) const 
    { return (*m_func) ( ( this->fun1 () )( object )  , ( this->fun2 () )( object) ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << m_desc << "("  << this->fun1() << "," << this->fun2() << ")" ; };
  private:
    Func m_func        ;
    std::string m_desc ;
  };  
  // ==========================================================================
  /** compare 2 objects using cmparison criteria CMP , 
   *  applied to functions:
   *  <c> cmp( f1(a) , f2(b) ) </c>
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */ 
  template<class TYPE,class CMP>
  class Compare : public std::binary_function<const TYPE,const TYPE,bool>
  {
  public:
    /// type for argument 
    typedef const TYPE&          argument ;
    /// typedef for actual function 
    typedef const Function<TYPE> function ;
    // typedef for comparison criteria 
    typedef const CMP            compare  ;
  public:
    /** constructor 
     *  @param fun1 the first functor 
     *  @param fun2 the second functor 
     */
    Compare( const function& fun1             , 
             const function& fun2             ,
             const compare&  cmp  = compare() )
      : m_fun1( fun1.clone() ) 
      , m_fun2( fun2.clone() ) 
      , m_cmp( cmp ) {};
    /** copy constructor 
     */
    Compare( const Compare& right ) 
      : m_fun1( right.m_fun1 -> clone() ) 
      , m_fun2( right.m_fun2 -> clone() ) 
      , m_cmp ( right.m_cmp             ) {};
    /// destructor 
    virtual ~Compare() 
    {
      if( 0 != m_fun1 ) { delete m_fun1 ; m_fun1 = 0 ; }
      if( 0 != m_fun2 ) { delete m_fun2 ; m_fun1 = 0 ; }
    };
    /// the only one essential method 
    bool operator() ( argument a1 , argument a2 ) const 
    { return m_cmp( (*m_fun1)( a1 ) ,  (*m_fun2)( a2 ) ) ; }
  private:
    Compare(){};
    Compare& operator=( const Compare& right );
  private:
    function* m_fun1 ;
    function* m_fun2 ;
    compare   m_cmp  ;
  };
  // ==========================================================================
  /** @class SimpleSwitch
   *  It is a function with acts similar to switch:
   *  
   *  result = condition ? value1 : value2 ;
   *  
   *  In particular it is useful for "converion" 
   *   of "predicates" into "functions"
   * 
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date 2005-02-11
   */
  template <class TYPE>
  class SimpleSwitch : public LoKi::Function<TYPE>
  {
  public:
    typedef          SimpleSwitch<TYPE>     Self        ;
    typedef typename LoKi::Function<TYPE>   FunB        ;
    typedef typename FunB::result_type      result_type ;
    typedef typename FunB::argument         argument    ;
  public:
    /** constructor from predicate and 2 constants 
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
    ( const LoKi::Predicate<TYPE>& cut           , 
      const double                 val1 =  true  ,
      const double                 val2 =  false )
      : FunB   (      ) 
      , m_cut  ( cut  ) 
      , m_val1 ( val1 )
      , m_val2 ( val2 )
    {};
    /** copy constructor 
     *  @right object to be copied 
     */
    SimpleSwitch ( const Self& right ) 
      : AuxFunBase ( right )
      , FunB       ( right        ) 
      , m_cut      ( right.m_cut  ) 
      , m_val1     ( right.m_val1 ) 
      , m_val2     ( right.m_val2 ) 
    {};
    /// destructor 
    virtual ~SimpleSwitch() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual Self* clone() const { return new Self( *this ) ; }
    /// MANDATORY: the only one essential method:
    virtual result_type operator() ( argument object ) const 
    { return m_cut( object ) ? m_val1 : m_val2 ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "("  << m_cut << "?"  << m_val1 << ":" << m_val2 << ")" ; };    
  private:
    SimpleSwitch() ;
  private:
    LoKi::PredicateFromPredicate<TYPE> m_cut  ;
    double                             m_val1 ;
    double                             m_val2 ;
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
  template<class TYPE>
  class Switch : public LoKi::Function<TYPE>
  {
  public:
    typedef          Switch<TYPE>           Self        ;
    typedef typename LoKi::Function<TYPE>   FunB        ;
    typedef typename FunB::result_type      result_type ;
    typedef typename FunB::argument         argument    ;
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
    ( const LoKi::Predicate<TYPE>& cut  , 
      const LoKi::Function<TYPE>&  fun1 , 
      const LoKi::Function<TYPE>&  fun2 )
      : FunB   (      ) 
      , m_cut  ( cut  ) 
      , m_fun1 ( fun1 ) 
      , m_fun2 ( fun2 )
    {};    
    /** constructor from predicate ,function and constant
     *
     *  The function return a value of the function 
     *  if predicate evaluates to "true", otherwise the 
     *  value of constant is returned 
     *
     *  @param cut predicate for branch
     *  @param fun1 the function
     *  @param val2 the constant 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-11
     */
    Switch
    ( const LoKi::Predicate<TYPE>& cut  , 
      const LoKi::Function<TYPE>&  fun1 , 
      const double                 val2 )
      : FunB   (      ) 
      , m_cut  ( cut  ) 
      , m_fun1 ( fun1 ) 
      , m_fun2 ( LoKi::Constant<TYPE>( val2 )  )
    {};    
    /** constructor from predicate ,function and constant 
     *
     *  The function return a value of the constant  
     *  if predicate evaluates to "true", otherwise the 
     *  value of function is returned 
     *
     *  @param cut predicate for branch
     *  @param val1 the constant 
     *  @param fun2 the function
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-11
     */
    Switch
    ( const LoKi::Predicate<TYPE>& cut  , 
      const double                 val1 ,
      const LoKi::Function<TYPE>&  fun2 ) 
      : FunB   (      ) 
      , m_cut  ( cut  ) 
      , m_fun1 ( LoKi::Constant<TYPE>( val1 )  )
      , m_fun2 ( fun2 ) 
    {};    
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
     *  @param val1 the first constant 
     *  @param val2 the second constant 
     *
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-11
     */
    Switch
    ( const LoKi::Predicate<TYPE>& cut           , 
      const double                 val1 =  true  ,
      const double                 val2 =  false )
      : FunB   (      ) 
      , m_cut  ( cut  ) 
      , m_fun1 ( LoKi::Constant<TYPE>( val1 )  )
      , m_fun2 ( LoKi::Constant<TYPE>( val2 )  )
    {};
    /** copy constructor 
     *  @right object to be copied 
     */
    Switch ( const Self& right ) 
      : AuxFunBase ( right        ) 
      , FunB       ( right        ) 
      , m_cut      ( right.m_cut  ) 
      , m_fun1     ( right.m_fun1 ) 
      , m_fun2     ( right.m_fun2 ) 
    {}
    /// destructor 
    virtual ~Switch() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual Self* clone() const { return new Self( *this ) ; }
    /// MANDATORY: the only one essential method:
    virtual result_type operator() ( argument object ) const 
    { return m_cut( object ) ? m_fun1( object ) : m_fun1( object ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "("  << m_cut << "?"  << m_fun1 << ":" << m_fun2 << ")" ; };
  private:
    Switch() ;
  private:
    LoKi::PredicateFromPredicate<TYPE> m_cut  ;
    LoKi::FunctionFromFunction<TYPE>   m_fun1 ;
    LoKi::FunctionFromFunction<TYPE>   m_fun2 ;
  };  
  // ==========================================================================
  /** @class Valid 
   *  The trivial predicate to check the validity of argument.
   *  It is OK for any pointer-like types or for types with
   *  implemented implicit conversion to "bool" 
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date 2004-02-11
   */
  template <class TYPE>
  class Valid : public LoKi::Predicate<TYPE>
  {
  public:
    typedef          Valid<TYPE>             Self        ;
    typedef typename LoKi::Predicate<TYPE>   FunB        ;
    typedef typename FunB::result_type       result_type ;
    typedef typename FunB::argument          argument    ;
  public:
    /// constructor 
    Valid() : FunB() {} ;
    /// copy constructor 
    Valid( const Valid& right ) 
      : LoKi::AuxFunBase ( right ) 
      , FunB             ( right ) {};
    /// virtual destructor 
    virtual ~Valid() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual Self* clone() const { return new Self( *this ) ; }
    /// MANDATORY: the only one essential method 
    virtual result_type operator() ( argument object ) const 
    { return LoKi::valid ( object ) ? true : false  ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(Valid?)"; };
  };  
  // ==========================================================================
  /** @class TheSame 
   *  Trivial predicate to check if the argument 
   *  is equal to some predefined value. 
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date 2004-02-11
   */
  template <class TYPE>
  class TheSame : public LoKi::Predicate<TYPE>
  {
  public:
    typedef typename LoKi::Predicate<TYPE>::argument    argument     ;
    typedef typename LoKi::Predicate<TYPE>::result_type result_type  ;
  public:
    /// constructor form the value 
    TheSame 
    ( argument value ) 
      : LoKi::Predicate<TYPE>() 
      , m_value ( value ) 
    {}
    /// copy constructor 
    TheSame
    ( const TheSame& right ) 
      : LoKi::AuxFunBase     ( right ) 
      , LoKi::Predicate<TYPE>( right )
      , m_value              ( right.m_value )
    {}
    /// virtual destructor 
    virtual ~TheSame() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  TheSame* clone() const { return new TheSame( *this ) ; }
    /// MANDATORY: the only one essential method 
    virtual result_type operator() ( argument object ) const 
    { return m_value == object ? true : false ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "(SAME?)"; };
  private :
    // the default contructor is disabled
    TheSame();
  private:
    // the predefined value 
    typename LoKi::Field<argument>::Type m_value ;
  };  
  // ==========================================================================
  /** @class EqualToValue 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-04-07
   */
  template <class TYPE>
  class EqualToValue 
    : public PredicateFromFunctionAndValue<TYPE>
  {
  public:
    typedef PredicateFromFunctionAndValue<TYPE> _Base ;
    typedef typename _Base::_Function       _Function ;
    typedef typename _Base::value_type     value_type ;
    typedef typename _Base::result_type   result_type ;
    typedef typename _Base::argument         argument ;
  public:
    /** constructor fro the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     */
    EqualToValue 
    ( const _Function&  fun , 
      const value_type& val ) 
      : PredicateFromFunctionAndValue<TYPE>( fun , val ) 
    {};
    /** constructor fro the function and the value 
     *  @param val the reference value 
     */
    EqualToValue 
    ( const value_type& val ,
      const _Function&  fun )
      : PredicateFromFunctionAndValue<TYPE>( fun , val ) 
    {};
    /// copy contructor 
    EqualToValue 
    ( const EqualToValue& right )
      : LoKi::AuxFunBase                   ( right ) 
      , PredicateFromFunctionAndValue<TYPE>( right )
    {};
    /// MANDATORY: virtual destructor 
    virtual ~EqualToValue(){} ;
    /// MANDATORY: clone method ("virtual construcor")
    virtual  EqualToValue* clone() const { return new EqualToValue(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument a ) const
    { return this->value() == this->fun( a )  ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << "(" << this->fun() << "==" << this->value() << ")" ; }
  private:
    /// The default constructor is disabled 
    EqualToValue();
    /// The assignement operator is disabled 
    EqualToValue& operator=( const  EqualToValue& ) ;
  };  
  // ==========================================================================
  /** @class NotEqualToValue 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-04-07
   */
  template <class TYPE>
  class NotEqualToValue 
    : public PredicateFromFunctionAndValue<TYPE>
  {
  public:
    typedef PredicateFromFunctionAndValue<TYPE> _Base ;
    typedef typename _Base::_Function       _Function ;
    typedef typename _Base::value_type     value_type ;
    typedef typename _Base::result_type   result_type ;
    typedef typename _Base::argument         argument ;
  public:
    /** constructor fro the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     */
    NotEqualToValue 
    ( const _Function&  fun , 
      const value_type& val ) 
      : PredicateFromFunctionAndValue<TYPE>( fun , val ) 
    {};
    /** constructor fro the function and the value 
     *  @param val the reference value 
     */
    NotEqualToValue 
    ( const value_type& val ,
      const _Function&  fun )
      : PredicateFromFunctionAndValue<TYPE>( fun , val ) 
    {};
    /// copy contructor 
    NotEqualToValue 
    ( const NotEqualToValue& right )
      : LoKi::AuxFunBase                   ( right ) 
      , PredicateFromFunctionAndValue<TYPE>( right )
    {};
    /// MANDATORY: virtual destructor 
    virtual ~NotEqualToValue(){} ;
    /// MANDATORY: clone method ("virtual construcor")
    virtual  NotEqualToValue* clone() const { return new NotEqualToValue(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument a ) const
    { return this->value() != this->fun( a )  ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << "(" << this->fun() << "!=" << this->value() << ")" ; }
  private:
    /// The default constructor is disabled 
    NotEqualToValue();
    /// The assignement operator is disabled 
    NotEqualToValue& operator=( const  NotEqualToValue& ) ;
  };
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Math.h"
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PRIMITIVES_H
// ============================================================================
