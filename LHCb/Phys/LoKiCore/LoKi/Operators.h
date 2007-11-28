// $Id: Operators.h,v 1.8 2007-11-28 13:56:33 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_OPERATORS_H 
#define LOKI_OPERATORS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Primitives.h"
#include "LoKi/apply.h"
#include "LoKi/Cast.h"
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
/** inline construction of logical 'AND'
 *
 *  @code 
 *
 *  typedef LoKi::Functor<SomeType,bool>             Pred ;
 *  typedef LoKi::FunctorFromFunctor<SomeType,bool>  PfP  ;
 *
 *  void func( const Pred& A , const Pred& B )
 *  {
 *    PfP p1 = A && B             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::And
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
template <class TYPE,class TYPE2>
inline   LoKi::And<TYPE,TYPE2> 
operator&&  ( const  LoKi::Functor<TYPE,TYPE2>& fun1 , 
              const  LoKi::Functor<TYPE,TYPE2>& fun2 ) 
{ return LoKi::And<TYPE,TYPE2>  ( fun1   , fun2   ) ; } 
// ============================================================================
/** inline construction of logical 'OR' 
 *
 *  @code 
 *
 *  typedef LoKi::Functor<SomeType,bool>             Pred ;
 *  typedef LoKi::FunctorFromFunctor<SomeType,bool>  PfP  ;
 *
 *  void func( const Pred& A , const Pred& B )
 *  {
 *    PfP p1 = A || B             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::Or
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
template <class TYPE, class TYPE2>
inline   LoKi::Or<TYPE,TYPE2> 
operator|| ( const  LoKi::Functor<TYPE,TYPE2>& fun1 , 
             const  LoKi::Functor<TYPE,TYPE2>& fun2 ) 
{ return LoKi::Or<TYPE,TYPE2> ( fun1   , fun2 ) ; } 
// ============================================================================
/** inline construction for logical negate 
 *
 *  @code 
 *
 *  typedef LoKi::Functor<SomeType,bool>             Pred ;
 *  typedef LoKi::FunctorFromFunctor<SomeType,bool>  PfP  ;
 *
 *  void func( const Pred& A  )
 *  {
 *    PfP p1 = !A            ; // operator form
 *  }
 *
 *  @endcode 
 *
 *
 *  @see std::logical_not 
 *  @see LoKi::Not
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline LoKi::Not<TYPE,TYPE2> 
operator!  ( const  LoKi::Functor<TYPE,TYPE2>& fun  ) 
{ return LoKi::Not<TYPE,TYPE2> ( fun )  ; } 
// ============================================================================
/** inline construction for unary negate 
 *
 *  @code 
 *
 *  typedef LoKi::Functor<SomeType,double>             Func;
 *  typedef LoKi::FunctorFromFunctor<SomeType,double>  FfF  ;
 *
 *  void func( const Func& A  )
 *  {
 *    FfF p1 = -A             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *
 *  @see std::negate
 *  @see LoKi::Negate
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline LoKi::Negate<TYPE,TYPE2> 
operator-( const  LoKi::Functor<TYPE,TYPE2>& fun  ) 
{ return LoKi::Negate<TYPE,TYPE2> ( fun )  ; } 
// ============================================================================
/** inline construction of operator "<" 
 *
 *  @code 
 *
 *  typedef LoKi::Functor<SomeType,double>          Func ;
 *  typedef LoKi::FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    PfP f = A < B             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::Less
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::Less<TYPE,TYPE2> 
operator<  ( const  LoKi::Functor<TYPE,TYPE2>&  fun1 , 
             const  LoKi::Functor<TYPE,TYPE2>&  fun2 ) 
{ return LoKi::Less<TYPE,TYPE2> ( fun1   , fun2   ) ; } ;
// ============================================================================
/** inline construction of operator "<" 
 *
 *  @code 
 *
 *  typedef LoKi::Functor<SomeType,double>          Func ;
 *  typedef LoKi::FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const double B  )
 *  {
 *    PfP f = A < B             ; // operator form
 *  }
 *    
 *  @endcode 
 *
 *  @see LoKi::Less
 *  @see LoKi::Constant
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE,class TYPE2>
inline   LoKi::Less<TYPE,TYPE2> 
operator< ( const  LoKi::Functor<TYPE,TYPE2>&       fun , 
            typename LoKi::Constant<TYPE,TYPE2>::T2 val ) 
{ return LoKi::Less<TYPE,TYPE2>( fun , val ) ; }
// ============================================================================
/** inline construction for operator "<" 
 *
 *  @code 
 *
 *  typedef LoKi::Functor<SomeType,double>          Func ;
 *  typedef LoKi::FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const double B  )
 *  {
 *    PfP f = B < A             ; // operator form
 *  }
 *    
 *  @endcode 
 *
 *  @see LoKi::Less
 *  @see LoKi::Constant
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::Less<TYPE,TYPE2> 
operator< ( typename LoKi::Constant<TYPE,TYPE2>::T2 val , 
            const  LoKi::Functor<TYPE,TYPE2>&       fun ) 
{ return LoKi::Less<TYPE,TYPE2>( val , fun ) ; }
// ============================================================================
/** inline construction for operator ">" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFrinFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    PfP f = A > B             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::Less
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::Less<TYPE, TYPE2> 
operator>  ( const  LoKi::Functor<TYPE,TYPE2>&  fun1 , 
             const  LoKi::Functor<TYPE,TYPE2>&  fun2 ) 
{ return LoKi::Less<TYPE,TYPE2> ( fun2 , fun1 ) ; }
// ============================================================================
/** inline construction for operator ">" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFrinFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const double B )
 *  {
 *    PfP f = A > B             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::Less
 *  @see LoKi::Constant
 *  @return functor
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::Less<TYPE,TYPE2> 
operator>  ( const  LoKi::Functor<TYPE,TYPE2>&       fun  , 
             typename LoKi::Constant<TYPE,TYPE2>::T2 val ) 
{ return LoKi::Less<TYPE,TYPE2>( val , fun ) ; }
// ============================================================================
/** inline construction of operator ">" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFrinFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const double B )
 *  {
 *    PfP f = B > A             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::Less
 *  @see LoKi::Constant
 *  @return functor
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::Less<TYPE,TYPE2> 
operator> ( typename LoKi::Constant<TYPE,TYPE2>::T2 val , 
            const  LoKi::Functor<TYPE,TYPE2>&       fun  ) 
{ return LoKi::Less<TYPE,TYPE2>( fun , val ) ; }
// ============================================================================
/** inline construction of operator "==" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    PfP f = A == B             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::Equal
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::Equal<TYPE,TYPE2> 
operator== ( const LoKi::Functor<TYPE,TYPE2>&  fun1 , 
             const LoKi::Functor<TYPE,TYPE2>&  fun2 ) 
{ return LoKi::Equal<TYPE,TYPE2>  ( fun1   , fun2   ) ; }
// ============================================================================
/** inline construction of operator "==" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const double B )
 *  {
 *    PfP f = A == B             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::EqualToValue
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::EqualToValue<TYPE,TYPE2> 
operator== ( const LoKi::Functor<TYPE,TYPE2>&        fun  , 
             typename LoKi::Constant<TYPE,TYPE2>::T2 val  ) 
{ return LoKi::EqualToValue<TYPE,TYPE2> ( fun   , val  ) ; }
// ============================================================================
/** inline construction of operator "==" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const double B )
 *  {
 *    PfP f = B == A             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::EqualToValue
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::EqualToValue<TYPE,TYPE2> 
operator== ( typename LoKi::Constant<TYPE,TYPE2>::T2 val  ,
             const LoKi::Functor<TYPE,TYPE2>&        fun  ) 
{ return LoKi::EqualToValue<TYPE,TYPE2> ( val , fun  ) ; }
// ============================================================================
/** inline construction of operator "<=" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    PfP f = A <= B             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::LessOrEqual
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::LessOrEqual<TYPE,TYPE2> 
operator<= ( const  LoKi::Functor<TYPE,TYPE2>&  fun1 , 
             const  LoKi::Functor<TYPE,TYPE2>&  fun2 ) 
{ return LoKi::LessOrEqual<TYPE,TYPE2> ( fun1 , fun2 ) ; } 
// ============================================================================
/** inline construction of operator "<=" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const double  B )
 *  {
 *    PfP f = A <= B             ; // operator form
 *  }
 *
 *  @endcode 
 * 
 *  @see LoKi::LessOrEqual
 *  @see LoKi::Constant
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::LessOrEqual<TYPE,TYPE2> 
operator<= ( const  LoKi::Functor<TYPE,TYPE2>&       fun ,
             typename LoKi::Constant<TYPE,TYPE2>::T2 val ) 
{ return LoKi::LessOrEqual<TYPE,TYPE2>( fun , val ) ; }
// ============================================================================
/** inline construction of operator "<=" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const double  B )
 *  {
 *    PfP f = B <= A             ; // operator form
 *  }
 *
 *  @endcode 
 * 
 *  @see LoKi::LessOrEqual
 *  @see LoKi::Constant
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE,class TYPE2>
inline   LoKi::LessOrEqual<TYPE,TYPE2>
operator<= ( typename LoKi::Constant<TYPE,TYPE2>::T2 val ,
             const  LoKi::Functor<TYPE,TYPE2>&       fun )
{ return LoKi::LessOrEqual<TYPE,TYPE2>( val , fun ) ; }
// ============================================================================
/** inline construction of operator ">=" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const Func&  B )
 *  {
 *    PfP f = A >= B             ; // operator form
 *  }
 *
 *  @endcode 
 * 
 *  @see LoKi::LessOrEqual
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::LessOrEqual<TYPE,TYPE2> 
operator>= ( const  LoKi::Functor<TYPE,TYPE2>& fun1 , 
             const  LoKi::Functor<TYPE,TYPE2>& fun2 ) 
{ return LoKi::LessOrEqual<TYPE,TYPE2> ( fun2 , fun1 ) ; }
// ============================================================================
/** inline construction of operator ">=" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const double  B )
 *  {
 *    PfP f = A >= B             ; // operator form
 *  }
 *
 *  @endcode 
 * 
 *  @see LoKi::LessOrEqual
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::LessOrEqual<TYPE,TYPE2> 
operator>= ( const  LoKi::Functor<TYPE,TYPE2>&       fun , 
             typename LoKi::Constant<TYPE,TYPE2>::T2 val ) 
{ return LoKi::LessOrEqual<TYPE,TYPE2> ( val , fun ) ; }
// ============================================================================
/** inline construction of operator ">=" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const double  B )
 *  {
 *    PfP f = B >= A             ; // operator form
 *  }
 *
 *  @endcode 
 * 
 *  @see LoKi::LessOrEqual
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::LessOrEqual<TYPE,TYPE2> 
operator>= ( typename LoKi::Constant<TYPE,TYPE2>::T2 val , 
             const  LoKi::Functor<TYPE,TYPE2>&       fun )
{ return LoKi::LessOrEqual<TYPE,TYPE2> ( fun , val ) ; }
// ============================================================================
/** inline construction of operator "!=" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    PfP f = A != B             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::NotEqual
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline LoKi::NotEqual<TYPE,TYPE2> 
operator!= ( const LoKi::Functor<TYPE,TYPE2>&  fun1 , 
             const LoKi::Functor<TYPE,TYPE2>&  fun2 ) 
{ return LoKi::NotEqual<TYPE,TYPE2> ( fun1 , fun2 ) ; } 
// ============================================================================
/** inline construction of operator "!=" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const double B )
 *  {
 *    PfP f = A != B             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::NotEqual
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline LoKi::NotEqualToValue<TYPE,TYPE2> 
operator!= ( const LoKi::Functor<TYPE,TYPE2>&        fun , 
             typename LoKi::Constant<TYPE,TYPE2>::T2 val )
{ return LoKi::NotEqualToValue<TYPE,TYPE2> ( fun , val ) ; }
// ============================================================================
/** inline construction of operator "!=" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>          Func ;
 *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
 *
 *  void func( const Func& A , const double B )
 *  {
 *    PfP f = B != A             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::NotEqual
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline LoKi::NotEqualToValue<TYPE,TYPE2> 
operator!= ( typename LoKi::Constant<TYPE,TYPE2>::T2 val ,
             const LoKi::Functor<TYPE,TYPE2>&        fun )
{ return LoKi::NotEqualToValue<TYPE,TYPE2> ( val , fun ) ; }
// ============================================================================
/** inline construction of operator "+" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>            Func ;
 *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    FfF f = A + B             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::Plus
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::Plus<TYPE,TYPE2> 
operator+ ( const LoKi::Functor<TYPE,TYPE2>&  fun1 , 
            const LoKi::Functor<TYPE,TYPE2>&  fun2 ) 
{ return LoKi::Plus<TYPE,TYPE2> ( fun1  , fun2 ) ; } 
// ============================================================================
/** inline construction of operator "+" 
 *
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>            Func ;
 *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
 *
 *  void func( const Func& A , const double B )
 *  {
 *    FfF f = A + B             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::Plus
 *  @see LoKi::Constant
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::Plus<TYPE,TYPE2> 
operator+ ( const LoKi::Functor<TYPE,TYPE2>&        fun , 
            typename LoKi::Constant<TYPE,TYPE2>::T2 val ) 
{ return LoKi::Plus<TYPE,TYPE2>( fun , val ) ; }
// ============================================================================
/** inline construction of operator "+" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>            Func ;
 *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
 *
 *  void func( const Func& A , const double B )
 *  {
 *    FfF f = B + A            ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::Plus
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::Plus<TYPE,TYPE2> 
operator+ ( typename LoKi::Constant<TYPE,TYPE2>::T2 val ,
            const LoKi::Functor<TYPE,TYPE2>&        fun )
{ return LoKi::Plus<TYPE,TYPE2>( val , fun ) ; }
// ============================================================================
/** inline construction of operator "-" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>            Func ;
 *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
 *
 *  void func( const Func& A , const Func& B )
 *  {
 *    FfF f = A - B             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::Minus
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE,class TYPE2>
inline   LoKi::Minus<TYPE,TYPE2> 
operator- ( const LoKi::Functor<TYPE,TYPE2>&  fun1 , 
            const LoKi::Functor<TYPE,TYPE2>&  fun2 ) 
{ return LoKi::Minus<TYPE,TYPE2> ( fun1  , fun2 ) ; } 
// ============================================================================
/** inline construction of operator "-" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>            Func ;
 *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
 *
 *  void func ( const Func& A , const double B )
 *  {
 *    FfF f = A - B             ; // operator form
 *  }
 *
 *  @endcode 
 *
 *  @see LoKi::Minus
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::Minus<TYPE,TYPE2> 
operator- ( const LoKi::Functor<TYPE,TYPE2>&        fun , 
            typename LoKi::Constant<TYPE,TYPE2>::T2 val )
{ return LoKi::Minus<TYPE,TYPE2> ( fun  , val ) ; } 
// ============================================================================
/** inline construction of operator "-" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>            Func ;
 *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
 *
 *  void func ( const Func& A , const double B )
 *  {
 *    FfF f = B - A             ; // operator form
 *  }
 *
 *  @endcode  
 *
 *  @see LoKi::Minus
 *  @see LoKi::Constant
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::Minus<TYPE,TYPE2> 
operator- ( typename LoKi::Constant<TYPE,TYPE2>::T2 val ,
            const LoKi::Functor<TYPE,TYPE2>&        fun )
{ return LoKi::Minus<TYPE,TYPE2> ( val , fun ) ; } 
// ============================================================================
/** inline construction of operator "/" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>            Func ;
 *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
 *
 *  void func ( const Func& A , const Func& B )
 *  {
 *    FfF f = A / B             ; // operator form
 *  }
 *
 *  @endcode  
 *
 *  @see LoKi::Divide
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::Divide<TYPE,TYPE2> 
operator/ ( const LoKi::Functor<TYPE,TYPE2>&  fun1 , 
            const LoKi::Functor<TYPE,TYPE2>&  fun2 ) 
{ return LoKi::Divide<TYPE,TYPE2>  ( fun1  , fun2 ) ; } 
// ============================================================================
/** inline construction of operator "/" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>            Func ;
 *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
 *
 *  void func ( const Func& A , const double B )
 *  {
 *    FfF f = A / B              ; // operator form
 *  }
 *
 *  @endcode  
 *
 *  @see LoKi::Divide
 *  @see LoKi::Constant
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE,class TYPE2>
inline   LoKi::Divide<TYPE,TYPE2> 
operator/ ( const LoKi::Functor<TYPE,TYPE2>&        fun , 
            typename LoKi::Constant<TYPE,TYPE2>::T2 val )
{ return LoKi::Divide<TYPE,TYPE2>  ( fun  , val ) ; } 
// ============================================================================
/** inline construction of operator "/" 
 *
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>            Func ;
 *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
 *
 *  void func ( const Func& A , const double B )
 *  {
 *    FfF f = B / A             ; // operator form
 *  }
 *
 *  @endcode  
 *
 *  @see LoKi::Divide
 *  @see LoKi::Constant
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE,class TYPE2>
inline   LoKi::Divide<TYPE,TYPE2> 
operator/ ( typename LoKi::Constant<TYPE,TYPE2>::T2 val ,
            const LoKi::Functor<TYPE,TYPE2>&        fun )
{ return LoKi::Divide<TYPE,TYPE2>  ( val , fun ) ; }
// ============================================================================
/** inline construction of operator "*" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>            Func ;
 *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
 *
 *  void func ( const Func& A , const Func& B  )
 *  {
 *    FfF f = A * B             ; // operator form
 *  }
 *
 *  @endcode  
 *
 *  @see LoKi::Multiply
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE, class TYPE2>
inline   LoKi::Multiply<TYPE,TYPE2> 
operator* ( const LoKi::Functor<TYPE,TYPE2>&  fun1 , 
            const LoKi::Functor<TYPE,TYPE2>&  fun2 ) 
{ return LoKi::Multiply<TYPE,TYPE2>  ( fun1  , fun2 ) ; } 
// ============================================================================
/** inline construction of operator "*" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>            Func ;
 *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
 *
 *  void func ( const Func& A , const double B )
 *  {
 *    FfF f = A * B             ; // operator form
 *  }
 *
 *  @endcode  
 *
 *  @see LoKi::Multiply
 *  @see LoKi::Constant
 *  @return functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE,class TYPE2>
inline   LoKi::Multiply<TYPE,TYPE2> 
operator* ( const LoKi::Functor<TYPE,TYPE2>&        fun , 
            typename LoKi::Constant<TYPE,TYPE2>::T2 val )
{ return LoKi::Multiply<TYPE,TYPE2> ( fun , val ) ; }
// ============================================================================
/** inline construction of operator "*" 
 *
 *  @code 
 *
 *  typedef Functor<SomeType,double>            Func ;
 *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
 *
 *  void func ( const Func& A , const double B )
 *  {
 *    FfF f = B * A             ; // operator form
 *  }
 *
 *  @endcode  
 *
 *  @see LoKi::Multiply
 *  @see LoKi::Constant
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE,class TYPE2>
inline   LoKi::Multiply<TYPE,TYPE2> 
operator* ( typename LoKi::Constant<TYPE,TYPE2>::T2 val ,
            const LoKi::Functor<TYPE,TYPE2>&        fun )
{ return LoKi::Multiply<TYPE,TYPE2> ( val , fun ) ; }
// ============================================================================
/** compose/chain the function 
 *
 *  @code 
 *
 *  typedef Functor<Type1,Type2>             Func1 ;
 *  typedef Functor<Type2,Type3>             Func2 ;
 *  typedef FunctorFromFunctor<Type1,Type3>  Func3 ;
 *
 *  void func ( const Func1& A , const Func2& B  )
 *  {
 *    Func3  f = A >> B  ; // operator form
 *  }
 *
 *  @endcode  
 *
 *  @see LoKi::Compose
 *
 *  @param fun1 the first functor to be chained 
 *  @param fun2 the second functor to be chained 
 *  @return the chained functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE,class TYPE1,class TYPE2>
inline LoKi::Compose<TYPE,TYPE1,TYPE2>
operator >> ( const LoKi::Functor<TYPE,TYPE1>& fun1 ,
              const LoKi::Functor<TYPE1,TYPE2>& fun2 ) 
{ return LoKi::Compose<TYPE,TYPE1,TYPE2> ( fun1 , fun2 ) ; }
// ============================================================================
/** the streamer for two predicates is "logical AND" for the predicates 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23   
 */    
