// $Id: CaloAlgorithm.h,v 1.13 2002-04-27 14:38:18 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.12  2002/04/27 10:55:33  ibelyaev
//  update in template accessors for location of 'tools'
//
// ============================================================================
#ifndef CALOKERNEL_CALOALGORITHM_H 
#define CALOKERNEL_CALOALGORITHM_H 1
// ============================================================================
// Include files
// from STL
#include <string>
#include <map>
#include <algorithm>
// from Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/System.h"
// From LHCbKernel 
#include "Kernel/CaloMap.h"
// From CaloKernel 
#include "CaloKernel/CaloException.h"

template<class TYPE>
class         AlgFactory     ;

/** @class  CaloAlgorithm CaloAlgorithm.h CaloKernel/CaloAlgorithm.h
 *   
 *  The useful base class for calo algorithm.
 *  small externsion of ordinary algorithm base class
 *  Extensions:
 *  useful protected print methods, exceptions and assertions
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   30/06/2001
 */

class CaloAlgorithm : public Algorithm 
{
public:
  
  /** standard initialization method
   *  @see  Algorithm
   *  @see IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode initialize(); 
  
  /** standard execution method
   *  @see  Algorithm
   *  @see IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode execute   (); 
  
  /** standard finalization method
   *  @see  Algorithm
   *  @see IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode finalize  (); 
  
protected:
  
  /** Standard constructor (protected)
   *  @see  Algorithm
   *  @param name           name of the algorithm
   *  @param pSvcLocator    poinetr to Service Locator 
   */
  CaloAlgorithm
  ( const std::string& name        , 
    ISvcLocator*       pSvcLocator );
  
  /// destructor, virtual and protected 
  virtual ~CaloAlgorithm();
  
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
   *  @exception CaloException for Invalid Data Provider Service 
   *  @exception CaloException for invalid/unavailable  data  
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
    Print( " The data of type '"      + type                + 
           "' from address '"         + location            + 
           "' are retrieved from TS " , StatusCode::SUCCESS , MSG::DEBUG ) ;
    // return located valid data 
    return aux ;
  };
  
  /** @brief put results into Gaudi Event Transient Store 
   * 
   * 'Output' results: 
   *  register the output object in Gaudi Event Transient Store 
   *
   *  @see IDataProviderSvc
   *  @param object object to be registered 
   *  @param address address in Gaudi Event Transient Store 
   *           ("/Event" could be omitted )
   *  @exception CaloException for invalid event data service 
   *  @exception CaloException for invalid object 
   *  @exception CaloException for error result from event data service 
   *  @return status code 
   */
  StatusCode 
  put
  ( DataObject*        object  , 
    const std::string& address ) const ;
  
  /** the useful method for location of tools. 
   *  for empty "name" delegates to another method  
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
    // add the reference 
    Tool -> addRef();                               
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
        return tool( std::string( type , 0       , pos ) , // new type 
                     std::string( type , pos + 1       ) , // new name 
                     Tool , parent , create            ) ;
      }
    // retrieve the tool from Tool Service 
    StatusCode sc = toolSvc () 
      -> retrieveTool ( type , Tool, parent , create   );
    Assert( sc.isSuccess() , "Could not retrieve Tool'" + type + "'" , sc ) ;
    Assert( 0 != Tool      , "Could not retrieve Tool'" + type + "'"      ) ;
    // add the reference 
    Tool -> addRef();                               
    // debug printout 
    Print( " The Tool of type '" + Tool->type() + 
           "'/'"                 + Tool->name() + 
           "' is retrieved from IToolSvc " , sc , MSG::DEBUG ) ;
    // return located tool 
    return Tool ;
  };
  
protected: ///< "technical" methods 
  
  /** Print the error  message, return status code
   *  and perform the statistics of error messages 
   *  @see MsgStream
   *  @see IMessageSvc 
   *  @see StatusCode 
   *  @param msg    error message 
   *  @param st     status code 
   *  @return       status code 
   */
  StatusCode 
  Error     
  ( const std::string& msg , 
    const StatusCode & st  = StatusCode::FAILURE ) const ;
  
  /** Print the warning  message, return status code 
   *  and perform the statistics of warning  messages 
   *  @see MsgStream
   *  @see IMessageSvc 
   *  @see StatusCode 
   *  @param msg    warning message 
   *  @param st     statsu code 
   *  @return       status code 
   */
  StatusCode 
  Warning   
  ( const std::string& msg , 
    const StatusCode & st  = StatusCode::FAILURE ) const ; 
  
  /** Print the message and return status code 
   *  @see MsgStream
   *  @see IMessageSvc 
   *  @see StatusCode 
   *  @param msg    warning message 
   *  @param st     status code 
   *  @param lev    print level 
   *  @return       status code 
   */
  StatusCode Print     
  ( const std::string& msg , 
    const StatusCode & st  = StatusCode::FAILURE ,
    const MSG::Level & lev = MSG::INFO           ) const ;
  
  /** Assertion - throw exception, if condition is not fulfilled 
   *  @see CaloException
   *  @see GaudiException
   *  @exception CaloException for invalid condifition 
   *  @param ok           condition which should be "true"
   *  @param mesage       message to be associated with the exception 
   *  @param sc           status code to be returned (artificial) 
   *  @return             status code        
   */ 
  inline 
  StatusCode
  Assert 
  ( const bool         ok                            , 
    const std::string& message = ""                  , 
    const StatusCode&  sc      = StatusCode::FAILURE ) const;
  
  /** Assertion - throw exception, if condition is not fulfilled 
   *  @see CaloException
   *  @see GaudiException
   *  @exception CaloException for invalid condition
   *  @param ok           condition which shoudl be "true"
   *  @param mesage       message to eb associated with the exception 
   *  @param sc           status code to be returned (artificial) 
   *  @return             status code        
   */ 
  inline 
  StatusCode 
  Assert 
  ( const bool         ok                            , 
    const char*        message                       ,
    const StatusCode&  sc      = StatusCode::FAILURE ) const;
  
  /** Create and (re)-throw the exception  
   *  @see CaloException
   *  @see GaudiException
   *  @exception CaloException always!
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
   *  @see CaloException
   *  @see GaudiException
   *  @exception CaloException always!
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
   *  @see CaloException
   *  @see GaudiException
   *  @exception CaloException always!
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
  
  /// address/location/name in Transient Store of input  data container 
  const std::string& inputData  () const { return m_inputData  ; }
  
  /// address/location/name in Transient Store of output data container 
  const std::string& outputData () const { return m_outputData ; }
  
  /// address/location/name in Transient Store of detector data  
  const std::string& detData    () const { return m_detData    ; }
  
  /** set address/location/name in Transient Store of input  data container 
   *  @param  addr address/location/name in Transient Store of input data 
   */
  void setInputData ( const std::string& addr ) { m_inputData  = addr; }
  
  /** set address/location/name in Transient Store of output  data container 
   *  @param  addr address/location/name in Transient Store of output data 
   */
  void setOutputData( const std::string& addr ) { m_outputData = addr; }  
  
  /** set address/location/name in Transient Store of detector data  
   *  @param  addr address/location/name in Transient Store of detector 
   */
  void setDetData   ( const std::string& addr ) { m_inputData  = addr; }
  
