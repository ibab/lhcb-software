// $Id: BiFunctions.h,v 1.3 2007-08-11 20:17:00 ibelyaev Exp $
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
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-06-08
   */
  template <class TYPE1,class TYPE2>
  class Function<LoKi::Holder<TYPE1,TYPE2> >
    : public    std::unary_function<const LoKi::Holder<TYPE1,TYPE2>,double> 
    , virtual   public LoKi::AuxFunBase
  {
  public:
    /// the type of the argument
    typedef LoKi::Holder<TYPE1,TYPE2> TYPE ;
    typedef LoKi::Holder<TYPE1,TYPE2> Type ;
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
    typedef typename boost::call_traits<const TYPE>::param_type  argument;
    /// type for the first argument 
    typedef typename boost::call_traits<const TYPE1>::param_type first_argument      ;
    /// type for the first argument 
    typedef typename boost::call_traits<const TYPE1>::param_type first_argument_type ;
    /// type for the second argument 
    typedef typename boost::call_traits<const TYPE2>::param_type second_argument     ;
    /// type for the second argument 
    typedef typename boost::call_traits<const TYPE2>::param_type second_argument_type ;
    /// common FunB class 
    typedef Function<TYPE>  FunB ;
    /// own type 
    typedef Function<TYPE>  Self ;
    /// the native interface with 1 argument: 
    virtual typename Self::result_type operator ()  ( argument a ) const = 0 ;
    /// the only one essential method ("function")
    virtual typename Self::result_type  evaluate    ( argument a ) const 
    { return (*this)( a ) ; }    
    /// the only one essential method ("function")
    virtual typename Self::result_type  eval        ( argument a ) const 
    { return (*this)( a ) ; }
    /// the only one essential method ("function")
    virtual typename Self::result_type operator () 
      ( first_argument  a1 , 
        second_argument a2 ) const
    { return (*this)( Type ( a1 , a2 ) ) ; }
    /// the only one essential method ("function")
    virtual typename Self::result_type evaluate  
    ( first_argument  a1 , 
      second_argument a2 ) const
    { return (*this)( a1 , a2 ) ; }
    /// the only one essential method ("function")
    virtual typename Self::result_type eval    
    ( first_argument  a1 , 
      second_argument a2 ) const
    { return (*this)( a1 , a2 ) ; }
    /// MANDATORY: clone method 
    virtual Self* clone    ()              const = 0 ;
    /// virtual destructor 
    virtual ~Function(){} ;
  public:
    // ========================================================================
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
    // ========================================================================
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
    // ========================================================================
    /** apply the function to the 2 sequences of arguments 
     *  and produce the sequence of 
     *  "results" - similar to Python's "map" function
     *  @code 
     *  
     *   CONTAINER1                objects  = ... ; 
     *   CONTAINER2                objects2 = ... ; 
     *   const Function<SOMETYPE>& function = ... ;
     *   std:vector<double>        result ;
     *   function.evaluate ( objects.begin     ()       , 
     *                       objects.end       ()       , 
     *                       objects2.begin    ()       ,
     *                       std::back_inserter( result ) ) ;
     *  @endcode
     *  @param  first  begin iterator of the argument sequence
     *  @param  last   end iterator of the argument sequence
     *  @param  output output iterator for the result 
     *  @return length of the result sequence
     */
    template <class INPUT1,class INPUT2,class OUTPUT>
    inline size_t evaluate
    ( INPUT1 first  , 
      INPUT1 last   , 
      INPUT2 first2 ,
      OUTPUT output ) const 
    {
      // overall length 
      size_t length = 0  ;
      // 'transform'
      for ( ; first != last ; ++first , ++output , ++length , ++first2 ) 
      { *output = (*this)( *first , *first2 ) ; }
      return length ;
    };   
  protected:
    /// protected default constructor 
    Function(): AuxFunBase() {};
    /// protected copy constructor
    Function( const Function& fun ) : AuxFunBase( fun ) {};
  private:
    /// assignement         is private 
    Self& operator=( const Self& );
  } ;
  // ==========================================================================
  /** mimic 2-argument predicate
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-06-08
   */
  template <class TYPE1,class TYPE2>
  class Predicate<LoKi::Holder<TYPE1,TYPE2> >
    : public    std::unary_function<const LoKi::Holder<TYPE1,TYPE2>,bool> 
    , public    std::binary_function<const TYPE1,const TYPE2,bool> 
    , virtual   public LoKi::AuxFunBase
  {
  public:
    /// the type of the argument
    typedef LoKi::Holder<TYPE1,TYPE2> Type ;
    typedef LoKi::Holder<TYPE1,TYPE2> TYPE ;
     /// the actual type of base 
    typedef typename std::unary_function<const TYPE,bool>  Base_F ;
    typedef typename std::unary_function<const TYPE,bool>  Base_1 ;
    typedef typename std::binary_function<const TYPE1,const TYPE2,bool>  Base_2 ;
    /// the result value 
    typedef typename Base_F::result_type   result_type   ;
    /// the basic argument type 
    typedef typename Base_F::argument_type argument_type ;
    /// vector or results 
    typedef std::vector<result_type>       vector_result ;
    /// type for the argument 
    typedef typename boost::call_traits<const TYPE>::param_type argument;
    /// type for the first argument 
    typedef typename boost::call_traits<const TYPE1>::param_type first_argument      ;
    /// type for the first argument 
    typedef typename boost::call_traits<const TYPE1>::param_type first_argument_type ;
    /// type for the second argument 
    typedef typename boost::call_traits<const TYPE2>::param_type second_argument     ;
    /// type for the second argument 
    typedef typename boost::call_traits<const TYPE2>::param_type second_argument_type ;
    /// common FunB class 
    typedef Predicate<TYPE>  FunB ;
    /// own type 
    typedef Predicate<TYPE>  Self ;
    /// the native interface with 1 argument: 
    virtual typename Self::result_type operator ()  ( argument a ) const = 0 ;
    /// the only one essential method ("function")
    virtual typename Self::result_type  evaluate    ( argument a ) const 
    { return (*this)( a ) ; }    
    /// the only one essential method ("function")
    virtual typename Self::result_type  eval        ( argument a ) const 
    { return (*this)( a ) ; }
    /// the only one essential method ("function")
    virtual typename Self::result_type operator () 
      ( first_argument  a1 , 
        second_argument a2 ) const
    { return (*this)( Type ( a1 , a2 ) ) ; }
    /// the only one essential method ("function")
    virtual typename Self::result_type evaluate  
    ( first_argument  a1 , 
      second_argument a2 ) const
    { return (*this)( a1 , a2 ) ; }
    /// the only one essential method ("function")
    virtual typename Self::result_type eval    
    ( first_argument  a1 , 
      second_argument a2 ) const
    { return (*this)( a1 , a2 ) ; }
    /// MANDATORY: clone method 
    virtual Self* clone    ()              const = 0 ;
    /// virtual destructor 
    virtual ~Predicate (){} ;
  public:
    // ========================================================================
    /** apply the predicate to the sequence of arguments 
     *  and produce the sequence of 
     *  "results" - similar to Python's "map" function
     *  @code 
     *  
     *   CONTAINER                  objects  = ... ; 
     *   const Predicate<SOMETYPE>& predicate= ... ;
     *   std:vector<bool>           result ;
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
    // ========================================================================
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
    // ========================================================================
  protected:
    /// protected default constructor 
    Predicate(): AuxFunBase() {};
    /// protected copy constructor
    Predicate ( const Predicate& pred ) : AuxFunBase( pred ) {};
  private:
    /// assignement         is private 
    Predicate& operator=( const Predicate& );
  } ; 
} // end of namespace LoKi 
// ============================================================================
#endif // LOKI_BIFUNCTIONS_H
// ============================================================================
