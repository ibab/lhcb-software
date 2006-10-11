// $Id: Operators.h,v 1.4 2006-10-11 12:14:16 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/05/02 14:29:10  ibelyaev
//  censored
//
// ============================================================================
#ifndef LOKI_OPERATORS_H 
#define LOKI_OPERATORS_H 1
// ============================================================================
// Include files
// ============================================================================
#include "LoKi/Functions.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Inline operators to deal with LoKi functions and predicates   
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


// ============================================================================
/** @fn operator&& 
 *  inline construction of logical 'AND'
 *  @code 
 *  typedef Predicate<SomeType>     Pred ;
 *  typedef PredicateFromPredicate  PfP  ;
 *
 *  void func( const Pred& A , const Pred& B )
 *  {
 *    PfP p1 = A && B             ; // operator form
 *  }
 *  @endcode 
 *  @see LoKi::And
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator&&  ( const  LoKi::Predicate<TYPE>& fun1 , 
              const  LoKi::Predicate<TYPE>& fun2 ) 
{ return LoKi::And<TYPE>  ( fun1   , fun2   )    ; } ;
// ============================================================================

// ============================================================================
/** @fn operator& 
 *  it is an alias for <tt>operator&&</tt> method 
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator&   ( const  LoKi::Predicate<TYPE>& fun1 , 
              const  LoKi::Predicate<TYPE>& fun2 ) 
{ return fun1 && fun2 ; } ;
// ============================================================================

// ============================================================================
/** @fn operator&& 
 *  inline construction of logical 'AND' 
 *  @see LoKi::And
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator&&  ( const  LoKi::Predicate<TYPE>& fun  , 
              const  bool                   val  ) 
{ return fun && LoKi::BooleanConstant<TYPE>( val ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator& 
 *  it is an alias for <tt>operator&&</tt> method 
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator&   ( const  LoKi::Predicate<TYPE>& fun  , 
              const  bool                   val  ) 
{ return fun && val ; } ;
// ============================================================================

// ============================================================================
/** @fn operator&& 
 *  inline construction of logical 'AND' 
 *  @see LoKi::And
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator&&  ( const  bool                   val  ,
              const  LoKi::Predicate<TYPE>& fun  ) 
{ return fun && val ; } ;
// ============================================================================

// ============================================================================
/** @fn operator& 
 *  it is an alias for <tt>operator&&</tt> method 
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator&   ( const  bool                   val  ,
              const  LoKi::Predicate<TYPE>& fun  ) 
{ return fun && val ; } ;
// ============================================================================


// ============================================================================
/** @fn operator||
 *  inline construction of logical 'OR' 
 *  @code 
 *  typedef Predicate<SomeType>     Pred ;
 *  typedef PredicateFromPredicate  PfP  ;
 *
 *  void func( const Pred& A , const Pred& B )
 *  {
 *    PfP p1 = A || B             ; // operator form
 *  }
 *  @endcode 
 *  @see LoKi::Or
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator|| ( const  LoKi::Predicate<TYPE>& fun1 , 
             const  LoKi::Predicate<TYPE>& fun2 ) 
{ return LoKi::Or<TYPE>     ( fun1   , fun2   ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator|
 *  it is an alias for <tt>operator|</tt> method 
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator|  ( const  LoKi::Predicate<TYPE>& fun1 , 
             const  LoKi::Predicate<TYPE>& fun2 ) 
{ return fun1 || fun2 ; } ;
// ============================================================================

// ============================================================================
/** @fn operator|| 
 *  inline construction of logical 'OR' 
 *  @see LoKi::BooleanConstant
 *  @see LoKi::Or
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator||  ( const  LoKi::Predicate<TYPE>& fun  , 
              const  bool                   val  ) 
{ return fun || LoKi::BooleanConstant<TYPE>( val ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator|
 *  it is an alias for <tt>operator|</tt> method 
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator|   ( const  LoKi::Predicate<TYPE>& fun  , 
              const  bool                   val  ) 
{ return fun || val  ; } ;
// ============================================================================

// ============================================================================
/** @fn operator|| 
 *  inline construction of logical 'OR' 
 *  @see LoKi::Or
 *  @see LoKi::BooleanConstant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator|| ( const  bool                   val  ,
             const  LoKi::Predicate<TYPE>& fun  ) 
{ return fun || val ; } ;
// ============================================================================

// ============================================================================
/** @fn operator|
 *  it is an alias for <tt>operator|</tt> method 
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator|  ( const  bool                   val  ,
             const  LoKi::Predicate<TYPE>& fun  ) 
{ return fun || val ; } ;
// ============================================================================

// ============================================================================
/** @fn operator!
 *  it is an alias for <tt>operator!</tt> method 
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator!  ( const  LoKi::Predicate<TYPE>& fun  ) 
{ return LoKi::Not<TYPE>( fun )  ; } ;
// ============================================================================

// ============================================================================
/** @fn operator<
 *  inline construction of operator "<" 
 *  @code 
 *  typedef Function<SomeType>     Func ;
 *  typedef PredicateFromPredicate PfP  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    PfP f = A < B             ; // operator form
 *  }
 *  @endcode 
 *  @see LoKi::Less
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator<  ( const  LoKi::Function<TYPE>&  fun1 , 
             const  LoKi::Function<TYPE>&  fun2 ) 
{ return LoKi::Less<TYPE>   ( fun1   , fun2   ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator<
 *  inline construction of operator "<" 
 *  @see LoKi::Less
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator<  ( const  LoKi::Function<TYPE>&  fun  , 
             const  double                 val  ) 
{ return fun < LoKi::Constant<TYPE>( val ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator<
 *  inline construction of operator "<" 
 *  @see LoKi::Less
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator<  ( const  double                 val  , 
             const  LoKi::Function<TYPE>&  fun  ) 
{ return LoKi::Constant<TYPE>( val ) < fun ; } ;
// ============================================================================

// ============================================================================
/** @fn operator>
 *  inline construction of operator ">" 
 *  @code 
 *  typedef Function<SomeType>     Func ;
 *  typedef PredicateFromPredicate PfP  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    PfP f = A > B             ; // operator form
 *  }
 *  @endcode 
 *  @see LoKi::Less
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator>  ( const  LoKi::Function<TYPE>&  fun1 , 
             const  LoKi::Function<TYPE>&  fun2 ) 
{ return fun2 < fun1 ; }
// ============================================================================

// ============================================================================
/** @fn operator>
 *  inline construction of operator ">" 
 *  @see LoKi::Less
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator>  ( const  LoKi::Function<TYPE>&  fun  , 
             const  double                 val  ) 
{ return LoKi::Constant<TYPE>( val ) < fun ; } ;
// ============================================================================

// ============================================================================
/** @fn operator>
 *  inline construction of operator ">" 
 *  @see LoKi::Less
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator>  ( const  double                 val  , 
             const  LoKi::Function<TYPE>&  fun  ) 
{ return fun < LoKi::Constant<TYPE>( val ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator==
 *  inline construction of operator "==" 
 *  @code 
 *  typedef Function<SomeType>     Func ;
 *  typedef PredicateFromPredicate PfP  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    PfP f = A == B             ; // operator form
 *  }
 *  @endcode 
 *  @see LoKi::Equal
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator== ( const LoKi::Function<TYPE>&  fun1 , 
             const LoKi::Function<TYPE>&  fun2 ) 
{ return LoKi::Equal<TYPE>  ( fun1   , fun2   ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator==
 *  inline construction of operator "==" 
 *  @see LoKi::Equal
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator== ( const LoKi::Function<TYPE>&  fun  , 
             const double                 val  ) 
{ return LoKi::EqualToValue<TYPE>( fun , val ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator==
 *  inline construction of operator "==" 
 *  @see LoKi::Equal
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator== ( const double                 val  , 
             const LoKi::Function<TYPE>&  fun  )
{ return LoKi::EqualToValue<TYPE> ( val, fun ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator<=
 *  inline construction of operator "<=" 
 *  @see LoKi::LessOrEqual
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator<= ( const  LoKi::Function<TYPE>&  fun1 , 
             const  LoKi::Function<TYPE>&  fun2 ) 
{ return LoKi::LessOrEqual<TYPE> ( fun1 , fun2 ) ;} ;
// ============================================================================

// ============================================================================
/** @fn operator<=
 *  inline construction of operator "<=" 
 *  @code 
 *  typedef Function<SomeType>     Func ;
 *  typedef PredicateFromPredicate PfP  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    PfP f = A >= B             ; // operator form
 *  }
 *  @endcode 
 *  @see LoKi::LessOrEqual
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator<= ( const  LoKi::Function<TYPE>&  fun  , 
             const  double                 val  ) 
{ return  fun <= LoKi::Constant<TYPE> ( val ) ;} ;
// ============================================================================

// ============================================================================
/** @fn operator<=
 *  inline construction of operator "<=" 
 *  @see LoKi::LessOrEqual
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator<= ( const  double                 val  ,
             const  LoKi::Function<TYPE>&  fun  )
{ return  LoKi::Constant<TYPE>( val ) <= fun ;} ;
// ============================================================================

// ============================================================================
/** @fn operator>=
 *  inline construction of operator ">=" 
 *  @code 
 *  typedef Function<SomeType>     Func ;
 *  typedef PredicateFromPredicate PfP  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    PfP f = A >= B             ; // operator form
 *  }
 *  @endcode 
 *  @see LoKi::LessOrEqual
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator>= ( const  LoKi::Function<TYPE>&  fun1 , 
             const  LoKi::Function<TYPE>&  fun2 ) 
{ return ( fun2 <= fun1 ) ; }
// ============================================================================

// ============================================================================
/** @fn operator>=
 *  inline construction of operator ">=" 
 *  @see LoKi::LessOrEqual
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator>= ( const  LoKi::Function<TYPE>&  fun  , 
             const  double                 val  ) 
{ return ( val <= fun ) ; }
// ============================================================================

// ============================================================================
/** @fn operator>=
 *  inline construction of operator ">=" 
 *  @see LoKi::LessOrEqual
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator>= ( const  double                 val  ,
             const  LoKi::Function<TYPE>&  fun  )
{ return ( fun <= val ) ; }
// ============================================================================

// ============================================================================
/** @fn operator!=
 *  inline construction of operator "!=" 
 *  @code 
 *  typedef Function<SomeType>     Func ;
 *  typedef PredicateFromPredicate PfP  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    PfP f = A != B             ; // operator form
 *  }
 *  @endcode 
 *  @see LoKi::Equal
 *  @see LoKi::Not
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator!= ( const LoKi::Function<TYPE>&  fun1 , 
             const LoKi::Function<TYPE>&  fun2 ) 
{ return LoKi::NotEqual<TYPE> ( fun1 , fun2 ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator!=
 *  inline construction of operator "!=" 
 *  @see LoKi::Equal
 *  @see LoKi::Not
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator!= ( const LoKi::Function<TYPE>&  fun  , 
             const double                 val  ) 
{ return LoKi::NotEqualToValue<TYPE> ( fun , val ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator!=
 *  inline construction of operator "!=" 
 *  @see LoKi::Equal
 *  @see LoKi::Not
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::PredicateFromPredicate<TYPE> 
operator!= ( const double                 val  ,
             const LoKi::Function<TYPE>&  fun  )
{ return LoKi::NotEqualToValue<TYPE> ( val , fun ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator+
 *  inline construction of operator "+" 
 *  @code 
 *  typedef Function<SomeType>     Func ;
 *  typedef FunctionFromFunction   FfF  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    FfF f = A + B             ; // operator form
 *  }
 *  @endcode 
 *  @see LoKi::Plus
 *  @return functor
 */
