// $Id: select.h,v 1.1.1.1 2006-01-24 09:39:42 ibelyaev Exp $
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $ , version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_SELECT_H 
#define LOKI_SELECT_H 1
// ============================================================================
// Include files
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
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi
{  
  /** @fn select 
   *
   *  helpful utility to select object from the container
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
  inline OUTPUT select( INPUT      begin , 
                        INPUT      end   ,
                        OUTPUT     out   ,
                        const CUT& cut   )
  {
    for ( ; begin != end ; ++begin ) 
    { if ( cut (*begin) ) { *out = *begin ; ++out ; } }
    return out ;
  } ;  
} ; // end of namespace LoKi 


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_SELECT_H
// ============================================================================
