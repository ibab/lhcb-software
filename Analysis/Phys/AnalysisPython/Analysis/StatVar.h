// $Id:$ 
// ============================================================================
#ifndef ANALYSIS_STATVAR_H
#define ANALYSIS_STATVAR_H 1
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatEntity.h"
// ============================================================================
// Forward declarations 
// =============================================================================
class TTree  ;     // ROOT 
class TChain ;     // ROOT 
// =============================================================================
namespace Analysis 
{
  // ==========================================================================
  /** @class StatVar Analysis/StatVar.h
   *  Helper class to get statistical 
   *  infomation  about the variable/expression 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2013-10-13
   * 
   *                    $Revision:$
   *  Last modification $Date:$
   *                 by $Author:$
   */
  class GAUDI_API StatVar 
  {
  public: 
    // ========================================================================
    /** build statistic for the <code>expression</code>
     *  @param tree (INPUT) the tree 
     *  @param expression (INPUT) the expression
     *
     *  @code
     *  tree = ... 
     *  stat = tree.statVar( 'S_sw' ) 
     *  @endcode 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2013-10-13
     */
    static StatEntity 
    statVar ( TTree*             tree       , 
              const std::string& expression ) ;
    // ========================================================================
    /** build statistic for the <code>expression</code>
     *  @param tree       (INPUT) the tree 
     *  @param expression (INPUT) the expression
     *  @param cuts       (INPUT) the selection criteria 
     *
     *  @code
     *  tree = ... 
     *  stat = tree.statVar( 'S_sw' ,'pt>1000') 
     *  @endcode 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2013-10-13
     */
    static StatEntity 
    statVar ( TTree*             tree       , 
              const std::string& expression , 
              const std::string& cuts       ) ;
    // ========================================================================
  };
  // ==========================================================================
} //                                                  end of namespace Analysis
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // ANALYSIS_STATVAR_H
// ============================================================================