template <class TYPE>
inline   LoKi::FunctionFromFunction<TYPE> 
operator+ ( const LoKi::Function<TYPE>&  fun1 , 
            const LoKi::Function<TYPE>&  fun2 ) 
{ return LoKi::Plus<TYPE>    ( fun1  , fun2 ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator+
 *  inline construction of operator "+" 
 *  @see LoKi::Plus
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::FunctionFromFunction<TYPE> 
operator+ ( const LoKi::Function<TYPE>&  fun  , 
            const double                 val  ) 
{ return fun + LoKi::Constant<TYPE>( val )  ; } ;
// ============================================================================

// ============================================================================
/** @fn operator+
 *  inline construction of operator "+" 
 *  @see LoKi::Plus
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::FunctionFromFunction<TYPE> 
operator+ ( const double                 val  ,
            const LoKi::Function<TYPE>&  fun  )
{ return fun + val ; } ;
// ============================================================================

// ============================================================================
/** @fn operator-
 *  inline construction of operator "-" 
 *  @code 
 *  typedef Function<SomeType>     Func ;
 *  typedef FunctionFromFunction   FfF  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    FfF f = A - B             ; // operator form
 *  }
 *  @endcode 
 *  @see LoKi::Minus
 *  @return functor
 */
template <class TYPE>
inline   LoKi::FunctionFromFunction<TYPE> 
operator- ( const LoKi::Function<TYPE>&  fun1 , 
            const LoKi::Function<TYPE>&  fun2 ) 
{ return LoKi::Minus<TYPE>   ( fun1  , fun2 ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator-
 *  inline construction of operator "-" 
 *  @see LoKi::Minus
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::FunctionFromFunction<TYPE> 
operator- ( const LoKi::Function<TYPE>&  fun  , 
            const double                 val  ) 
{ return LoKi::Minus<TYPE>   ( fun  , LoKi::Constant<TYPE>( val )  ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator-
 *  inline construction of operator "-" 
 *  @see LoKi::Minus
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::FunctionFromFunction<TYPE> 
operator- ( const double                 val  ,
            const LoKi::Function<TYPE>&  fun  )
{ return LoKi::Minus<TYPE>   ( LoKi::Constant<TYPE>( val )  , fun ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator/
 *  inline construction of operator "/" 
 *  @code 
 *  typedef Function<SomeType>     Func ;
 *  typedef FunctionFromFunction   FfF  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    FfF f = A / B             ; // operator form
 *  }
 *  @endcode 
 *  @see LoKi::Divide
 *  @return functor
 */
template <class TYPE>
inline   LoKi::FunctionFromFunction<TYPE> 
operator/ ( const LoKi::Function<TYPE>&  fun1 , 
            const LoKi::Function<TYPE>&  fun2 ) 
{ return LoKi::Divide<TYPE>  ( fun1  , fun2 ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator/
 *  inline construction of operator "/" 
 *  @see LoKi::Divide
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::FunctionFromFunction<TYPE> 
operator/ ( const LoKi::Function<TYPE>&  fun  , 
            const double                 val  ) 
{ return LoKi::Divide<TYPE>  ( fun  , LoKi::Constant<TYPE>( val ) ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator/
 *  inline construction of operator "/" 
 *  @see LoKi::Divide
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::FunctionFromFunction<TYPE> 
operator/ ( const double                 val  ,
            const LoKi::Function<TYPE>&  fun  ) 
{ return LoKi::Divide<TYPE>  ( LoKi::Constant<TYPE>( val ) , fun ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator*
 *  inline construction of operator "*" 
 *  @code 
 *  typedef Function<SomeType>     Func ;
 *  typedef FunctionFromFunction   FfF  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    FfF f = A * B             ; // operator form
 *  }
 *  @endcode 
 *  @see LoKi::Multiply
 *  @return functor
 */
template <class TYPE>
inline   LoKi::FunctionFromFunction<TYPE> 
operator* ( const LoKi::Function<TYPE>&  fun1 , 
            const LoKi::Function<TYPE>&  fun2 ) 
{ return LoKi::Multiply<TYPE>  ( fun1  , fun2 ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator*
 *  inline construction of operator "*" 
 *  @see LoKi::Multiply
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::FunctionFromFunction<TYPE> 
operator* ( const LoKi::Function<TYPE>&  fun  , 
            const double                 val  ) 
{ return fun * LoKi::Constant<TYPE>( val ) ; } ;
// ============================================================================

// ============================================================================
/** @fn operator*
 *  inline construction of operator "*" 
 *  @see LoKi::Multiply
 *  @see LoKi::Constant
 *  @return functor
 */
template <class TYPE>
inline   LoKi::FunctionFromFunction<TYPE> 
operator* ( const double                 val  ,
            const LoKi::Function<TYPE>&  fun  )
{ return fun * val ; } ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_OPERATORS_H
// ============================================================================
