#ifndef LOKI_SELECT_H 
#define LOKI_SELECT_H 1
// ============================================================================
// Include files
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
  /** helpful utility to select object from the container
   *  to another container. 
   *  Indeed it is missing <tt>std::copy_if</tt> algorithm from STL
   *
   *  @param begin begin iterator from the sequence to be copied 
   *  @param end   end   iterator from the sequence to be copied 
   *  @param out   destination (output iterator) 
   *  @param cut   copy condition (predicate)
   *  @return last output iterator 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24   
   */
  template<class INPUT , class OUTPUT , class CUT>
  inline OUTPUT 
  __attribute__((deprecated("please replace LoKi::select with std::copy_if; you may have to wrap std::cref around your predicate")))
                select( INPUT      begin , 
                        INPUT      end   ,
                        OUTPUT     out   ,
                        const CUT& cut   )
  {
    for ( ; begin != end ; ++begin ) 
    { if ( cut (*begin) ) { *out = *begin ; ++out ; } }
    return out ;
  } 
  // ==========================================================================
  /** helpful utility to transform objects from one container
   *  to another container. 
   *
   *  @param begin begin iterator from the sequence to be copied 
   *  @param end   end   iterator from the sequence to be copied 
   *  @param out   destination (output iterator) 
   *  @param fun   thr function 
   *  @return last output iterator 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
   *  @date   2010-11-23   
   */
  template<class INPUT , class OUTPUT , class FUN>
  inline OUTPUT 
  __attribute__((deprecated("please replace LoKi::transform with std::transform; you may have to wrap std::cref around your predicate")))
  transform 
  ( INPUT      begin , 
    INPUT      end   ,
    OUTPUT     out   ,
    const FUN& fun   )
  {
    for ( ; begin != end ; ++begin ) { *out = fun ( *begin ) ; ++out ; }
    return out ;
  } 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_SELECT_H
// ============================================================================
