// $Id$
// ============================================================================
#ifndef ANALYSIS_USTAT_H 
#define ANALYSIS_USTAT_H 1
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// forward declaration
// ============================================================================
class TH1 ;                                     \
class RooAbsPdf  ;
class RooArgSet  ;
class RooDataSet ;
// ============================================================================
namespace Analysis 
{
  // ==========================================================================
  /** @class UStat UStat.h Analysis/UStat.h
   *  
   *
   *  @author Vanya Belyaev
   *  @date   2011-09-27
   */
  class GAUDI_API UStat 
  {
  public:
    // ========================================================================
    enum {
      InvalidArgs = 450 ,
      InvalidDims       ,
      InvalidItem1      ,
      InvalidItem2      , 
      InvalidDist       
    } ;  
    // ========================================================================
  public: 
    // ========================================================================
    /** calculate U-statistics 
     *  @param pdf   (input) PDF
     *  @param data  (input) data 
     *  @param hist  (update) the histogram with U-statistics 
     *  @param tStat (update) value for T-statistics 
     *  @param args  (input)  the arguments
     */
    static StatusCode calculate
    ( const RooAbsPdf&  pdf       , 
      const RooDataSet& data      ,  
      TH1&              hist      ,
      double&           tStat     ,
      RooArgSet *       args  = 0 ) ;
    // ========================================================================
  };
  // ==========================================================================
} //                                                  end of namespace Analysis
// ============================================================================
#endif // ANALYSIS_USTAT_H
// ============================================================================
