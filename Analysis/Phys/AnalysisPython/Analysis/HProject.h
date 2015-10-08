// $Id:$
// ===========================================================================
#ifndef ANALYSIS_HPROJECT_H 
#define ANALYSIS_HPROJECT_H 1
// ============================================================================
// Include files
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <limits>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// Forward declarations 
// =============================================================================
class TH1       ;     // ROOT 
class TH2       ;     // ROOT 
class TH3       ;     // ROOT 
// =============================================================================
class RooAbsData ; // RooFit 
class RooAbsReal ; // RooFit 
// =============================================================================
namespace Analysis 
{
  // ==========================================================================
  /** @class Project Project.h Analysis/Project.h
   *  Helper class to project Dataste to histogram 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2015-10-08
   * 
   *                    $Revision: 195425 $
   *  Last modification $Date: 2015-10-01 15:33:56 +0200 (Thu, 01 Oct 2015) $
   *                 by $Author: ibelyaev $
   */
  class GAUDI_API HProject 
  {
  public:
    // ========================================================================
    /** make a projection of RooDataSet into the histogram 
     *  @param data  (INPUT)  input data 
     *  @param histo (UPDATE) histogram 
     *  @param expression (INPUT) expression
     *  @param selection  (INPUT) selection criteria/weight 
     */
    static StatusCode project
      ( const RooAbsData*   data            , 
        TH1*                histo           ,
        const std::string&  expression      ,
        const std::string&  selection  = "" ,
        const unsigned long first      = 0                                         ,
        const unsigned long last       = std::numeric_limits<unsigned long>::max() ) ;
    // ========================================================================
    /** make a projection of RooDataSet into the histogram 
     *  @param data  (INPUT)  input data 
     *  @param histo (UPDATE) histogram 
     *  @param xexpression (INPUT) expression for x-axis 
     *  @param yexpression (INPUT) expression for y-axis 
     *  @param selection  (INPUT) selection criteria/weight 
     */
    static StatusCode project2
      ( const RooAbsData*   data            , 
        TH2*                histo           ,
        const std::string&  xexpression     ,
        const std::string&  yexpression     ,
        const std::string&  selection  = "" ,
        const unsigned long first      = 0                                         ,
        const unsigned long last       = std::numeric_limits<unsigned long>::max() ) ;
    // ========================================================================
    /** make a projection of RooDataSet into the histogram 
     *  @param data  (INPUT)  input data 
     *  @param histo (UPDATE) histogram 
     *  @param xexpression (INPUT) expression for x-axis 
     *  @param yexpression (INPUT) expression for y-axis 
     *  @param zexpression (INPUT) expression for z-axis 
     *  @param selection  (INPUT) selection criteria/weight 
     */
    static StatusCode project3
      ( const RooAbsData*   data            , 
        TH3*                histo           ,
        const std::string&  xexpression     ,
        const std::string&  yexpression     ,
        const std::string&  zexpression     ,
        const std::string&  selection  = "" ,
        const unsigned long first      = 0                                         ,
        const unsigned long last       = std::numeric_limits<unsigned long>::max() ) ;
    // ========================================================================
  public:
    // ========================================================================
    /** make a projection of RooDataSet into the histogram 
     *  @param data  (INPUT)  input data 
     *  @param histo (UPDATE) histogram 
     *  @param expression (INPUT) expression
     *  @param selection  (INPUT) selection criteria/weight 
     */
    static StatusCode project
      ( const RooAbsData*   data            , 
        TH1*                histo           ,
        const RooAbsReal&   expression      ,
        const RooAbsReal*   selection  = 0  ,
        const unsigned long first      = 0                                         ,
        const unsigned long last       = std::numeric_limits<unsigned long>::max() ) ;
    // ========================================================================
    /** make a projection of RooDataSet into the histogram 
     *  @param data  (INPUT)  input data 
     *  @param histo (UPDATE) histogram 
     *  @param xexpression (INPUT) expression for x-axis 
     *  @param yexpression (INPUT) expression for y-axis 
     *  @param selection  (INPUT) selection criteria/weight 
     */
    static StatusCode project2
      ( const RooAbsData*   data            , 
        TH2*                histo           ,
        const RooAbsReal&   xexpression     ,
        const RooAbsReal&   yexpression     ,
        const RooAbsReal*   selection  = 0  ,
        const unsigned long first      = 0                                         ,
        const unsigned long last       = std::numeric_limits<unsigned long>::max() ) ;
    // ========================================================================
    /** make a projection of RooDataSet into the histogram 
     *  @param data  (INPUT)  input data 
     *  @param histo (UPDATE) histogram 
     *  @param xexpression (INPUT) expression for x-axis 
     *  @param yexpression (INPUT) expression for y-axis 
     *  @param zexpression (INPUT) expression for z-axis 
     *  @param selection  (INPUT) selection criteria/weight 
     */
    static StatusCode project3
      ( const RooAbsData*   data            , 
        TH3*                histo           ,
        const RooAbsReal&   xexpression     ,
        const RooAbsReal&   yexpression     ,
        const RooAbsReal&   zexpression     ,
        const RooAbsReal*   selection  = 0  ,
        const unsigned long first      = 0                                         ,
        const unsigned long last       = std::numeric_limits<unsigned long>::max() ) ;
    // ========================================================================
  };
  // ==========================================================================
} //                                                  end of namespace Analysis
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // ANALYSIS_HPROJECT_H
// ============================================================================

