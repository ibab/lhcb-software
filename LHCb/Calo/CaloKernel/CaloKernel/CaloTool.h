// $Id: CaloTool.h,v 1.13 2002-11-13 20:36:50 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.12  2002/05/07 17:21:35  ibelyaev
//  bug fix for Win32
//
// ============================================================================
#ifndef CALOKERNEL_CALOTOOL_H 
#define CALOKERNEL_CALOTOOL_H 1
// Include files
// STD & STL 
#include <string>
#include <algorithm>
// GaudiKernel
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
// LHCbKernel 
#include "Kernel/CaloMap.h"
// CaloKernel 
#include "CaloKernel/CaloException.h"

// forward declarations 
template <class TYPE>
class ToolFactory      ;
class IChronoStatSvc   ;
class IToolSvc         ;
class DeCalorimeter    ;
class IDataProviderSvc ;

/** @class CaloTool CaloTool.h CaloKernel/CaloTool.h
 *   
 *  Useful base class for all Calorimeter tools  
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   01/11/2001
 */

class CaloTool: public AlgTool
{  
public:  
  
  /** standard initialization method 
   *  @see  AlgTool 
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode    initialize ();
  
  /** standard finalization method 
   *  @see  AlgTool 
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode    finalize   ();
  
  /** query interface method  
   *  @see  AlgTool 
   *  @see IAlgTool
   *  @see IInterface
   *  @param  id  unique interface ID 
   *  @param  pI  placeholder for interface 
   *  @return status code 
   */
  virtual StatusCode queryInterface 
  ( const InterfaceID& id ,
    void**             pI );

protected:
  
