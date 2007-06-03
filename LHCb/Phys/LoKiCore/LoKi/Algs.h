// $Id: Algs.h,v 1.8 2007-06-03 20:38:24 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.8 $
// ============================================================================
// $Log: not supported by cvs2svn $
//
// ============================================================================
#ifndef LOKI_ALGS_H 
#define LOKI_ALGS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
#include <functional>
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
 *  @date 2006-02-10
 */
// ============================================================================
namespace LoKi
{  
  /** @namespace  LoKi::Algs Algs.h LoKi/Algs.h
   *
   *  Helper namespace with auxillary 
   *  functions/algorithms of the general interest 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace  Algs 
  {
    // ========================================================================
    /** The trivial algorithm which accumulated the value 
     *  of the function over the sequence
     * 
     *  @param first    'begin'-iterator for the sequence
     *  @param last      'end'-iterator for the sequence
     *  @param function  function to be evaluated 
     *  @param result    the initial value 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-10
     */
    template <class OBJECT,class RESULT,class FUNCTOR,class OPERATION>
    inline RESULT accumulate
    ( OBJECT         first     ,
      OBJECT         last      , 
      const FUNCTOR& functor   ,
      RESULT         result    , 
      OPERATION      binop     )
    {
      for ( ; first != last ; ++first ) 
      { result = binop ( result , functor( *first ) ) ; }
      return result ;
    } 
    // ========================================================================
    /** The trivial algorithm which accumulates the value 
     *  of the function over the sequence for elements which 
     *  satisfy certain criteria 
     * 
     *  @param first    'begin'-iterator for the sequence
     *  @param last      'end'-iterator for the sequence
     *  @param function  function to be evaluated 
     *  @param predicate the seelection criteria
     *  @param result    the initial value 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-10
     */
    template <class OBJECT,class RESULT,class FUNCTOR,
              class PREDICATE,class OPERATION>
    inline RESULT accumulate
    ( OBJECT           first     ,
      OBJECT           last      , 
      const FUNCTOR&   functor   ,
      const PREDICATE& predicate ,
      RESULT           result    , 
      OPERATION        binop     )
    {
      for ( ; first != last ; ++first ) 
      { if ( predicate( *first ) ) 
      { result = binop ( result , functor( *first ) ) ; } }
      return result ;
    } 
    /** The missing "copy_if" algorithm from STL 
     *
     *  @param first 'begin'-iterator for the input sequence of objects
     *  @param last  'end'-iterator for the input sequence of objects
     *  @param output the position fo output iterator 
     *  @param cut    the condition
     *  @return the updated position of output itrator 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-20
     */
    template <class OBJECT,class OUTPUT,class PREDICATE>
    inline OUTPUT copy_if 
    ( OBJECT    first  , 
      OBJECT    last   , 
      OUTPUT    output , 
      PREDICATE cut    ) 
    {
      for ( ; first != last ; ++first ) 
      { if ( cut( *first ) ) { *output = *first ; ++output ; } }
      return output ;
    }
    /// a bit modified version of std::count_if 
    template <class OBJECT, class PREDICATE>
    inline std::size_t count_if 
    ( OBJECT            first , 
      OBJECT            last  ,
      const PREDICATE&  cuts  )
    {
      std::size_t result = 0 ;
      for ( ; first != last ; ++first ) 
      { if ( cuts ( *first ) ) { ++result ; } }
      return result ;
    }    
    /// a bit modified version of std::count_if 
    template <class OBJECT, class PREDICATE>
    inline OBJECT find_if 
    ( OBJECT            first , 
      OBJECT            last  ,
      const PREDICATE&  cuts  )
    {
      for ( ; first != last ; ++first ) 
      { if ( cuts ( *first ) ) { return first ; } }
      return last  ;
    }
    /// useful shortcut
    template <class OBJECT,class PREDICATE>
    inline bool found 
    ( OBJECT            first , 
      OBJECT            last  ,
      const PREDICATE&  cuts  )
    {
      return last != LoKi::Algs::find_if ( first , last , cuts ) ;
    }
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
    template<class OBJECT,class FUNCTION,class PREDICATE>
    inline OBJECT select_min
    ( OBJECT           first , 
      OBJECT           last  , 
      const FUNCTION&  fun   , 
      const PREDICATE& cut   )
    {
      // empty sequence?
      if ( first == last ) { return last ; }
      // store 
      OBJECT result = last ;
      for ( ; first != last ; ++first ) 
      {
        if ( !cut ( *first) ) { continue ; }              // CONTINUE 
        if ( last == result || 
             ( fun(*first) < fun(*result) ) ) { result = first ; }  
      }
      return result ;
    }
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
    template<class OBJECT,class FUNCTION,class PREDICATE>
    inline OBJECT select_max
    ( OBJECT           first , 
      OBJECT           last  , 
      const FUNCTION&  fun   , 
      const PREDICATE& cut   )
    {
      // empty sequence?
      if ( first == last ) { return last ; }
      // store 
      OBJECT result = last ;
      for ( ; first != last ; ++first ) 
      {
        if ( !cut ( *first) ) { continue ; }              // CONTINUE 
        if ( last == result || 
             ( fun(*result) < fun(*first) ) )  { result = first ; }  
      }
      return result ;
    }    
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
     *                          PT           ) ; 
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
     *  @return iterator for the element with minimal value of function
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2005-03-09
     */      
    template<class OBJECT,class FUNCTION>
    inline OBJECT select_min
    ( OBJECT           first , 
      OBJECT           last  , 
      const FUNCTION&  fun   )
    {
      // empty sequence?
      if ( first == last ) { return last ; }
      // store 
      OBJECT result = last ;
      for ( ; first != last ; ++first ) 
      {
        if ( last == result || 
             ( fun(*first) < fun(*result) ) ) { result = first ; }  
      }
      return result ;
    }
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
     *                          PT           ); 
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
     *  @return iterator for the element with minimal value of function
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2005-03-09
     */      
    template<class OBJECT,class FUNCTION>
    inline OBJECT select_max
    ( OBJECT           first , 
      OBJECT           last  , 
      const FUNCTION&  fun   )
    {
      // empty sequence?
      if ( first == last ) { return last ; }
      // store 
      OBJECT result = last ;
      for ( ; first != last ; ++first ) 
      {
        if ( last == result || 
             ( fun(*result) < fun(*first) ) )  { result = first ; }  
      }
      return result ;
    }
    
  } // end of namespace LoKi::Algs
} // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ALGS_H
// ============================================================================