template <class TYPE> 
inline LoKi::And<TYPE,bool>
operator >> 
( const LoKi::Functor<TYPE,bool>& cut1 , 
  const LoKi::Functor<TYPE,bool>& cut2 ) 
{ return LoKi::And<TYPE> ( cut1 , cut2 ) ; }
// ============================================================================
/** evaluate the function 
 *  
 *  @code 
 *
 *  typedef Functor<Type1,Type2>             Func ;
 *
 *  const Func&  fun   = ... ;
 *  const Type1& input = ... ;
 *
 *  Type2 output = input >> fun ;
 *
 *  @endcode  
 *
 *  @see LoKi::apply 
 *
 *  @param a argument 
 *  @param o the functor 
 *  @return the result of the functor evaluation 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23   
 */
template <class TYPE,class TYPE2>
inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
operator >> ( typename LoKi::Functor<TYPE,TYPE2>::argument a ,
              const LoKi::Functor<TYPE,TYPE2>&             o )
{ return LoKi::apply ( o , a ) ; }
// ============================================================================
/** evaluate the vector function 
 *
 *  @code 
 *
 *  typedef Functor<Type1,Type2>             Func ;
 *
 *  const std::vector<Type1> input = ...
 *
 *  std::vector<Type2> output = input >> fun ;
 *
 *  @endcode  
 *
 *  @see LoKi::apply 
 *
 *  @param input vector in input arguments 
 *  @param funct the functor
 *  @return the vector of results 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23   
 */  