  /** Standard constructor
   *  @see AlgTool
   *  @param type tool type (useless)
   *  @param name tool name
   *  @param parent pointer to parent object (service, algorithm or tool)  
   */
  CaloTool
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ); 
  
  /// destructor, virtual and protected 
  virtual ~CaloTool();
  
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
   *  @warning the third argument is due to stupid MicroSoft compiler, 
   *           only the type of the third argument is used
   *
   *  @see IDataProviderSvc
   *  @see SmartDataPtr
   *  @exception CaloException for Invalid Data Provider Service 
   *  @exception CaloException for invalid/unavailable  data  
   *  @param svc pointer to data service (data provider)
   *  @param location data location/address in Gaudi Transient Store
   *  @param type artificial argument (due to stupid MicroSoft compiler)
   */
  template<class TYPE>
  TYPE* 
  get
  ( IDataProviderSvc*  svc        , 
    const std::string& location   , 
    const TYPE*        /* type */ ) const
  {
    Assert( 0 != svc    , " get():: IDataProvider* points to NULL!"      );
    SmartDataPtr<TYPE> object( svc, location ) ;
    Assert( !(!object)  , " get():: No Valid data at '" + location + "'" );
    TYPE* aux = object ;
    Assert( 0 != aux    , " get():: No valid data at '" + location + "'" ); 
    const std::string type( System::typeinfoName( typeid( *aux ) ) );
    // debug printout 
    Print( " The data from address '"     + location + 
           "' are retrieved from TS "     , 
           StatusCode::SUCCESS            , MSG::DEBUG   ) ;
    Print( " [ The actual data type is '" + type     + 
           "' ] "                         , 
           StatusCode::SUCCESS            , MSG::VERBOSE ) ;
    return aux ;
  };
  
  /** the useful method for location of tools 
   *  @see IToolSvc
   *  @see IAlgTool
   *  @attention do not forget to 'release' tool after the usage!
   *  @exception CaloException for invalid Tool Service 
   *  @exception CaloException for error from Tool Service 
   *  @exception CaloException for invalid tool 
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
  ( const std::string& type          , 
    const std::string& name          , 
    TOOL*&             Tool          , 
    const IInterface*  parent = 0    , 
    bool               create = true ) const
  {
    // for empty names delegate to another method 
    if( name.empty() ) { return tool( type , Tool , parent , create ) ; }
    Assert( 0 != toolSvc() , "IToolSvc* points toNULL!" );
    // retrieve the tool from Tool Service 
    StatusCode sc = toolSvc () 
      -> retrieveTool ( type , name , Tool, parent , create );
    Assert( sc.isSuccess() , 
            "Could not retrieve Tool'" + type + "'/'" + name + "'", sc ) ;
    Assert( 0 != Tool      , 
            "Could not retrieve Tool'" + type + "'/'" + name + "'"     ) ;
    // add the reference 
    // Tool -> addRef();                               
    // debug printout 
    Print( " The Tool of type '" + Tool->type() + 
           "'/'"                 + Tool->name() + 
           "' is retrieved from IToolSvc " , sc , MSG::DEBUG ) ;
    // return the located tool 
    return Tool ;
  };
  
  /** the useful method for location of tools 
   *  @see IToolSvc
   *  @see IAlgTool
   *  @attention do not forget to 'release' tool after the usage!
   *  @exception CaloException for invalid Tool Service 
   *  @exception CaloException for error from Tool Service 
   *  @exception CaloException for invalid tool 
   *  @param type   tool type, could be of "Type/Name" format 
   *  @param Tool   tool itself (return)
   *  @param parent tool parent
   *  @param create flag for creation of nonexisting tools 
   *  @return pointer to the tool
   */
  template<class TOOL>
  TOOL* 
  tool
  ( const std::string& type           , 
    TOOL*&             Tool           , 
    const IInterface*  parent  = 0    , 
    bool               create  = true ) const 
  {
    // check the environment 
    Assert( 0 != toolSvc() , "IToolSvc* points toNULL!" );
    // "type" or "type/name" ?
    std::string::const_iterator it = 
      std::find( type.begin() , type.end () , '/' );
    // "type" is compound!
    if( type.end() != it ) 
      {
        const std::string::size_type pos = it - type.begin() ;
        const std::string ntype( type , 0       , pos               );
        const std::string nname( type , pos + 1 , std::string::npos );
        return tool( ntype , // new type 
                     nname , // new name 
                     Tool  , parent , create            ) ;
      }
    StatusCode sc = toolSvc () 
      -> retrieveTool ( type , Tool, parent , create   );
    Assert( sc.isSuccess() , "Could not retrieve Tool'" + type + "'" , sc ) ;
    Assert( 0 != Tool      , "Could not retrieve Tool'" + type + "'"     ) ;
    // add the reference 
    // Tool -> addRef(); 
    // debug printout 
    Print( " The Tool of type '" + Tool->type() + 
           "'/'"                 + Tool->name() + 
           "' is retrieved from IToolSvc " , sc , MSG::DEBUG ) ;
    // return located tool 
    return Tool ;
  };

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
  
  /** accessor to detector service 
   *  @return pointer to detector service 
   */
  inline IDataProviderSvc*     detSvc   () const { return m_detSvc ; }

  /** accessor to associated calorimeter detector  (const version)
   *  @return pointer to associated calorimeter detector 
   */
  inline const DeCalorimeter* det       () const { return m_det ;}
  
  /** set new detector 
   *  @param Det pointer to new calorimeter detector
   */
  inline const DeCalorimeter* setDet ( const DeCalorimeter* Det )
  { m_det = Det ; return det() ; }

  /** accessor to detector name 
   *  @return name of the detector
   */
  inline const std::string&   detName() const { return m_detName; }
  
  /** set new name of the detector 
   *  @param newV new detector name
   */
  inline const std::string&   setDetName( const std::string& newV ) 
  { m_detName = newV ; return detName() ; }
  
