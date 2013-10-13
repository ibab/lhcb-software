// $Id$
// =============================================================================
#ifndef ANALYSIS_SFACTOR_H 
#define ANALYSIS_SFACTOR_H 1
// =============================================================================
// Include files
// =============================================================================
// GaudiKernel
// =============================================================================
#include "GaudiKernel/Kernel.h"
// =============================================================================
// LHCbMath
// =============================================================================
#include "LHCbMath/ValueWithError.h"
// =============================================================================
// Forward declarations 
// =============================================================================
class TTree   ;     // ROOT 
class TTChain ;     // ROOT 
// =============================================================================
namespace Analysis
{
  // ==========================================================================
  /** @class SFactor  Analysis/SFactor.h
   *  Get sum and sum of squares for the simple branch in Tree, e.g.
   *  s-factor from usage of s_weight 
   *  The direct summation in python is rather slow, thus C++ routine helps
   *  to speedup procedure drastically 
   * 
   *  @code 
   *
   *  tree  = ...
   *  sf = tree.sFactor ( "S_sw")
   *  sumw  = sf.value () 
   *  sumw2 = sf.cov2  () 
   * 
   *  scale = sumw/sumw2 ## use in fit! 
   *
   *  @endcode 
   *  
   *  Also it is a way to get the signal component (with right uncertainty)
   *
   *  @param  tree    (INPUT) the tree 
   *  @param  varname (INPUT) name for the simple variable 
   *  @return s-fatcor in a form of value +- sqrt(cov2)  
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2013-04-27
   */
  class GAUDI_API SFactor 
  {
  public : 
    // ========================================================================
    /** Get sum and sum of squares for the simple branch in Tree, e.g.
     *  s-factor from usage of s_weight 
     *  The direct summation in python is rather slow, thus C++ routine helps
     *  to speedup procedure drastically 
     * 
     *  @code 
     *
     *  tree  = ...
     *  sf = tree.sFactor ( "S_sw")
     *  sumw  = sf.value () 
     *  sumw2 = sf.cov2  () 
     * 
     *  scale = sumw/sumw2 ## use in fit! 
     *
     *  @endcode 
     *  
     *  Also it is a way to get the signal component (with right uncertainty)
     *
     *  @param  tree    (INPUT) the tree 
     *  @param  varname (INPUT) name for the simple variable 
     *  @return s-fatcor in a form of value +- sqrt(cov2)  
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2013-04-27
     */
    static Gaudi::Math::ValueWithError
    sFactor ( TTree* tree ,  const std::string& varname = "S_sw" ) ;
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                  end of namespace Analysis
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // ANALYSIS_SFACTOR_H
// ============================================================================
