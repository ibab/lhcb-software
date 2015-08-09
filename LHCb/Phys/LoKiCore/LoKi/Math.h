// $Id$
// ============================================================================
#ifndef LOKI_MATH_H 
#define LOKI_MATH_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
#include <algorithm>
// ============================================================================
// LokiCore
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Primitives.h"
#include "LoKi/BasicFunctors.h"
#include "LoKi/Power.h"
// ============================================================================
/** @file
 *
 *  Collection of math-functions for LoKi functions
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
  /*  just the exponent for LoKi 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  exp ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2> 
      ( std::exp , fun , "exp" ); }
  // ==========================================================================
  /*  sine for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  sin ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2> 
      ( std::sin , fun , "sin" ) ; }
  // ==========================================================================
  /*  hyperbolic sine for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE,class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  sinh ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2> 
      ( std::sinh, fun , "sinh" ) ; }
  // ==========================================================================
  /*  cosine for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE,class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  cos ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2>
      ( std::cos , fun , "cos" ) ; }
  // ==========================================================================  
  /*  hyperbolic cosine for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  cosh ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2>
      ( std::cosh , fun , "cosh" ) ; }
  // ==========================================================================  
  /*  tan for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE,class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  tan ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2>
      ( std::tan , fun , "tan" ) ; }
  // ==========================================================================
  /*  hyperbolic tangent for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  tanh ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2>
      ( std::tanh , fun , "tanh" ) ; }
  // ==========================================================================  
  /*  sqrt for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE,class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  sqrt ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2>
      ( std::sqrt , fun , "sqrt" ) ; }
  // ==========================================================================
  /*  cbrt for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE,class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  cbrt ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2>
      ( std::cbrt , fun , "cbrt" ) ; }
  // ==========================================================================
  /*  asine for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  asin ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2>
      ( std::asin , fun , "asin" ) ; }
  // ==========================================================================  
  /*  acos for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  acos ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2>
      ( std::acos , fun , "acos" ) ; }
  // ==========================================================================  
  /*  atan for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  atan ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2>
      ( std::atan , fun , "atan" ) ; }
  // ==========================================================================  
  /*  log for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE,class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  log ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2>
      ( std::log , fun , "log" ) ; }
  // ==========================================================================  
  /*  log10 for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE,class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  log10 ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2>
      ( std::log10 , fun , "log10" ) ; }
  // ==========================================================================  
  /*  abs for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  abs ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2>
      ( std::fabs , fun , "abs" ) ; }
  // ==========================================================================  
  /*  abs for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE,class TYPE2> 
  inline LoKi::ComposeFunction<TYPE,TYPE2>
  fabs ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
  { return LoKi::ComposeFunction<TYPE,TYPE2>
      ( std::fabs , fun , "abs" ) ; }
  // ==========================================================================  
  /*  atan2 for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction2<TYPE,TYPE2>
  atan2 ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
          const LoKi::Functor<TYPE,TYPE2>& fun2 ) 
  { return LoKi::ComposeFunction2<TYPE,TYPE2>
      ( std::atan2 , fun1 , fun2 , "atan2" ); }
  // ==========================================================================
  /*  atan2 for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction2<TYPE,TYPE2>
  atan2 ( const LoKi::Functor<TYPE,TYPE2>&       fun1 , 
          typename LoKi::Constant<TYPE,TYPE2>::T2 val2 )
  { return LoKi::ComposeFunction2<TYPE,TYPE2>
      ( std::atan2 , fun1 , val2 , "atan2" ) ; }
  // ==========================================================================  
  /*  atan2 for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction2<TYPE,TYPE2>
  atan2 ( typename LoKi::Constant<TYPE,TYPE2>::T2 val1 ,
          const LoKi::Functor<TYPE,TYPE2>&        fun2 ) 
  { return LoKi::ComposeFunction2<TYPE,TYPE2>
      ( std::atan2 , val1 , fun2  , "atan2" ) ; }
  // ==========================================================================  
  /*  atan  for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction2<TYPE,TYPE2>
  atan  ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
          const LoKi::Functor<TYPE,TYPE2>& fun2 ) 
  { return LoKi::ComposeFunction2<TYPE,TYPE2>
      ( std::atan2 , fun1 , fun2 , "atan2" ); }
  // ==========================================================================
  /*  atan  for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction2<TYPE,TYPE2>
  atan  ( const LoKi::Functor<TYPE,TYPE2>&       fun1 , 
          typename LoKi::Constant<TYPE,TYPE2>::T2 val2 )
  { return LoKi::ComposeFunction2<TYPE,TYPE2>
      ( std::atan2 , fun1 , val2 , "atan2" ) ; }
  // ==========================================================================  
  /*  atan  for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction2<TYPE,TYPE2>
  atan  ( typename LoKi::Constant<TYPE,TYPE2>::T2 val1 ,
          const LoKi::Functor<TYPE,TYPE2>&        fun2 ) 
  { return LoKi::ComposeFunction2<TYPE,TYPE2>
      ( std::atan2 , val1 , fun2  , "atan2" ) ; }
  // ==========================================================================  
  /*  pow for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction2<TYPE,TYPE2>
  pow ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
        const LoKi::Functor<TYPE,TYPE2>& fun2 ) 
  { return LoKi::ComposeFunction2<TYPE,TYPE2>
      ( std::pow , fun1 , fun2  , "pow" ) ; }
  // ==========================================================================  
  /*  pow for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction2<TYPE,TYPE2>
  pow ( const LoKi::Functor<TYPE,TYPE2>&             fun1 , 
        typename LoKi::Constant<TYPE,TYPE2>::T2 val2 )
  { return LoKi::ComposeFunction2<TYPE,TYPE2>
      ( std::pow , fun1 , val2 , "pow" ) ; }
  // ==========================================================================  
  /*  pow for LoKi function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */      
  template <class TYPE, class TYPE2> 
  inline LoKi::ComposeFunction2<TYPE,TYPE2>
  pow ( typename LoKi::Constant<TYPE,TYPE2>::T2 val1 ,
        const LoKi::Functor<TYPE,TYPE2>&        fun2 ) 
  { return LoKi::ComposeFunction2<TYPE,TYPE2>
      ( std::pow , val1 , fun2 , "pow" ) ; }
  // ==========================================================================  
  /*  minimum for LoKi functions 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2004-08-10 
   */
  template <class TYPE, class TYPE2>
  inline LoKi::Min<TYPE,TYPE2>
  min ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
        const LoKi::Functor<TYPE,TYPE2>& fun2 ) 
  { return LoKi::Min<TYPE,TYPE2>( fun1 , fun2 ) ; }
  // ==========================================================================  
  /*  minimum for LoKi functions 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2004-08-10 
   */
  template <class TYPE,class TYPE2>
  inline LoKi::Min<TYPE,TYPE2>
  min ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
        const LoKi::Functor<TYPE,TYPE2>& fun2 , 
        const LoKi::Functor<TYPE,TYPE2>& fun3 ) 
  { return LoKi::Min<TYPE,TYPE2>( fun1 , fun2 , fun3 ) ; }
  // ==========================================================================
  /*  minimum for LoKi functions 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2004-08-10 
   */
  template <class TYPE, class TYPE2>
  inline LoKi::Min<TYPE,TYPE2>
  min ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
        const LoKi::Functor<TYPE,TYPE2>& fun2 , 
        const LoKi::Functor<TYPE,TYPE2>& fun3 , 
        const LoKi::Functor<TYPE,TYPE2>& fun4 ) 
  { return LoKi::Min<TYPE,TYPE2>( fun1 , fun2 , fun3 , fun4 ) ; }
  // ==========================================================================
  /*  minimum for LoKi functions 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2004-08-10 
   */
  template <class TYPE, class TYPE2>
  inline LoKi::Min<TYPE,TYPE2>
  min ( const LoKi::Functor<TYPE,TYPE2>&        fun1 , 
        typename LoKi::Constant<TYPE,TYPE2>::T2 val2 ) 
  { return LoKi::Min<TYPE,TYPE2> ( fun1 , val2 ) ; }
  // ==========================================================================
  /*  minimum for LoKi functions 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2004-08-10 
   */
  template <class TYPE, class TYPE2>
  inline LoKi::Min<TYPE,TYPE2>
  min ( typename LoKi::Constant<TYPE,TYPE2>::T2 val1 , 
        const LoKi::Functor<TYPE,TYPE2>&        fun2 )  
  { return LoKi::Min<TYPE,TYPE2> ( val1 , fun2 ) ; }
  // ==========================================================================
  /*  maximum for LoKi functions 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2004-08-10 
   */
  template <class TYPE, class TYPE2>
  inline LoKi::Max<TYPE,TYPE2>
  max ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
        const LoKi::Functor<TYPE,TYPE2>& fun2 ) 
  { return LoKi::Max<TYPE,TYPE2>( fun1 , fun2 ) ; }
  // ==========================================================================  
  /*  maximum for LoKi functions 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2004-08-10 
   */
  template <class TYPE,class TYPE2>
  inline LoKi::Max<TYPE,TYPE2>
  max ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
        const LoKi::Functor<TYPE,TYPE2>& fun2 , 
        const LoKi::Functor<TYPE,TYPE2>& fun3 ) 
  { return LoKi::Max<TYPE,TYPE2>( fun1 , fun2 , fun3 ) ; }
  // ==========================================================================
  /*  maximum for LoKi functions 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2004-08-10 
   */
  template <class TYPE,class TYPE2>
  inline LoKi::Max<TYPE,TYPE2>
  max ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
        const LoKi::Functor<TYPE,TYPE2>& fun2 , 
        const LoKi::Functor<TYPE,TYPE2>& fun3 , 
        const LoKi::Functor<TYPE,TYPE2>& fun4 ) 
  { return LoKi::Max<TYPE,TYPE2>( fun1 , fun2 , fun3 , fun4 ) ; }
  // ==========================================================================      
  /*  maximum for LoKi functions 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2004-08-10 
   */
  template <class TYPE, class TYPE2>
  inline LoKi::Max<TYPE,TYPE2>
  max ( const LoKi::Functor<TYPE,TYPE2>&        fun1 , 
        typename LoKi::Constant<TYPE,TYPE2>::T2 val2 )
  { return LoKi::Max<TYPE,TYPE2>( fun1 , val2 ) ; }
  // ==========================================================================  
  /*  maximum for LoKi functions 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2004-08-10 
   */
  template <class TYPE, class TYPE2>
  inline LoKi::Max<TYPE,TYPE2>
  max ( typename LoKi::Constant<TYPE,TYPE2>::T2 val1 , 
        const LoKi::Functor<TYPE,TYPE2>&        fun2 )
  { return LoKi::Max<TYPE,TYPE2>( val1 , fun2 ) ; }
  // ==========================================================================
  /** helpful function for creation of comparison object 
   *  @param fun1 function to be applied to the first object to be compared 
   *  @param fun2 function to be applied to the second object to be compared 
   *  @param cmp comparison criteria for result of the function evaluation
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */
  template <class TYPE,class TYPE2,class CMP>
  inline LoKi::Compare<TYPE,CMP,TYPE2> compare
  ( const LoKi::Functor<TYPE,TYPE2>& fun1  , 
    const LoKi::Functor<TYPE,TYPE2>& fun2  ,
    const CMP&                  cmp = std::less<TYPE2>() )
  { return LoKi::Compare<TYPE,CMP,TYPE2> ( fun1 , fun2 , cmp ) ; }
  // ==========================================================================  
  /** select element form the sequence with minimal value of 
   *  given function.
   *  It is just an extension of the STL <tt>std::min_element</tt> 
   *  standard algorithm 
   *  @code 
   *  SEQUENCE prts = ... ; 
   *  SEQUENCE::iterator particle = 
   *              select_min( prts.begin() , prts.end() , PT ); 
   *  @endcode 
   *  Where <tt>SEQUENCE</tt> could be any container or container-like 
   *  object (like e.g. <tt>LoKi::Range</tt>) of <tt>const Particle*</tt>.
   *  This example illustrates the selection of particle with minimal 
   *  transverse momentum  from "container" <tt>prts</tt>.
   *  @see PT 
   *  @see LoKi::Function
   *  @param first  'begin' iterator of the sequence 
   *  @param last   'end'   iterator of the sequence    
   *  @param fun    function 
   *  @return iterator for the element with minimal value of function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-09-05
   */      
  template<class IT,class TYPE, class TYPE2>
  inline IT select_min 
  ( IT                               first , 
    IT                               last  , 
    const LoKi::Functor<TYPE,TYPE2>& fun   )
  {
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type results ;
    return std::min_element 
      ( first , 
        last  , 
        LoKi::compare ( fun , fun , std::less<results>() ) ) ;  
  }
  // ==========================================================================
  /** select element form the sequence with maximal value of function
   *  It is just an extension of the STL <tt>std::min_element</tt> 
   *  standard algorithm 
   *  @code 
   *  SEQUENCE prts = ... ; 
   *  SEQUENCE::iterator particle = 
   *              select_max( prts.begin() , prts.end() , CL ); 
   *  @endcode 
   *  Where <tt>SEQUENCE</tt> could be any container or container-like 
   *  object (like e.g. <tt>LoKi::Range</tt>) of <tt>const Particle*</tt>.
   *  This example illustrates the selection of particle with maximal 
   *  convidence level from "container" <tt>prts</tt>.
   *  @see CL
   *  @see LoKi::Function
   *  @param first  'begin' iterator of the sequence 
   *  @param last   'end'   iterator of the sequence    
   *  @param fun    function 
   *  @return iterator for the element with maximal value of function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-09-05
   */      
  template<class IT,class TYPE, class TYPE2>
  inline IT select_max
  ( IT first                             , 
    IT last                              , 
    const LoKi::Functor<TYPE,TYPE2>& fun )
  {
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type results ;
    return std::max_element 
      ( first , 
        last  , 
        LoKi::compare ( fun , fun , std::less<results>() ) ) ;  
  }
  // ==========================================================================
  /** select element form the sequence with minimal value of function
   *
   *  It is just an extension of the STL <tt>std::min_element</tt> 
   *  standard algorithm 
   *  @code 
   *  SEQUENCE prts = ... ; 
   *  const Particle* particle = select_min( prts , PT ); 
   *  @endcode 
   *  Where <tt>SEQUENCE</tt> could be any container or container-like 
   *  object (like e.g. <tt>LoKi::Range</tt>) of <tt>const Particle*</tt>.
   *  This example illustrates the selection of particle with minimal 
   *  transverse momentum  from "container" <tt>prts</tt>.
   *  @see PT 
   *  @see LoKi::Function
   *  @param sequence sequence/container/range 
   *  @param fun      function 
   *  @return element with minimal value of function  
   *  @attention be careful with empty ranges/containers/sequences 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-09-05
   */      
  template<class SEQUENCE , class TYPE, class TYPE2>
  inline typename SEQUENCE::value_type select_min
  ( SEQUENCE& sequence  , const LoKi::Functor<TYPE,TYPE2>& fun )
  {
    typedef typename SEQUENCE::value_type Type ;
    if ( sequence.begin() == sequence.end() ) { return Type (); }
    return *select_min( sequence.begin() , sequence.end() , fun );
  }
  // ==========================================================================  
  /** select element form the sequence with maximal value of function
   *
   *  It is just an extension of the STL <tt>std::min_element</tt> 
   *  standard algorithm 
   *  @code 
   *  SEQUENCE prts = ... ; 
   *  const Particle* particle = select_max( prts , CL ); 
   *  @endcode 
   *  Where <tt>SEQUENCE</tt> could be any container or container-like 
   *  object (like e.g. <tt>LoKi::Range</tt>) of <tt>const Particle*</tt>.
   *  This example illustrates the selection of particle with maximal 
   *  confidence level from "container" <tt>prts</tt>.
   *  @see CL
   *  @see LoKi::Function
   *  @param sequence sequence/container/range 
   *  @param fun      function 
   *  @return element with maximal value of function  
   *  @attention be careful with empty ranges/containers/sequences 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-09-05
   */      
  template<class SEQUENCE , class TYPE, class TYPE2>
  inline typename SEQUENCE::value_type select_max
  ( SEQUENCE& sequence  , const LoKi::Functor<TYPE,TYPE2>& fun )
  {
    typedef typename SEQUENCE::value_type Type ;
    if ( sequence.begin() == sequence.end() ) { return Type () ; }
    return *select_max( sequence.begin() , sequence.end() , fun );
  }
  // ==========================================================================  
  /** select element form the sequence with minimal value of 
   *  given function 
   *  It is just an extension of the STL <tt>std::min_element</tt> 
   *  standard algorithm with additional conditions 
   *  @code 
   *  SEQUENCE prts = ... ; 
   *  SEQUENCE::iterator particle = 
   *              select_min( prts.begin() , 
   *                          prts.end()   , 
   *                          PT           , 
   *                          P > 10 * GeV ); 
   *  @endcode 
   *  Where <tt>SEQUENCE</tt> could be any container or container-like 
   *  object (like e.g. <tt>LoKi::Range</tt>) of <tt>const Particle*</tt>.
   *  This example illustrates the selection of particle with minimal 
   *  transverse momentum  from "container" <tt>prts</tt>. Only 
   *  particles with momentum in excess of 10 GeV are considered.
   *
   *  @see PT 
   *  @see LoKi::Function
   *  @param first  'begin' iterator of the sequence 
   *  @param last   'end'   iterator of the sequence    
   *  @param fun    function 
   *  @param cut    predicate which need to be satisfied 
   *  @return iterator for the element with minimal value of function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2005-03-09
   */      
  template<class IT,class TYPE, class TYPE2>
  inline IT select_min
  ( IT first                         , 
    IT last                          , 
    const LoKi::Functor<TYPE,TYPE2>& fun , 
    const LoKi::Functor<TYPE,bool>&  cut )
  {
    // empty sequence 
    if ( first == last ) { return last ; }
    //
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type results ;
    typedef std::less<results>             LESS ;
    typedef LoKi::Compare<TYPE,LESS,TYPE2> CMP  ;
    const CMP cmp = LoKi::compare ( fun , fun , LESS() )  ;
    //
    IT result = last ;
    for (  ; first != last ; ++first ) 
    {
      if ( !cut( *first )           ) {                  continue ; }
      if ( last == result           ) { result = first ; continue ; }
      if ( cmp ( *first , *result ) ) { result = first ;            }  
    }
    //
    return result ;
  }
  // ==========================================================================
  /** select element form the sequence with maximal value of 
   *  given function 
   *
   *  It is just an extension of the STL <tt>std::max_element</tt> 
   *  standard algorithm with additional conditions 
   *
   *  @code 
   *  SEQUENCE prts = ... ; 
   *  SEQUENCE::iterator particle = 
   *              select_max( prts.begin() , 
   *                          prts.end()   , 
   *                          PT           , 
   *                          P > 10 * GeV ); 
   *  @endcode 
   *
   *  Where <tt>SEQUENCE</tt> could be any container or container-like 
   *  object (like e.g. <tt>LoKi::Range</tt>) of <tt>const Particle*</tt>.
   *  This example illustrates the selection of particle with maximal
   *  transverse momentum  from "container" <tt>prts</tt>. Only 
   *  particles with momentum in excess of 10 GeV are considered.
   *
   *  @see PT 
   *  @see LoKi::Function
   *  @param first  'begin' iterator of the sequence 
   *  @param last   'end'   iterator of the sequence    
   *  @param fun    function 
   *  @param cut    predicate which need to be satisfied 
   *  @return iterator for the element with minimal value of function
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2005-03-09
   */      
  template<class IT,class TYPE, class TYPE2>
  inline IT select_max
  ( IT first                             , 
    IT last                              , 
    const LoKi::Functor<TYPE,TYPE2>& fun , 
    const LoKi::Functor<TYPE,bool>&  cut )
  {
    // empty sequence 
    if ( first == last ) { return last ; }
    
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type results ;
    typedef std::less<results>        LESS ;
    typedef LoKi::Compare<TYPE,LESS> CMP  ;
    const CMP cmp = LoKi::compare ( fun , fun , LESS() )  ;
    //
    IT result = last ;
    for (  ; first != last ; ++first ) 
    {
      if ( !cut( *first )           ) {                  continue ; }
      if ( last == result           ) { result = first ; continue ; }
      if ( cmp ( *result , *first ) ) { result = first ;            }  
    } 
    return result ;
  }
  // ==========================================================================
  /** "in-range" function
   *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *   @date 2009-11-21
   */
  template <class TYPE>
  inline LoKi::InRange<TYPE>
  inRange ( const double                      low  ,
            const LoKi::Functor<TYPE,double>& fun  , 
            const double                      high )
  {
    return LoKi::InRange<TYPE> ( low , fun , high ) ;
  }
  // ========================================================================== 
  /** "in-range" function
   *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *   @date 2009-11-21
   */
  template <class TYPE>
  inline LoKi::InRange2<TYPE>
  inRange ( const LoKi::Functor<TYPE,double>& low  , 
            const LoKi::Functor<TYPE,double>& fun  , 
            const LoKi::Functor<TYPE,double>& high ) 
  {
    return LoKi::InRange2<TYPE> ( low , fun , high ) ;
  }
  // ========================================================================== 
  /** "in-range" function
   *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *   @date 2009-11-21
   */
  template <class TYPE>
  inline LoKi::InRange2<TYPE>
  inRange ( const double                      low  , 
            const LoKi::Functor<TYPE,double>& fun  , 
            const LoKi::Functor<TYPE,double>& high ) 
  {
    return LoKi::InRange2<TYPE> ( low , fun , high ) ;
  }
  // ========================================================================== 
  /** "in-range" function
   *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *   @date 2009-11-21
   */
  template <class TYPE>
  inline LoKi::InRange2<TYPE>
  inRange ( const LoKi::Functor<TYPE,double>& low  , 
            const LoKi::Functor<TYPE,double>& fun  , 
            const double                      high ) 
  {
    return LoKi::InRange2<TYPE> ( low , fun , high ) ;
  }
  // ========================================================================== 
  /** "in-range" function
   *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *   @date 2009-11-21
   */
  template <class TYPE>
  inline LoKi::InRange2<TYPE>
  in_range ( const LoKi::Functor<TYPE,double>& low  , 
             const LoKi::Functor<TYPE,double>& fun  , 
             const LoKi::Functor<TYPE,double>& high ) 
  {
    return inRange2 ( low , fun , high ) ;
  }
  // ========================================================================== 
  /** "in-range" function
   *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *   @date 2009-11-21
   */
  template <class TYPE>
  inline LoKi::InRange2<TYPE>
  in_range ( const double                      low  , 
             const LoKi::Functor<TYPE,double>& fun  , 
             const LoKi::Functor<TYPE,double>& high ) 
  {
    return inRange ( low , fun , high ) ;
  }
  // ========================================================================== 
  /** "in-range" function
   *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *   @date 2009-11-21
   */
  template <class TYPE>
  inline LoKi::InRange2<TYPE>
  in_range ( const LoKi::Functor<TYPE,double>& low  , 
             const LoKi::Functor<TYPE,double>& fun  , 
             const double                      high ) 
  {
    return inRange ( low , fun , high ) ;
  }
  // ==========================================================================
  /** "scale" predicate function 
   *   @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *   @date 2009-12-06
   */
  template <class TYPE>
  inline LoKi::XScaler<TYPE>
  scale ( const LoKi::Functor<TYPE, bool>& cut , 
          const LoKi::Functor<void,bool>&  sf  ) 
  { return LoKi::XScaler<TYPE>( cut , sf ) ;  }
  // ==========================================================================
  /** "round" the floating result to integer value 
   *   @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *   @date 2011-04-02
   */
  template <class TYPE>
  inline LoKi::Round<TYPE>
  round ( const LoKi::Functor<TYPE, double>& fun ) 
  { return LoKi::Round<TYPE>( fun , 1.0 ) ;  }
  // ==========================================================================
  /** "jbit" the floating result to integer value 
   *  get the jth bit of value.
   *  The action :  
   *   - 1. f -> round ( f ) 
   *   - 2. f -> abs   ( f ) 
   *   - 3. Gaudi::Math::bit ( f , j )  
   *  @see LoKi::JBit
   *  @see Gaudi::Math::bit
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-04-02
   */
  template <class TYPE>
  inline LoKi::JBit<TYPE>
  jbit  ( const LoKi::Functor<TYPE, double>& fun , 
          const unsigned int                 j   ) 
  { return LoKi::JBit<TYPE>( fun , j ) ;  }
  // ==========================================================================
  /** "jbits" the floating result to integer value 
   *  get the (j1-j2) bits of value.
   *  The action :  
   *   - 1. f -> round ( f ) 
   *   - 2. f -> abs   ( f ) 
   *   - 3. Gaudi::Math::bits ( f , j1 , j2  )  
   *  @see LoKi::JBits
   *  @see Gaudi::Math::bits
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-04-02
   */
  template <class TYPE>
  inline LoKi::JBits<TYPE>
  jbits ( const LoKi::Functor<TYPE, double>& fun , 
          const unsigned int                 j1  ,
          const unsigned int                 j2  ) 
  { return LoKi::JBits<TYPE>( fun , j1 , j2 ) ;  }
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Math
  {
    // ========================================================================
    template <class TYPE1, class TYPE2>
    struct _Abs_: public std::unary_function<TYPE1,TYPE2>
    {
      inline TYPE2 operator() ( TYPE1 i ) const { return std::abs ( i ) ; }
    } ;
    // ========================================================================
    template <class TYPE1, class TYPE2> 
    std::vector<TYPE2> _abs_ ( const  std::vector<TYPE1>& vct ) 
    {
      std::vector<TYPE2> res ( vct.size() ) ;
      std::transform ( vct.begin() , 
                       vct.end  () , 
                       res.begin() , _Abs_<TYPE1,TYPE2>() ) ;
      return res ;
    }
    // ========================================================================
  } //                                              end of namespace LoKi::Math  
  // ==========================================================================
} // end of namespace Loki 
// ============================================================================
namespace std 
{
  //
  // 
  inline 
  std::vector<unsigned int>   abs ( const std::vector<int>&   vct ) 
  { return LoKi::Math::_abs_<int ,unsigned int>( vct ) ; }
  inline 
  std::vector<unsigned long>  abs ( const std::vector<long>&  vct ) 
  { return LoKi::Math::_abs_<long,unsigned long>( vct ) ; }
  inline 
  std::vector<unsigned short> abs ( const std::vector<short>& vct ) 
  { return LoKi::Math::_abs_<short,unsigned short>( vct ) ; }
  inline 
  std::vector<float>          abs ( const std::vector<float>&  vct ) 
  { return LoKi::Math::_abs_<float,float>       ( vct ) ; }
  inline 
  std::vector<double>         abs ( const std::vector<double>& vct ) 
  { return LoKi::Math::_abs_<double,double>     ( vct ) ; }
  //
  // just for completenes 
  //
  inline 
  const std::vector<unsigned int>& 
  abs ( const std::vector<unsigned int>&   vct ) { return vct ; }
  inline 
  const std::vector<unsigned short>& 
  abs ( const std::vector<unsigned short>& vct ) { return vct ; }
  inline 
  const std::vector<unsigned long>& 
  abs ( const std::vector<unsigned long>&  vct ) { return vct ; }
  // ==========================================================================
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MATH_H
// ============================================================================
