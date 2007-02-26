// $Id: Algs.h,v 1.7 2007-02-26 13:13:08 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2006/11/25 19:12:55  ibelyaev
//  improve Doxygen
//
// Revision 1.5  2006/05/02 14:29:09  ibelyaev
//  censored
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
    template <class OBJECT,class RESULT,class FUNCTOR>
    inline RESULT accumulate
    ( OBJECT     first     ,
      OBJECT     last      , 
      FUNCTOR    functor   ,
      RESULT     result    )
    {
      for ( ; first != last ; ++first ) { result += functor( *first ) ; }
      return result ;
    } ;
    // ========================================================================
    
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
    template <class OBJECT,class RESULT,class FUNCTOR,class PREDICATE>
    inline RESULT accumulate
    ( OBJECT     first     ,
      OBJECT     last      , 
      FUNCTOR    functor   ,
      PREDICATE  predicate ,
      RESULT     result    )
    {
      for ( ; first != last ; ++first ) 
      { if ( predicate( *first ) ) { result += functor( *first ) ; } }
      return result ;
    } ;
    // ========================================================================
    
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

  } // end of namespace LoKi::Algs 
  
} // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ALGS_H
// ============================================================================
