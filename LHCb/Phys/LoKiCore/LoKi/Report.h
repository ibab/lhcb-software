// $Id: Report.h,v 1.1.1.1 2006-01-24 09:39:42 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_REPORT_H 
#define LOKI_REPORT_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IMessageSvc.h"
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
  /** @class Report
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-01-17
   */
  class Report
  {
  public:    
    /** Print the error  message, return status code
     *  @param msg    error message 
     *  @param st     status code 
     *  @param mx     maximal number of printouts
     *  @return       status code 
     */
    static StatusCode Error     
    ( const std::string&  msg                       , 
      const StatusCode    st  = StatusCode::FAILURE ,
      const size_t        mx  = 10                  ) ;    
    /** Print the warning  message, return status code 
     *  @param msg    warning message 
     *  @param st     status code  
     *  @param mx     maximal number of printouts
     *  @return       status code 
     */
    static StatusCode Warning   
    ( const std::string&  msg                       , 
      const StatusCode    st  = StatusCode::FAILURE , 
      const size_t        mx  = 10                  ) ;
    /** Print the message and return status code 
     *  @param msg    warning message 
     *  @param st     status code 
     *  @param lev    print level 
     *  @return       status code 
     */
    static StatusCode Print     
    ( const std::string& msg                       , 
      const StatusCode   st  = StatusCode::FAILURE ,
      const MSG::Level   lev = MSG::INFO           ) ;    
    /** make a report 
     *  @return status code 
     */
    static StatusCode             report      () ;
  } ;
}; // end of namespace LoKi 


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_REPORT_H
// ============================================================================
