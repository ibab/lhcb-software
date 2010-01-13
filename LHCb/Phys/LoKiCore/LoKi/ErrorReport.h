// $Id: ErrorReport.h,v 1.11 2010-01-13 11:04:17 ibelyaev Exp $
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
  class IReporter; 
  // ==========================================================================
  /** @class ErrorReport ErrorReport.h LoKi/ErrorReport.h
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-04-15
   */
  class ErrorReport 
  {
  public: 
    // ========================================================================
    /** static function for the instance of the reporter 
     *  @return the refence to the unique static instance 
     */
    static ErrorReport& instance() ;
    // ========================================================================
    /** get the LoKi reporter
     *  @return pointer to currently active LoKi error reporter tool
     */
    const LoKi::IReporter* reporter    () const ;
    // ========================================================================
    /** set new active eporter 
     *  @return status code 
     */
    StatusCode             setReporter ( const LoKi::IReporter* reporter ) ;
    // ========================================================================
    /** make a report 
     *  @return status code 
     */
    StatusCode             report      () const ;
    // ========================================================================
    /** Print the error  message, return status code
     *  @param msg    error message 
     *  @param st     status code 
     *  @param mx     maximal number of printouts
     *  @return       status code 
     */
    StatusCode Error     
    ( const std::string&  msg                   , 
      const StatusCode&   st  = 
      StatusCode ( StatusCode::FAILURE , true ) ,
      const size_t        mx  = 10              ) const  ;
    // ========================================================================
    /** Print the warning  message, return status code 
     *  @param msg    warning message 
     *  @param st     status code  
     *  @param mx     maximal number of printouts
     *  @return       status code 
     */
    StatusCode Warning   
    ( const std::string&  msg                   , 
      const StatusCode&   st  = 
      StatusCode ( StatusCode::FAILURE , true ) , 
      const size_t        mx  = 10              ) const  ;
    // ========================================================================
    /** Print the message and return status code 
     *  @param msg    warning message 
     *  @param st     status code 
     *  @param lev    print level 
     *  @return       status code 
     */
    StatusCode Print     
    ( const std::string& msg                    , 
      const StatusCode&  st  = 
      StatusCode ( StatusCode::SUCCESS , true ) ,
      const MSG::Level   lev = MSG::INFO        ) const ;
    // ========================================================================
    /** Assertion - throw exception, if condition is not fulfilled 
     *  @see GaudiException
     *  @see LoKi::Exception
     *  @exception LoKi::Exception for invalid condifition 
     *  @exception GaudiException for invalid condifition 
     *  @param ok           condition which should be "true"
     *  @param message      message to be associated with the exception 
     *  @param sc           status code to be returned (artificial) 
     */ 
    inline void       Assert 
    ( const bool         ok                     , 
      const std::string& message = ""           , 
      const StatusCode&  sc      = 
      StatusCode ( StatusCode::FAILURE , true ) ) const 
    {
      if  ( !ok ) { Exception( message , sc ) ; }
    } 
    // ========================================================================
    /** Create and (re)-throw the exception  
     *  @see LoKi::Exception
     *  @exception LoKi::Exception always!
     *  @param msg    exception message 
     *  @param exc    (previous) exception of type GaudiException
     *  @param sc     status code  
     */
    void       Exception 
    ( const std::string    & msg                        ,  
      const GaudiException & exc                        , 
      const StatusCode&      sc  = 
      StatusCode ( StatusCode::FAILURE , true ) ) const ;
    // ========================================================================
    /** Create and (re)-throw the exception  
     *  @see GaudiException
     *  @exception GaudiException always!
     *  @param msg    exception message 
     *  @param exc    (previous) exception of type std::exception
     *  @param sc     status code  
     */
    void       Exception 
    ( const std::string    & msg                        ,  
      const std::exception & exc                        , 
      const StatusCode&      sc  = 
      StatusCode ( StatusCode::FAILURE , true ) ) const ;
    // ========================================================================
    /** Create and throw the exception  
     *  @see GaudiException
     *  @exception GaudiException always!
     *  @param msg    exception message 
     *  @param sc     status code  
     */
    void       Exception 
    ( const std::string& msg = "no message"        ,  
      const StatusCode&  sc  = 
      StatusCode ( StatusCode::FAILURE , true ) ) const ;
    // ========================================================================
  private:
    // ========================================================================
    /// private destructor 
    ~ErrorReport( );
    // ========================================================================
  private:
    // ========================================================================
    /// private default constructor
    ErrorReport();
    // private copy constructor
    ErrorReport( const ErrorReport&);
    // ========================================================================
  private:
    // ========================================================================
    typedef GaudiUtils::HashMap<std::string,unsigned long> Counter ;   
    /// counter of errorr 
    mutable Counter m_errors     ; // counter of errorr 
    /// counter of warnings 
    mutable Counter m_warnings   ; // counter of warnings 
    /// counter of exception
    mutable Counter m_exceptions ; //  counter of exceptions
    //
    const LoKi::IReporter* m_reporter ;    
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ERRORREPORT_H
// ============================================================================