template <class TYPEI, class TYPE,class TYPE2>
inline std::vector<typename LoKi::Functor<TYPE,TYPE2>::result_type>
operator >> ( const std::vector<TYPEI>&        input ,  
              const LoKi::Functor<TYPE,TYPE2>& funct )
{
  typedef std::vector<typename LoKi::Functor<TYPE,TYPE2>::result_type> OUTPUT ;
  OUTPUT out ;
  out.reserve ( input.size() ) ;
  //
  LoKi::apply 
    ( input.begin () , input.end () , funct , std::back_inserter ( out ) ) ;
  // 
  return out ;
}
/** evaluate the vector function 
 *
 *  @code 
 *
 *  typedef Functor<Type1,Type2>             Func ;
 *
 *  const std::vector<Type1> input = ...
 *
 *  std::vector<Type2> output = input >> fun ;
 *
 *  @endcode  
 *
 *  @see LoKi::apply 
 *
 *  @param input vector in input arguments 
 *  @param funct the functor
 *  @return the vector of results 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23   
 */  
template <class TYPEI, class TYPE,class TYPE2>
inline typename LoKi::Functor<std::vector<TYPE>,TYPE2>::result_type
operator >> ( const std::vector<TYPEI>&                     input ,  
              const LoKi::Functor<std::vector<TYPE>,TYPE2>& funct )
{
  typedef std::vector<TYPE> INPUT ;
  return LoKi::apply ( funct , INPUT( input.begin() , input.end() ) ) ;
}
// ============================================================================
/** evaluate/filter the vector function/predicate 
 *
 *  @code 
 *
 *  typedef Functor<Type1,bool>            Func ;
 *
 *  const std::vector<Type1> input = ...
 *
 *  std::vector<Type1> output = input >> fun ;
 *
 *  @endcode  
 *
 *  @see LoKi::apply 
 *
 *  @param input vector in input arguments 
 *  @param pred the predicate to be evaluated 
 *  @return the filtered inputs 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23   
 */  
