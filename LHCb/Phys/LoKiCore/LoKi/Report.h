// $Id: Report.h,v 1.6 2007-06-03 20:38:24 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2006/11/25 19:12:56  ibelyaev
//  improve Doxygen
//
// Revision 1.4  2006/05/02 14:29:10  ibelyaev
//  censored
//
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
    /** Print the error  message
     *  @param msg    error message 
     *  @param st     status code 
     *  @param mx     maximal number of printouts
     */
    static StatusCode Error     
    ( const std::string&  msg                       , 
      const StatusCode    st  = StatusCode::FAILURE ,
      const size_t        mx  = 10                  ) ;    
    /** Print the warning  message
     *  @param msg    warning message 
     *  @param st     status code  
     *  @param mx     maximal number of printouts
     */
    static StatusCode Warning   
    ( const std::string&  msg                       , 
      const StatusCode    st  = StatusCode::FAILURE , 
      const size_t        mx  = 10                  ) ;
    /** Print the message 
     *  @param msg    warning message 
     *  @param st     status code 
     *  @param lev    print level 
     */
    static void Print     
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
     */ 
    inline 
    static void Assert 
    ( const bool         ok                            , 
      const std::string& message = ""                  , 
      const StatusCode   sc      = StatusCode::FAILURE ) 
    {
      if ( !ok) { Exception( message , sc ) ; } ; sc.ignore() ;
    }
    /** Create and (re)-throw the exception  
     *  @see LoKi::Exception
     *  @exception LoKi::Exception always!
     *  @param msg    exception message 
     *  @param exc    (previous) exception of type GaudiException
     *  @param sc     status code  
     */
    static void Exception 
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
    static void Exception 
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
    static void Exception 
    ( const std::string& msg = "no message"        ,  
      const StatusCode   sc  = StatusCode::FAILURE ) ;
    /// make a report 
    static void report      () ;
  } ;
} // end of namespace LoKi 


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_REPORT_H
// ============================================================================
