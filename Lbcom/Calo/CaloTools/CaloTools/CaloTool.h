// $Id: CaloTool.h,v 1.1.1.1 2001-11-01 13:21:40 ibelyaev Exp $
// ============================================================================
// ============================================================================
// ============================================================================
#ifndef CALOTOOLS_CALOTOOL_H 
#define CALOTOOLS_CALOTOOL_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Stat.h"
// 
// forward declarations 
class IChronoStatSvc ;
class IToolSvc       ;
class DeCalorimeter  ;
class GaudiException ;

/** @class CaloTool CaloTool.h CaloTools/CaloTool.h
 *   
 *  Useful base class for all Calorimeter tools  
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   01/11/2001
 */

class CaloTool: public AlgTool
{
  
protected:
  
  /** Standard constructor
   *  @param type tool type (useless)
   *  @param name tool name
   *  @param parent pointer to parent object (service, algorithm or tool)  
   */
  CaloTool( const std::string& type   ,
            const std::string& name   ,
            const IInterface*  parent ); 
  
  /** destructor, virtual and protected 
   */
  virtual ~CaloTool();

  /** standard initialization method 
   *  @return status code 
   */
  virtual StatusCode    initialize ();
  
  /** standard finalization method 
   *  @return status code 
   */
  virtual StatusCode    finalize   ();
  
  /** accessor to service locator 
   *  @return pointer to service locator 
   */
  inline ISvcLocator*         svcLoc    () const { return m_svcLoc   ; }
  
  /** accessor to Chrono & Stat Service  
   *  @return pointer to Chrono & Stat Service 
   */
  inline IChronoStatSvc*      chronoSvc () const { return m_chronoSvc ; }
  
  /** accessor to Tool Service (for creation of daighter tools)
   *  @return pointer to Tool Service 
   */ 
  inline IToolSvc*            toolSvc   () const { return m_toolSvc ; }
  
  /** accessor to associated calorimeter detector  (const version)
   *  @return pointer to associated calorimeter detector 
   */
  inline const DeCalorimeter* det      () const { return m_det ;}
  
  /** set new detector 
   *  @param Det pointer to new calorimeter detector
   */
  inline const DeCalorimeter* setDet ( const DeCalorimeter* Det )
  { m_det = Det ; return det() ; }

  /**  Print the error  message, return status code
       and perform the statistics of error messages 
       @param msg    error message 
       @param st     status code 
       @return       status code 
  */
  inline StatusCode 
  Error     ( const std::string& msg , 
              const StatusCode & st  = StatusCode::FAILURE ) const ;
  
  /**  Print the warning  message, return status code 
       and perform the statistics of warning  messages 
       @param msg    warning message 
       @param st     statsu code 
       @return       status code 
  */
  inline StatusCode 
  Warning   ( const std::string& msg , 
              const StatusCode & st  = StatusCode::FAILURE ) const ; 
  
  /**  Print the message and return status code 
       @param msg    warning message 
       @param st     status code 
       @param lev    print level 
       @return       status code 
  */
  inline StatusCode 
  Print     ( const std::string& msg , 
              const StatusCode & st  = StatusCode::FAILURE ,
              const MSG::Level & lev = MSG::INFO           ) const ;

  /** Assertion - throw exception, if condition is not fulfilled 
      @param ok            condition which should be "true"
      @param mesage       message to eb associated with the exception 
      @return             status code        
  */
  inline StatusCode 
  Assert ( bool ok , const std::string& message = " ") const;
  
  /** Assertion - throw exception, if condition is not fulfilled 
      @param ok            condition which shoudl be "true"
      @param mesage       message to eb associated with the exception 
      @return             status code        
  */
  inline StatusCode
  Assert ( bool ok , const char*        message  ) const ;
  