private:
  
  /// no default constructor 
  CaloAlgorithm();
  /// no copy constructor 
  CaloAlgorithm( const CaloAlgorithm& );
  /// no assignment oprtator  
  CaloAlgorithm& operator=( const CaloAlgorithm& );
  
private:
  
  /// address/location/name in Transient Store of input  data container 
  std::string      m_inputData  ;
  /// address/location/name in Transient Store of output data container 
  std::string      m_outputData ;
  /// address/location/name in Transient Store of detector data  
  std::string      m_detData    ;
  
  typedef CaloMap<std::string,unsigned int> Counter;
  /// counter of error
  mutable Counter m_errors   ;
  /// counter of error
  mutable Counter m_warnings ;
  
};

// ============================================================================
/** Assertion - throw exception, if condition is not fulfilled 
 *  @param OK           condition which should be "true"
 *  @param mesage       message to eb associated with the exception 
 *  @param sc           status code to be returned (artificial) 
 *  @return             status code        
 */ 
// ============================================================================
inline 
StatusCode 
CaloAlgorithm::Assert 
( const bool         OK  , 
  const std::string& msg , 
  const StatusCode&  sc  ) const
{
  StatusCode ok ( StatusCode::SUCCESS );
  return OK ? ok : Exception( msg , MSG::FATAL , sc ) ; 
};

// ============================================================================
/** Assertion - throw exception, if condition is not fulfilled 
 *  @param OK           condition which should be "true"
 *  @param mesage       message to eb associated with the exception 
 *  @param sc           status code to be returned (artificial) 
 *  @return             status code        
 */ 
// ============================================================================
inline 
StatusCode
CaloAlgorithm::Assert 
( const bool        OK  , 
  const char*       msg ,
  const StatusCode& sc  ) const
{ 
  StatusCode ok ( StatusCode::SUCCESS ) ;
  return OK ? ok : Exception( msg , MSG::FATAL , sc ) ; 
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOALGORITHM_H 
// ============================================================================

