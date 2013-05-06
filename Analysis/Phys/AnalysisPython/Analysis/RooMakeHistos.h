// $Id$
// ===============================================================================
#ifndef ANALYSIS_ROOMAKEHISTOS_H 
#define ANALYSIS_ROOMAKEHISTOS_H 1
// ===============================================================================
// Include files
// ===============================================================================
// GaudiKernel 
// ===============================================================================
#include "GaudiKernel/Kernel.h"
// ===============================================================================
// ROOT
// ===============================================================================
#include "RooCmdArg.h"
// ===============================================================================
// forward declaration 
// ===============================================================================
class RooAbsData       ;
class TH1              ; 
class RooAbsRealLValue ;
// ===============================================================================
namespace Analysis 
{
  // ==============================================================================
  /** @class RooMakeHistos RooMakeHistos.h Analysis/RooMakeHistos.h
   *  Helper class to "fix" the problem with "masked" 
   *   RooAbsData::createHistogram method
   *
   *  @author Vanya Belyaev
   *  @date   2011-07-16
   */
  class GAUDI_API RooMakeHistos 
  {
  public:
    // ================
    /// the only one method 
    static TH1* create_histo 
    ( const RooAbsData&       dataset                   , 
      const std::string&      name                      , 
      const RooAbsRealLValue& xvar                      ,
      const RooCmdArg&        arg1 = RooCmdArg::none () , 
      const RooCmdArg&        arg2 = RooCmdArg::none () , 
      const RooCmdArg&        arg3 = RooCmdArg::none () , 
      const RooCmdArg&        arg4 = RooCmdArg::none () , 
      const RooCmdArg&        arg5 = RooCmdArg::none () , 
      const RooCmdArg&        arg6 = RooCmdArg::none () , 
      const RooCmdArg&        arg7 = RooCmdArg::none () , 
      const RooCmdArg&        arg8 = RooCmdArg::none () ) ;
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                  End of namespace Analysis 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // ANALYSIS_ROOMAKEHISTOS_H
// ============================================================================
