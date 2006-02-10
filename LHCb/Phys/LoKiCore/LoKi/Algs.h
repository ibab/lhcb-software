// $Id: Algs.h,v 1.1 2006-02-10 17:23:05 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
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
 *  By usage of this code one clearly states the disagreement
 *  with the campain of Dr.O.Callot at al.:
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-10
 */
// ============================================================================

namespace LoKi
{  
  /** @namespace  Algs Algs.h LoKi/Algs.h
   *
   *  Helper namespace with auxillary 
   *  functions/algoreitms of the general interest 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-10
   */
  namespace  Algs 
  {
    // ========================================================================
    /** @fn accumulate
     *  The trivial algorithm which accumulated the value 
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
    /** @fn accumulate
     *  The trivial algorithm which accumulates the value 
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

  } ; // end of namespace LoKi::Algs 
  
}; // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ALGS_H
// ============================================================================