  /**  Create and (re)-throw the exception  
       @param msg    exception message 
       @param exc    (previous) exception of type GaudiException
       @param lvl    print level 
       @param sc     status code  
       @return       status code (fictive) 
  */
  StatusCode 
  Exception ( const std::string    & msg                        ,  
              const GaudiException & exc                        , 
              const MSG::Level     & lvl = MSG::FATAL           ,
              const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /**  Create and (re)-throw the exception  
       @param msg    exception message 
       @param exc    (previous) exception of type std::exception
       @param lvl    print level 
       @param sc     status code  
       @return       status code (fictive) 
  */
  StatusCode 
  Exception ( const std::string    & msg                        ,  
              const std::exception & exc                        , 
              const MSG::Level     & lvl = MSG::FATAL           ,
              const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /**  Create and throw the exception  
       @param msg    exception message 
       @param lvl    print level 
       @param sc     status code  
       @return       status code (fictive) 
  */
  StatusCode 
  Exception ( const std::string& msg = "no message"        ,  
              const MSG::Level & lvl = MSG::FATAL          ,
              const StatusCode & sc  = StatusCode::FAILURE ) const ;
  
private:
  
  /** default constructor is private!
   */
  CaloTool();
  
  /** copy constructor is private!
   */
  CaloTool( const CaloTool& copy );
  
  /** assignement operator is private!
   *  @param copy object to be copied 
   */
  CaloTool& operator=( const CaloTool& copy );
  
private:
  
  const DeCalorimeter*   m_det       ;
  IChronoStatSvc*        m_chronoSvc ;
  IToolSvc*              m_toolSvc   ;
  ISvcLocator*           m_svcLoc    ;
  
};

// ============================================================================
/**  Print the error  message and return status code
     @param msg    error message 
     @param st     status code 
     @return       status code 
*/
// ============================================================================
inline StatusCode 
CaloTool::Error     ( const std::string& msg , 
                      const StatusCode & st  ) const 
{
  Stat stat( chronoSvc() , name()+":Error" ); 
  return Print( msg , st , MSG::ERROR ); 
};

// ============================================================================
/**  Print the warning  message and return status code 
     @param msg    warning message 
     @param st     statsu code 
     @return       status code 
*/
// ============================================================================
inline StatusCode 
CaloTool::Warning   ( const std::string& msg , 
                      const StatusCode & st  ) const 
{
  Stat stat( chronoSvc() , name()+":Warning" ); 
  return Print( msg , st , MSG::WARNING ); 
};

// ============================================================================
/**  Print the message and return status code 
     @param msg    warning message 
     @param st     status code 
     @param lvl    print level 
     @return       status code 
*/
// ============================================================================
inline StatusCode 
CaloTool::Print     ( const std::string& msg , 
                      const StatusCode & st  ,
                      const MSG::Level & lvl ) const 
{
  MsgStream log( msgSvc() , name() ); 
  log << lvl 
      << type () 
      << " "   << msg ;
  ///
  if( !st.isSuccess() ) { log << " \tStatusCode=" << st ;}
  ///
  log << endreq ; 
  return  st;
};

// ============================================================================
/** Assertion - throw exception, if condition is not fulfilled 
    @param OK           condition which should be "true"
    @param mesage       message to eb associated with the exception 
    @return             status code        
*/ 
// ============================================================================
inline StatusCode 
CaloTool::Assert ( bool OK , const std::string& msg ) const
{
  StatusCode ok ( StatusCode::SUCCESS );
  return OK ? ok : Exception( msg , MSG::ERROR ) ; 
};

// ============================================================================
/** Assertion - throw exception, if condition is not fulfilled 
    @param OK           condition which should be "true"
    @param mesage       message to eb associated with the exception 
    @return             status code        
*/ 
// ============================================================================
inline StatusCode
CaloTool::Assert ( bool OK , const char*        msg  ) const
{ 
  StatusCode ok ( StatusCode::SUCCESS ) ;
  return OK ? ok : Exception( msg , MSG::ERROR ) ; 
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOTOOLS_CALOTOOL_H
// ============================================================================
