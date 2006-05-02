// $Id: Report.cpp,v 1.4 2006-05-02 14:29:11 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
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

// ============================================================================
/** Print the error  message, return status code
 *  @param msg    error message 
 *  @param st     status code 
 *  @param mx     maximal number of printouts
 *  @return       status code 
 */
// ============================================================================
StatusCode LoKi::Report::Error     
( const std::string&  msg , 
  const StatusCode    st  ,
  const size_t        mx  ) 
{ return LoKi::ErrorReport::instance().Error( msg , st , mx ) ; } ;
// ============================================================================

// ============================================================================
/** Print the warning  message, return status code 
 *  @param msg    warning message 
 *  @param st     status code  
 *  @param mx     maximal number of printouts
 *  @return       status code 
 */
// ============================================================================
StatusCode LoKi::Report::Warning   
( const std::string&  msg , 
  const StatusCode    st  , 
  const size_t        mx  ) 
{ return LoKi::ErrorReport::instance().Warning( msg , st , mx ) ; } ;
// ============================================================================


// ============================================================================
/** Print the message and return status code 
 *  @param msg    warning message 
 *  @param st     status code 
 *  @param lev    print level 
 *  @return       status code 
 */
// ============================================================================
StatusCode LoKi::Report::Print     
( const std::string& msg , 
  const StatusCode   st  ,
  const MSG::Level   lev )
{ return LoKi::ErrorReport::instance().Print( msg , st , lev  ) ; } ; 
// ============================================================================

// ============================================================================
/** Create and (re)-throw the exception  
 *  @see LoKi::Exception
 *  @exception LoKi::Exception always!
 *  @param msg    exception message 
 *  @param exc    (previous) exception of type GaudiException
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode LoKi::Report::Exception 
( const std::string    & msg ,  
  const GaudiException & exc , 
  const StatusCode       sc  ) 
{ return LoKi::ErrorReport::instance().Exception( msg , exc , sc ) ; } ; 
// ============================================================================

// ============================================================================
/** Create and (re)-throw the exception  
 *  @see GaudiException
 *  @exception GaudiException always!
 *  @param msg    exception message 
 *  @param exc    (previous) exception of type std::exception
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode LoKi::Report::Exception 
( const std::string    & msg ,  
  const std::exception & exc , 
  const StatusCode       sc  ) 
{ return LoKi::ErrorReport::instance().Exception( msg , exc , sc ) ; } ; 
// ============================================================================

// ============================================================================
/** Create and throw the exception  
 *  @see GaudiException
 *  @exception GaudiException always!
 *  @param msg    exception message 
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode LoKi::Report::Exception 
( const std::string& msg ,  
  const StatusCode   sc  ) 
{ return LoKi::ErrorReport::instance().Exception( msg , sc ) ; } ; 
// ============================================================================


// ============================================================================
/** make a report 
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::Report::report      () 
{ return LoKi::ErrorReport::instance().report() ; } ; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

