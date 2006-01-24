// $Id: ErrorReport.h,v 1.1.1.1 2006-01-24 09:39:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_ERRORREPORT_H 
#define LOKI_ERRORREPORT_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/HashMap.h"
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
  class IReporter;
  
  /** @class ErrorReport ErrorReport.h LoKi/ErrorReport.h
   *  
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-04-15
   */
  class ErrorReport 
  {
  public:
    
    /** static function for the instance of the reporter 
     *  @return the refence to the unique static instance 
     */
    static ErrorReport& instance() ;
    
    /** get the LoKi reporter
     *  @return pointer to currently active LoKi error reporter tool
     */
    const LoKi::IReporter* reporter    () const ;
    
    /** set new active eporter 
     *  @return status code 
     */
    StatusCode             setReporter ( const LoKi::IReporter* reporter ) ;
  
    /** make a report 
     *  @return status code 
     */
    StatusCode             report      () const ;
    
    /** Print the error  message, return status code
     *  @param msg    error message 
     *  @param st     status code 
     *  @param mx     maximal number of printouts
     *  @return       status code 
     */
    StatusCode Error     
    ( const std::string&  msg                       , 
      const StatusCode    st  = StatusCode::FAILURE ,
      const size_t        mx  = 10                  ) const  ;
    
    /** Print the warning  message, return status code 
     *  @param msg    warning message 
     *  @param st     status code  
     *  @param mx     maximal number of printouts
     *  @return       status code 
     */
    StatusCode Warning   
    ( const std::string&  msg                       , 
      const StatusCode    st  = StatusCode::FAILURE , 
      const size_t        mx  = 10                  ) const  ;
    
    /** Print the message and return status code 
     *  @param msg    warning message 
     *  @param st     status code 
     *  @param lev    print level 
     *  @return       status code 
     */
    StatusCode Print     
    ( const std::string& msg                       , 
      const StatusCode   st  = StatusCode::FAILURE ,
      const MSG::Level   lev = MSG::INFO           ) const ;
  
  private:
    
    /// private destructor 
    ~ErrorReport( ); 
    
  private:
    
    /// private default constructor
    ErrorReport();
    // private copy constructor
    ErrorReport( const ErrorReport&);
    
    
  private:
    
    typedef GaudiUtils::HashMap<std::string,unsigned long> Counter ;   
    /// counter of errorr 
    mutable Counter m_errors     ;
    /// counter of warnings 
    mutable Counter m_warnings   ;
    
    const LoKi::IReporter* m_reporter ;    
  };

}; // end of namespace LoKi 


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ERRORREPORT_H
// ============================================================================
