// $Id: Report.cpp,v 1.8 2010-01-13 11:04:17 ibelyaev Exp $
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/Report.h"
#include "LoKi/ErrorReport.h"
// ============================================================================
/** @file
 *
 * Implementation file for class LoKi::Report
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
/*  Print the error  message, return status code
 *  @param msg    error message 
 *  @param st     status code 
 *  @param mx     maximal number of printouts
 */
// ============================================================================
StatusCode LoKi::Report::Error     
( const std::string&  msg , 
  const StatusCode&   st  ,
  const size_t        mx  ) 
{ 
  st.ignore() ;
  return LoKi::ErrorReport::instance().Error( msg , st , mx ) ;
} 
// ============================================================================
/*  Print the warning  message, return status code 
 *  @param msg    warning message 
 *  @param st     status code  
 *  @param mx     maximal number of printouts
 */
// ============================================================================
StatusCode LoKi::Report::Warning   
( const std::string&  msg , 
  const StatusCode&   st  , 
  const size_t        mx  ) 
{
  st.ignore() ;
  return LoKi::ErrorReport::instance().Warning ( msg , st , mx ) ;
} 
// ============================================================================
/*  Print the message and return status code 
 *  @param msg    warning message 
 *  @param st     status code 
 *  @param lev    print level 
 *  @return       status code 
 */
// ============================================================================
void LoKi::Report::Print     
( const std::string& msg , 
  const StatusCode&  st  ,
  const MSG::Level   lev )
{ 
  st.ignore() ; 
  LoKi::ErrorReport::instance().Print( msg , st , lev  ).ignore() ; 
}  
// ============================================================================
/*  Create and (re)-throw the exception  
 *  @see LoKi::Exception
 *  @exception LoKi::Exception always!
 *  @param msg    exception message 
 *  @param exc    (previous) exception of type GaudiException
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
void LoKi::Report::Exception 
( const std::string    & msg ,  
  const GaudiException & exc , 
  const StatusCode&      sc  ) 
{ 
  sc.ignore() ; 
  LoKi::ErrorReport::instance().Exception( msg , exc , sc ) ; 
}  
// ============================================================================
/*  Create and (re)-throw the exception  
 *  @see GaudiException
 *  @exception GaudiException always!
 *  @param msg    exception message 
 *  @param exc    (previous) exception of type std::exception
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
void LoKi::Report::Exception 
( const std::string    & msg ,  
  const std::exception & exc , 
  const StatusCode&      sc  ) 
{ 
  sc.ignore() ;
  LoKi::ErrorReport::instance().Exception( msg , exc , sc ) ; 
} 
// ============================================================================
/*  Create and throw the exception  
 *  @see GaudiException
 *  @exception GaudiException always!
 *  @param msg    exception message 
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
void LoKi::Report::Exception 
( const std::string& msg ,  
  const StatusCode&  sc  ) 
{ 
  sc.ignore() ;
  LoKi::ErrorReport::instance().Exception( msg , sc ) ; 
}                                                                   
// ============================================================================
//  make a report 
// ============================================================================
void LoKi::Report::report      () 
{ 
  LoKi::ErrorReport::instance().report() ; 
}  
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

