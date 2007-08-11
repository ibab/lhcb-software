// $Id: Functions.h,v 1.20 2007-08-11 20:17:00 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_FUNCTIONS_H 
#define LOKI_FUNCTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
#include <vector>
#include <string>
#include <ostream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AuxFunBase.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/call_traits.hpp"
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
  /** @class Function
   *  basic abstract type for LoKi function 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE>
  class Function 
    : public    std::unary_function<const TYPE,double> 
    , public virtual LoKi::AuxFunBase
  {
  public:
    /// the type of the argument
    typedef TYPE        Type ;
    /// the actual type of base 
    typedef typename std::unary_function<const TYPE,double>  Base_F ;
    typedef typename std::unary_function<const TYPE,double>  Base_1 ;
    /// the result value 
    typedef typename Base_F::result_type   result_type   ;
    /// the basic argument type 
    typedef typename Base_F::argument_type argument_type ;
    /// vector or results 
    typedef std::vector<result_type>       vector_result ;
    /// type for the argument 
    typedef typename boost::call_traits<const TYPE>::param_type argument;
    /// common FunB class 
    typedef Function<TYPE>  FunB ;
    /// own type 
    typedef Function<TYPE>  Self ;     
    /// the only one essential method ("function")
    virtual result_type operator () ( argument    ) const = 0 ;
    /// the only one essential method ("function")
    virtual result_type evaluate    ( argument  a ) const 
    { return (*this)( a ) ; }
    /// the only one essential method ("function")
    virtual result_type eval        ( argument  a ) const 
    { return (*this)( a ) ; }
    /// clone method 
    virtual FunB*  clone    ()                 const = 0 ;
    /// virtual destructor 
    virtual ~Function(){}
  public:
    /** apply the function to the sequence of arguments 
     *  and produce the sequence of 
     *  "results" - similar to Python's "map" function
     *  @code 
     *  
     *   CONTAINER                 objects  = ... ; 
     *   const Function<SOMETYPE>& function = ... ;
     *   std:vector<double>        result ;
     *   function.evaluate ( objects.begin     ()       , 
     *                       objects.end       ()       , 
     *                       std::back_inserter( result ) ) ;
     *  @endcode
     *  @param  first  begin iterator of the argument sequence
     *  @param  last   end iterator of the argument sequence
     *  @param  output output iterator for the result 
     *  @return length of the result sequence
     */
    template <class INPUT,class OUTPUT>
    inline size_t evaluate
    ( INPUT  first  , 
      INPUT  last   , 
      OUTPUT output ) const 
    {
      // overall length 
      size_t length = 0  ;
      // 'transform'
      for ( ; first != last ; ++first , ++output , ++length ) 
      { *output = (*this)( *first ) ; }
      return length ;
    };   
    /** apply the function to the sequence of arguments 
     *  and produce the sequence of 
     *  "results" - similar to Python's "map" function
     *  @code 
     *   CONTAINER                 objects  = ... ; 
     *   const Function<SOMETYPE>& function = ... ;
     *   std::vector<double> result  = 
     *     function.evaluate ( objects.begin () ,
     *                         objects.end   () ) ;
     *  @endcode
     *  @param  first  begin iterator of the argument sequence
     *  @param  last   end iterator of the argument sequence
     *  @return container of results
     */ 
    template <class INPUT>
    inline vector_result evaluate
    ( INPUT  first  , 
      INPUT  last   ) const 
    {
      vector_result result ( std::distance ( first , last ) );
      evaluate( first , last , result.begin() ) ;
      return result ;
    };
  protected:
    /// protected default constructor 
    Function(): AuxFunBase() {};
    /// protected copy constructor
    Function( const Function& fun ) : AuxFunBase( fun ) {};
  private:
    /// assignement         is private 
    Self& operator=( const Self& );
  };  
  // ==========================================================================
  /** @class Predicate
   *  The basic abstract for LoKi predicate 
   *  (the function with boolean return value)
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  template <class TYPE>
  class Predicate 
    : public std::unary_function<const TYPE,bool> 
    , virtual public LoKi::AuxFunBase 
  {
  public:
    /// the type of the argument
    typedef TYPE        Type ;
    /// the actual type of base 
    typedef typename std::unary_function<const TYPE,bool>  Base_P ;
    typedef typename std::unary_function<const TYPE,bool>  Base_1 ;
    /// the result value 
    typedef typename Base_P::result_type   result_type   ;
    /// the basic argument type 
    typedef typename Base_P::argument_type argument_type ;
    /// vector of results 
    typedef std::vector<result_type>       vector_result ;
    /// type for argument 
    typedef typename boost::call_traits<const TYPE>::param_type argument;
    /// common FunB class 
    typedef Predicate<TYPE>  FunB     ;
    /// own type 
    typedef Predicate<TYPE>  Self     ;     
    /// the only one essential method ("function")
    virtual bool  operator () ( argument   ) const = 0 ;
    /// the only one essential method ("function")
    virtual bool  evaluate    ( argument a ) const 
    { return (*this)( a ) ; }
    /// the only one essential method ("function")
    virtual bool  eval        ( argument a ) const 
    { return (*this)( a ) ; }
    /// clone method 
    virtual FunB* clone    ()              const = 0 ;
    /// virtual destructor 
    virtual ~Predicate(){}
  public:
    /** apply the predicate to the sequence of arguments 
     *  and produce the sequence of 
     *  "results" - similar to Python's "map" function
     *  @code 
     *  
     *   CONTAINER                  objects  = ... ; 
     *   const Predicate<SOMETYPE>& predicate= ... ;
     *   std:vector<double>         result ;
     *   predicate.evaluate ( objects.begin     ()       , 
     *                        objects.end       ()       , 
     *                        std::back_inserter( result ) ) ;
     *  @endcode
     *  @param  first  begin iterator of the argument sequence
     *  @param  last   end iterator of the argument sequence
     *  @param  output output iterator for the result 
     *  @return length of the result sequence
     */
    template <class INPUT,class OUTPUT>
    inline size_t evaluate
    ( INPUT  first  , 
      INPUT  last   , 
      OUTPUT output ) const 
    {
      // overall length 
      size_t length = 0 ;
      // 'transform'
      for ( ; first != last ; ++first , ++output , ++length ) 
      { *output = (*this)( *first ) ; }
      return length ;
    };
    /** apply the predicate to the sequence of arguments 
     *  and produce the sequence of 
     *  "results" - similar to Python's "map" function
     *  @code 
     *  
     *   CONTAINER                  objects  = ... ; 
     *   const Predicate<SOMETYPE>& predicate = ... ;
     *   std:vector<bool> result = 
     *   predicate.evaluate ( objects.begin () ,
     *                        objects.end   () ) ;
     *  @endcode
     *  @param  first  begin iterator of the argument sequence
     *  @param  last   end iterator of the argument sequence
     *  @return container of results
     */ 
    template <class INPUT>
    inline vector_result evaluate
    ( INPUT  first  , 
      INPUT  last   ) const 
    {
      vector_result result ( std::distance ( first , last ) );
      evaluate ( first , last , result.begin() ) ;
      return result ;
    } ;
  protected:
    /// protected default constructor 
    Predicate(): AuxFunBase() {};
    /// protected copy constructor
    Predicate ( const Predicate& pred ) : AuxFunBase( pred ) {};
  private:
    /// assignement         is private 
    Predicate& operator=( const Predicate& );
  } ;
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
    /// delegate the object type
    virtual std::string   objType () const { return m_fun -> objType() ; }
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
    inline const function&   fun () const { return *m_fun ; }
  private:
    /// default constructor is private 
    FunctionFromFunction();
  private:
    // the underlaying function 
    const function* m_fun ; ///< the underlaying function 
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
   *  typede Predicate<SomeType>              Pred ;
   *  typedef PredicateFromPredicate<SomeType> PfP  ;
   *  
   *  void func( const Pred& A , const Pred& B ) 
   *  {
   *    // construct different predicates from other predicates  
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
    /// delegate the object type
    virtual std::string   objType () const { return m_pr -> objType() ; }
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
    // the underlying predicate 
    const FunB* m_pr ; ///< the underlying predicate 
  };
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_FUNCTIONS_H
// ============================================================================
