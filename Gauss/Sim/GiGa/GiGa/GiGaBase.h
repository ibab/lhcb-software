// $Id: GiGaBase.h,v 1.17 2002-12-07 14:27:50 ibelyaev Exp $
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
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ISerialize.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/SmartDataPtr.h"
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
public:
  
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

protected:
  
  /** @brief templated  access to the data in Gaudi transient store 
   * 
   *  Quick and safe access to the data in Gaudi transient store.
   *  The method located the data at given address and perform the 
   *  debug printout about located data
   *    
   *  Usage:
   *
   *  - 
   *  MCCaloDigits*        digits = get( evtSvc () , inputData () , digits );
   *  if( 0 == digits ) { return StatusCode::FAILURE ;}
   *
   *  -
   *  const DeCalorimeter* det    = get( detSvc () , detData   () , det    );
   *  if( 0 == det ) { return StatusCode::FAILURE ;}
   *
   *  @warning the third argument is artificial to please MicroSoft
   *           stupid compiler!
   *
   *  @see IDataProviderSvc
   *  @see SmartDataPtr
   *  @exception GiGaException for Invalid Data Provider Service 
   *  @exception GiGaException for invalid/unavailable  data  
   *  @param svc pointer to data service (data provider)
   *  @param location data location/address in Gaudi Transient Store 
   *  @param type artificial algument (to please MicroSoft compiler)
   */
  template<class TYPE>
  TYPE* 
  get
  ( IDataProviderSvc*  svc        , 
    const std::string& location   ,
    const TYPE*        /* type */ ) const 
  {
    // check the environment 
    Assert( 0 != svc   , "  get():: IDataProvider* points to NULL!"      );
    SmartDataPtr<TYPE> object( svc, location ) ;
    Assert( !(!object) ,  " get():: No valid data at '" + location + "'" );
    TYPE* aux = object ;
    Assert( 0 != aux   ,  " get():: No valid data at '" + location + "'" );
    const std::string type( System::typeinfoName( typeid( *aux ) ) );
    // debug printout 
    // return located valid data 
    Print( " The data from address '"     + location     + 
           "' are retrieved from TS "                    , 
           StatusCode::SUCCESS            , MSG::DEBUG   ) ;
    Print( " [ The actual data type is '" + type         + 
           "' ] "                                        ,
           StatusCode::SUCCESS            , MSG::VERBOSE ) ;
    return aux ;
  };
  
  /** the useful method for location of tools. 
   *  for empty "name" delegates to another method  
   *  @see IToolSvc
   *  @see IAlgTool
   *  @attention do not forget to 'release' tool after the usage!
   *  @exception GiGaException for invalid Tool Service 
   *  @exception GiGaException for error from Tool Service 
   *  @exception GiGaException for invalid tool 
   *  @param type   tool type 
   *  @param name   tool name
   *  @param Tool   tool itself (return) 
   *  @param parent tool parent
   *  @param create flag for creation of nonexisting tools 
   *  @return pointer to the tool
   */
  template<class TOOL>
  TOOL* 
  tool
  ( const std::string& type           , 
    const std::string& name           , 
    TOOL*&             Tool           , 
    const IInterface*  parent  = 0    , 
    bool               create  = true ) const 
  {
    // for empty names delegate to another method 
    if( name.empty() ) { return tool( type , Tool , parent , create ) ; }
    Assert( 0 != toolSvc() , "IToolSvc* points toNULL!" );
    // get the tool from Tool Service 
    StatusCode sc = toolSvc () 
      -> retrieveTool ( type , name , Tool, parent , create );
    Assert( sc.isSuccess() , 
            "Could not retrieve Tool'" + type + "'/'" + name + "'", sc ) ;
    Assert( 0 != Tool      , 
            "Could not retrieve Tool'" + type + "'/'" + name + "'"     ) ;
    // debug printout 
    Print( " The Tool of type '" + Tool->type() + 
           "'/'"                 + Tool->name() + 
           "' is retrieved from IToolSvc " , sc , MSG::DEBUG ) ;
    // return located tool 
    return Tool ;
  };
  
  /** the useful method for location of tools.
   *  @see IToolSvc
   *  @see IAlgTool
   *  @attention do not forget to 'release' tool after the usage!
   *  @exception GiGaException for invalid Tool Service 
   *  @exception GiGaException for error from Tool Service 
   *  @exception GiGaException for invalid tool 
   *  @param type   tool type, could be of "Type/Name" format 
   *  @param Tool   tool itself (return)
   *  @param parent tool parent
   *  @param create flag for creation of nonexisting tools 
   *  @return pointer to the tool
   */
  template<class TOOL>
  TOOL* 
  tool
  ( const std::string& type          , 
    TOOL*&             Tool          , 
    const IInterface*  parent = 0    , 
    bool               create = true ) const
  {
    // check the environment 
    Assert( 0 != toolSvc() , "IToolSvc* points toNULL!" );
    // "type" or "type/name" ?
    std::string::const_iterator it = 
      std::find( type.begin() , type.end () , '/' );
    // "type" is compound!
    if( type.end() != it ) 
      {
        std::string::size_type pos = it - type.begin()   ;
        const std::string ntype( type , 0       , pos               );
        const std::string nname( type , pos + 1 , std::string::npos );
        return tool( ntype , // new type 
                     nname , // new name 
                     Tool , parent , create            ) ;
      }
    // retrieve the tool from Tool Service 
    StatusCode sc = toolSvc () 
      -> retrieveTool ( type , Tool, parent , create   );
    Assert( sc.isSuccess() , "Could not retrieve Tool'" + type + "'" , sc ) ;
    Assert( 0 != Tool      , "Could not retrieve Tool'" + type + "'"      ) ;
    // debug printout 
    Print( " The Tool of type '" + Tool->type() + 
           "'/'"                 + Tool->name() + 
           "' is retrieved from IToolSvc " ,  sc  , MSG::DEBUG ) ;
    // return located tool 
    return Tool ;
  };


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
  ///
  typedef std::map<std::string,unsigned int> Counter;
  /// counter of errors 
  mutable Counter       m_errors     ;
  /// counter of warning 
  mutable Counter       m_warnings   ; 
  /// counter of exceptions
  mutable Counter       m_exceptions ;
  ///
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