protected:
  
  /** Print the error  message, return status code
   *  and perform the statistics of error messages 
   *  @param msg    error message 
   *  @param st     status code  
   *  @param mx     maximal number of printout
  *  @return       status code 
   */
  StatusCode 
  Error     
  ( const std::string& msg                       , 
    const StatusCode & st  = StatusCode::FAILURE ,
    const size_t       mx  = 10                  ) const ;
  
  /** Print the warning  message, return status code 
   *  and perform the statistics of warning  messages 
   *  @param msg    warning message 
   *  @param st     statsu code 
   *  @param mx     maximal number of printout
   *  @return       status code 
   */
  StatusCode 
  Warning   
  ( const std::string& msg , 
    const StatusCode & st  = StatusCode::FAILURE ,
    const size_t       mx  = 10                  ) const ;
  
  /** Print the message and return status code 
   *  @param msg    warning message 
   *  @param st     status code 
   *  @param lev    print level 
   *  @return       status code 
   */
  StatusCode 
  Print 
  ( const std::string& msg , 
    const StatusCode & st  = StatusCode::FAILURE ,
    const MSG::Level & lev = MSG::INFO           ) const ;
  
  /** Assertion - throw exception, if condition is not fulfilled 
   *  @param ok            condition which should be "true"
   *  @param mesage       message to be associated with the exception 
   *  @param sc           status code to be returned 
   *  @return             status code        
   */
  inline StatusCode 
  Assert 
  ( const bool         ok                            , 
    const std::string& message = " "                 ,
    const StatusCode&  sc      = StatusCode::FAILURE ) const;
  
  /** Assertion - throw exception, if condition is not fulfilled 
   *  @param ok            condition which shoudl be "true"
   *  @param mesage       message to be associated with the exception 
   *  @param sc           status code to be returned 
   *  @return             status code        
  */
  inline StatusCode
  Assert
  ( const bool         ok                            , 
    const char*        message                       ,
    const StatusCode&  sc      = StatusCode::FAILURE ) const;
  
  /** Create and (re)-throw the exception  
   *  @param msg    exception message 
   *  @param exc    (previous) exception of type GaudiException
   *  @param lvl    print level 
   *  @param sc     status code  
   *  @return       status code (fictive) 
   */
  StatusCode 
  Exception
  ( const std::string    & msg                        ,  
    const GaudiException & exc                        , 
    const MSG::Level     & lvl = MSG::FATAL           ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /** Create and (re)-throw the exception  
   *  @param msg    exception message 
   *  @param exc    (previous) exception of type std::exception
   *  @param lvl    print level 
   *  @param sc     status code  
   *  @return       status code (fictive) 
   */
  StatusCode 
  Exception
  ( const std::string    & msg                        ,  
    const std::exception & exc                        , 
    const MSG::Level     & lvl = MSG::FATAL           ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /** Create and throw the exception  
   *  @param msg    exception message 
   *  @param lvl    print level 
   *  @param sc     status code  
   *  @return       status code (fictive) 
   */
  StatusCode 
  Exception 
  ( const std::string& msg = "no message"        ,  
    const MSG::Level & lvl = MSG::FATAL          ,
    const StatusCode & sc  = StatusCode::FAILURE ) const ;
  
private:
  
  /// default constructor  is private!
  CaloTool();
  /// copy constructor     is private!
  CaloTool
  ( const CaloTool& copy );
  /// assignement operator is private!
  CaloTool& operator=
  ( const CaloTool&  );
  
private:
  
  IChronoStatSvc*        m_chronoSvc ;
  IToolSvc*              m_toolSvc   ;
  ISvcLocator*           m_svcLoc    ;
  IDataProviderSvc*      m_detSvc    ;
  
  const DeCalorimeter*   m_det       ;
  std::string            m_detName   ;
  
  typedef CaloMap<std::string,unsigned int> Counter;
  /// counter of error
  mutable Counter m_errors     ;
  /// counter of error
  mutable Counter m_warnings   ;
  /// counter of exceptions
  mutable Counter m_exceptions ;
  
};
// ============================================================================

// ============================================================================
/** Assertion - throw exception, if condition is not fulfilled 
 *  @param OK           condition which should be "true"
 *  @param mesage       message to be associated with the exception 
 *  @param sc           status code to be returned 
 *  @return             status code        
 */ 
// ============================================================================
inline 
StatusCode 
CaloTool::Assert 
( const bool OK          , 
  const std::string& msg ,
  const StatusCode&  sc  ) const
{
  StatusCode ok ( StatusCode::SUCCESS );
  return OK ? ok : Exception( msg , MSG::FATAL , sc ) ; 
};
// ============================================================================

// ============================================================================
/** Assertion - throw exception, if condition is not fulfilled 
 *  @param OK           condition which should be "true"
 *  @param mesage       message to be associated with the exception 
 *  @param sc           status code to be returned 
 *  @return             status code        
 */ 
// ============================================================================
inline 
StatusCode
CaloTool::Assert 
( const bool OK          , 
  const char*        msg ,
  const StatusCode&  sc  ) const
{ 
  StatusCode ok ( StatusCode::SUCCESS ) ;
  return OK ? ok : Exception( msg , MSG::FATAL , sc  ) ; 
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
#endif // CALOKERNEL_CALOTOOL_H
// ============================================================================
