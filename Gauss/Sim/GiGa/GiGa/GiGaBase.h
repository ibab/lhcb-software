// $Id: GiGaBase.h,v 1.15 2002-05-07 12:21:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef     GIGA_GIGABASE_H
#define     GIGA_GIGABASE_H 1 
// ============================================================================
// STL
#include <string>
#include <exception>
#include <map>
// GaudiKernel 
#include "GaudiKernel/ISerialize.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgTool.h"
// GiGa
#include "GiGa/IGiGaInterface.h"
// forward declarations 
class IMessageSvc          ;
class ISvcLocator          ;
class IChronoStatSvc       ;
class IDataProviderSvc     ;
class IIncidentSvc         ;
class IObjManager          ;
class IGiGaSvc             ; 
class IGiGaSetUpSvc        ; 
//
class PropertyMgr          ;
class MsgStream            ;
class StreamBuffer         ;
class GaudiException       ;
//

/** @class GiGaBase GiGaBase.h GiGa/GiGaBase.h
 *    
 *  Helper class for implementation of some GiGa classes. 
 *  It allows an easy configuration of properties and services location  
 *  Implement almost all virtual "technical functions".
 *
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */

class  GiGaBase:
  public virtual IGiGaInterface    , 
  public virtual IIncidentListener , 
  public virtual        ISerialize ,
  public                   AlgTool 
{
protected:
  
  /** standard constructor 
   *  @see AlgTool 
   *  @param type tool   type (?)  
   *  @param name object name 
   *  @param parent pointer to parent object  
   */
  GiGaBase( const std::string& type   ,
            const std::string& name   , 
            const IInterface*  parent );
  /// virtual destructor 
  virtual ~GiGaBase();

public:

  /** initialize the object
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode initialize() ;

  /** finalize the object 
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode finalize  () ;

  /// serialize object for reading 
  virtual StreamBuffer& serialize( StreamBuffer& S )       ;

  /// serialize object for writing 
  virtual StreamBuffer& serialize( StreamBuffer& S ) const ; 

  /** handle the incident
   *  @param i reference to the incident
   */ 
  virtual void handle( const Incident& i ) ;
  /// 
protected:
  
  /** accessor to service locator 
   *  @return pointer to service locator 
   */
  inline ISvcLocator*      svcLoc    () const { return serviceLocator() ; };  
  
  /** accessor to GiGa Service 
   *  @return pointer to GiGa Service 
   */
  inline IGiGaSvc*         gigaSvc   () const { return m_gigaSvc   ; }; 

  /** accessor to GiGa SetUp Service 
   *  @return pointer to GiGa SetUp Service 
   */
  inline IGiGaSetUpSvc*    setupSvc  () const { return m_setupSvc  ; };

  /** accessor to Chrono & Stat Service 
   *  @return pointer to Chrono & Stat Service 
   */
  inline IChronoStatSvc*   chronoSvc () const { return m_chronoSvc ; };

  /** accessor to Event Data  Service 
   *  @return pointer to Event Data  Service 
   */
  inline IDataProviderSvc* evtSvc    () const { return m_evtSvc    ; }; 

  /** accessor to Detector Data Service 
   *  @return pointer to Detector Data  Service 
   */
  inline IDataProviderSvc* detSvc    () const { return m_detSvc    ; }; 

  /** accessor to Incident  Service 
   *  @return pointer to Message Service 
   */
  inline IIncidentSvc*     incSvc    () const { return m_incSvc    ; }; 

  /** accessor to Object Manager
   *  @return pointer to Object Manager  
   */
  inline IObjManager*      objMgr    () const { return m_objMgr    ; };  
  ///
  
protected:

  /** assertion 
   *  @param assertion   assertion condition
   *  @param msg         assertion message 
   *  @param sc          assertion status code 
   *  @return status code 
   */
  inline StatusCode Assert
  ( bool  assertion                               , 
    const std::string& msg = "GiGaBase::unknown"  , 
    const StatusCode&  sc  = StatusCode::FAILURE  ) const ;
  
  /** assertion 
   *  @param assertion   assertion condition
   *  @param msg         assertion message 
   *  @param sc          assertion status code 
   *  @return status code 
   */
  inline StatusCode Assert
  ( bool  assertion                              ,
    const char*        msg                       ,
    const StatusCode&  sc  = StatusCode::FAILURE ) const ;
  
  /** Print the error    message and return status code 
   *  @param mgs message to be printed 
   *  @param sc  status code 
   *  @return status code 
   */
  StatusCode Error   
  ( const std::string& msg , 
    const StatusCode & sc  = StatusCode::FAILURE ) const ;  
  
  /** Print the warning  message and return status code 
   *  @param mgs message to be printed 
   *  @param sc  status code 
   *  @return status code 
   */
  StatusCode Warning 
  ( const std::string& msg , 
    const StatusCode & sc  = StatusCode::FAILURE ) const ;
  
  /** Print the message and return status code 
   *  @param mgs message to be printed 
   *  @param sc  status code 
   *  @param lvl print level  
   *  @return status code 
   */
  StatusCode Print   
  ( const std::string& msg , 
    const StatusCode & sc  = StatusCode::SUCCESS ,
    const MSG::Level & lvl = MSG::INFO           ) const ;
  
  /** re-throw the exception and print 
   *  @param msg exception message  
   *  @param exc previous exception 
   *  @param lvl print level 
   *  @param status code
   *  @return status code 
   */
  StatusCode Exception 
  ( const std::string    & msg                       ,  
    const GaudiException & exc                       , 
    const MSG::Level     & lvl = MSG::FATAL          ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /** re-throw the exception and print 
   *  @param msg exception message  
   *  @param exc previous exception 
   *  @param lvl print level 
   *  @param status code
   *  @return status code 
   */
  StatusCode Exception 
  ( const std::string    & msg                       ,  
    const std::exception & exc                       , 
    const MSG::Level     & lvl = MSG::FATAL          ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /** re-throw the exception and print 
   *  @param msg exception message  
   *  @param lvl print level 
   *  @param status code
   *  @return status code 
   */
  StatusCode Exception 
  ( const std::string    & msg                       ,  
    const MSG::Level     & lvl = MSG::FATAL          ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
private: 
  ///
  GiGaBase();                              ///< no default 
  GiGaBase           ( const GiGaBase& );  ///< no copy 
  GiGaBase& operator=( const GiGaBase& );  ///< no assignment 
  ///
private:
  ///
  /// name of GiGa Service   
  std::string           m_gigaName   ; 
  /// name of GiGa SetUp Service   
  std::string           m_setupName  ; 
  /// name of Chrono & Stat service
  std::string           m_chronoName ;
  /// name of Event Data Provider Service  
  std::string           m_evtName    ; 
  /// name of Detector Data Provider Service  
  std::string           m_detName    ; 
  /// name of Incident Service 
  std::string           m_incName    ; 
  /// name of Object Manager 
  std::string           m_omName     ;
  /// pointer to GiGa Service 
  IGiGaSvc*             m_gigaSvc    ;
  /// pointer to GiGa SetUp Service 
  IGiGaSetUpSvc*        m_setupSvc   ;
  /// pointer to Chrono& Stat  Service 
  IChronoStatSvc*       m_chronoSvc  ; 
  /// pointer to Event Data  Service 
  IDataProviderSvc*     m_evtSvc     ; 
  /// pointer to Detector Data  Service 
  IDataProviderSvc*     m_detSvc     ; 
  /// pointer to Incident  Service 
  IIncidentSvc*         m_incSvc     ; 
  /// pointer to Object Manager  
  IObjManager*          m_objMgr     ;
  ///
  typedef std::map<std::string,unsigned int> Counter;
  /// counter of errors 
  mutable Counter m_errors     ;
  /// counter of warning 
  mutable Counter m_warnings   ; 
  /// counter of exceptions
  mutable Counter m_exceptions ;
};
// ============================================================================

// ============================================================================
/** assertion 
 *  @param assertion   assertion condition
 *  @param msg         assertion message 
 *  @param sc          assertion status code 
 */
// ============================================================================
inline StatusCode  GiGaBase::Assert
( bool               assertion , 
  const std::string& msg       , 
  const StatusCode&  sc        ) const 
{  
  StatusCode status = StatusCode::SUCCESS ;
  return (assertion) ? status : Exception( msg , MSG::FATAL , sc ) ;
};
// ============================================================================

// ============================================================================
/** assertion 
 *  @param assertion   assertion condition
 *  @param msg         assertion message 
 *  @param sc          assertion status code 
 */
// ============================================================================
inline StatusCode  GiGaBase::Assert
( bool               assertion , 
  const char*        msg       , 
  const StatusCode&  sc        ) const 
{ 
  StatusCode status = StatusCode::SUCCESS ;
  return (assertion) ? status : Exception( msg , MSG::FATAL , sc ) ;
};
// ============================================================================

// ============================================================================
#endif ///<   GIGA_GIGABASE_H
// ============================================================================