template <class TYPEI, class TYPE>
inline std::vector<TYPEI>
operator >> 
( const std::vector<TYPEI>&       input ,  
  const LoKi::Functor<TYPE,bool>& pred  )
{
  typedef std::vector<TYPEI> OUTPUT ;
  //
  OUTPUT out ;
  out.reserve ( input.size() ) ;
  //
  LoKi::apply_filter 
    ( input.begin() , input.end() , pred , std::back_inserter ( out ) ) ;
  // 
  return out ;
}
// ============================================================================
/** "source"
 *
 *   @code
 *  
 *    const LoKi::Functor<void, std::vector<TYPE> > source ;
 *
 *    std::vector<TYPE> dest ;
 *
 *    source >> dest ;
 *
 *   @endcode
 *
 *  The concept belongs to the Gerhard "The Great" Raven.
 *
 *   @param  source the source 
 *   @param  dest   the destination 
 *   @return the destination 
 *
 *   @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *   @date 2001-01-23   
 */  
template <class TYPE>
inline std::vector<TYPE> 
operator >> 
( const LoKi::Functor<void,std::vector<TYPE> >& source , 
  std::vector<TYPE>&                            dest   ) 
{
  dest = source.evaluate() ;
  return dest ;
}
// ============================================================================
/** "source"
 *
 *   @code
 *  
 *    const LoKi::Functor<void, std::vector<TYPE*> > source ;
 *
 *    std::vector<const TYPE*> dest ;
 *
 *    source >> dest ;
 *
 *   @endcode
 *
 *  The concept belongs to the Gerhard "The Great" Raven.
 *
 *   @param  source the source 
 *   @param  dest   the destination 
 *   @return the destination 
 *
 *   @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *   @date 2001-01-23   
 */  
