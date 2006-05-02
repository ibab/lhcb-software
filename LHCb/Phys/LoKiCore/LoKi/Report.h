// $Id: Report.h,v 1.4 2006-05-02 14:29:10 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
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
class GaudiException ;

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
    
    /** Assertion - throw exception, if condition is not fulfilled 
     *  @see GaudiException
     *  @see LoKi::Exception
     *  @exception LoKi::Exception for invalid condifition 
     *  @exception GaudiException for invalid condifition 
     *  @param ok           condition which should be "true"
     *  @param message      message to be associated with the exception 
     *  @param sc           status code to be returned (artificial) 
     *  @return             status code        
     */ 
    inline static StatusCode Assert 
    ( const bool         ok                            , 
      const std::string& message = ""                  , 
      const StatusCode   sc      = StatusCode::FAILURE ) 
    {
      return ( ok ? StatusCode::SUCCESS : Exception( message , sc ) ) ;
    } ;
    
    /** Create and (re)-throw the exception  
     *  @see LoKi::Exception
     *  @exception LoKi::Exception always!
     *  @param msg    exception message 
     *  @param exc    (previous) exception of type GaudiException
     *  @param sc     status code  
     *  @return       status code (fictive) 
     */
    static StatusCode Exception 
    ( const std::string    & msg                        ,  
      const GaudiException & exc                        , 
      const StatusCode       sc  = StatusCode::FAILURE  ) ;
    
    /** Create and (re)-throw the exception  
     *  @see GaudiException
     *  @exception GaudiException always!
     *  @param msg    exception message 
     *  @param exc    (previous) exception of type std::exception
     *  @param sc     status code  
     *  @return       status code (fictive) 
     */
    static StatusCode Exception 
    ( const std::string    & msg                        ,  
      const std::exception & exc                        , 
      const StatusCode       sc  = StatusCode::FAILURE  ) ;
    
    /** Create and throw the exception  
     *  @see GaudiException
     *  @exception GaudiException always!
     *  @param msg    exception message 
     *  @param sc     status code  
     *  @return       status code (fictive) 
     */
    static StatusCode Exception 
    ( const std::string& msg = "no message"        ,  
      const StatusCode   sc  = StatusCode::FAILURE ) ;
    
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