template <class TYPE>
inline std::vector<const TYPE*> 
operator >> 
( const LoKi::Functor<void,std::vector<TYPE*> >& source , 
  std::vector<const TYPE*>&                      dest   ) 
{
  std::vector<TYPE*>& aux = source.evaluate() ;
  dest = std::vector<const TYPE*>( aux.begin() , aux.end() ) ;
  return dest ;  
}
// ============================================================================
/** "source"
 *
 *   @code
 *  
 *    const LoKi::Functor<void, std::vector<const TYPE*> > source ;
 *
 *    std::vector<TYPE*> dest ;
 *
 *    source >> dest ;
 *
 *   @endcode
 *
 *  The concept belongs to the Gerhard "The Great" Raven.
 *
 *   @param  source the source 
 *   @param  dest   the destination 
 *   @return the destination 
 *
 *   @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *   @date 2001-01-23   
 */  
template <class TYPE>
inline std::vector<TYPE*> 
operator >> 
( const LoKi::Functor<void,std::vector<const TYPE*> >& source , 
  std::vector<TYPE*>&                                  dest   ) 
{
  /// evaluate the functor 
  std::vector<const TYPE*>& aux = source.evaluate() ;
  dest.erase() ;
  dest.reserve( aux.size() ) ;
  std::transform 
    ( aux.begin ()                  , 
      aux.end   ()                  , 
      std::back_inserter ( dest )   , 
      LoKi::Cast::ConstAway<TYPE>() ) ;
  /// 
  return dest ;                   
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_OPERATORS_H
// ============================================================================
