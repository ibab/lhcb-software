// $Id: CaloAlgorithm.h,v 1.4 2002-04-01 12:50:24 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/04/01 11:00:35  ibelyaev
// enrich CaloAlgorithm,CaloTool,CaloMap and CaloHashMap interafces
//
// Revision 1.2  2002/03/18 18:16:20  ibelyaev
//  small update for LHCbKernel package
//
// Revision 1.1.1.1  2001/11/25 14:07:38  ibelyaev
// New Package: substitution of the  previous CaloGen package
//
// Revision 1.5  2001/11/13 09:34:41  ibelyaev
//  add accessor to IToolSvc interface in CaloAlgorithm base class
//
// Revision 1.4  2001/11/12 19:01:02  ibelyaev
//  minor  reorganization of header files
//
// ============================================================================
#ifndef CALOKERNEL_CALOALGORITHM_H 
#define CALOKERNEL_CALOALGORITHM_H 1
// ============================================================================
// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
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
   *  @return status code 
   */
  virtual StatusCode initialize(); 
  
  /** standard execution method
   *  @return status code 
   */
  virtual StatusCode execute   (); 
  
  /** standard finalization method
   *  @return status code 
   */
  virtual StatusCode finalize  (); 
  
protected:
  
  /** Standard constructor (protected)
   *  @param name           name of the algorithm
   *  @param pSvcLocator    poinetr to Service Locator 
   */
  CaloAlgorithm( const std::string& name        , 
                 ISvcLocator*       pSvcLocator );
  
  /** destructor, virtual and protected 
   */  
  virtual ~CaloAlgorithm();
  
protected:
  
  /** templated  access to the data in Gaudi transient store 
   *  
   *  Usage:
   *
   *  - 
   *  MCCaloDigits* digits = get<MCCaloDigits>( evtSvc() , inputData() );
   *  if( 0 == digits ) { return StatusCode::FAILURE ;}
   *
   *  -
   *  const DeCalorimeter* det = 
   *      get<const DeCalorimeter>( detSvc() , detData() );
   *  if( 0 == det ) { return StatusCode::FAILURE ;}
   *
   *  @exception CaloException for Invalid Data Provider Service 
   *  @exception CaloException for invalid/unavailable  data  
   *  @param svc pointer to data service (data provider)
   *  @param location data location/address in Gaudi Transient Store 
   */
  template<class TYPE>
  TYPE* get( IDataProviderSvc* svc , const std::string& location ) const 
  {
    Assert( 0 != svc , " get<>():: IDataProvider* points to NULL!"       );
    SmartDataPtr<TYPE> object( svc, location ) ;
    Assert(  object  ,  " get<>():: No valid data at '" + location + "'" );
    ///
    return object ;
  };
  
  /** @brief put results into Gaudi Event Transient Store 
   * 
   * 'Output' results: 
   *  register the output object in Gaudi Event Transient Store 
   *  @param object object to be registered 
   *  @param address address in Gaudi Event Transient Store 
   *           ("/Event" could be omitted )
   *  @exception CaloException for invalid event data service 
   *  @exception CaloException for invalid object 
   *  @exception CaloException for error result from event data service 
   *  @return status code 
   */
  StatusCode put
  ( DataObject*        object  , 
    const std::string& address ) const ;
  
  /** the useful method for location of tools 
   *  @exception CaloException for invalid Tool Service 
   *  @exception CaloException for error from Tool Service 
   *  @exception CaloException for invalid tool 
   *  @param type   tool type 
   *  @param name   tool name
   *  @param tool   tool itself 
   *  @param parent tool parent
   *  @param createIf flag for creation of nonexisting tools 
   *  @return pointer to the tool
   */
  template<class TOOL>
  TOOL* tool( const std::string& type            , 
              const std::string& name            , 
              TOOL*&             tool            , 
              const IInterface*  parent   = 0    , 
              bool               createIf = true ) 
  {
    Assert( 0 != toolSvc() , "IToolSvc* points toNULL!" );
    StatusCode sc = toolSvc () 
      -> retrieveTool ( type , name , tool, parent , createIf );
    Assert( sc.isSuccess() , 
            "Could not retrieve Tool'" + type + "'/'" + name + "'", sc ) ;
    Assert( 0 != tool      , 
            "Could not retrieve Tool'" + type + "'/'" + name + "'"     ) ;
    ///
    return tool ;
  };
  
  /** the useful method for location of tools 
   *  @exception CaloException for invalid Tool Service 
   *  @exception CaloException for error from Tool Service 
   *  @exception CaloException for invalid tool 
   *  @param type   tool type 
   *  @param tool   tool itself 
   *  @param parent tool parent
   *  @param createIf flag for creation of nonexisting tools 
   *  @return pointer to the tool
   */
  template<class TOOL>
  TOOL* tool( const std::string& type            , 
              TOOL*&             tool            , 
              const IInterface*  parent   = 0    , 
              bool               createIf = true ) 
  {
    Assert( 0 != toolSvc() , "IToolSvc* points toNULL!" );
    StatusCode sc = toolSvc () 
      -> retrieveTool ( type , tool, parent , createIf );
    Assert( sc.isSuccess() , 
            "Could not retrieve Tool'" + type + "'" , sc ) ;
    Assert( 0 != tool      , 
            "Could not retrieve Tool'" + type + "'"     ) ;
    ///
    return tool ;
  };

protected: ///< "technical" methods 
  
  /** Print the error  message, return status code
   *  and perform the statistics of error messages 
   *  @param msg    error message 
   *  @param st     status code 
   *  @return       status code 
   */
  StatusCode 
  Error     ( const std::string& msg , 
              const StatusCode & st  = StatusCode::FAILURE ) const ;
  
  /** Print the warning  message, return status code 
   *  and perform the statistics of warning  messages 
   *  @param msg    warning message 
   *  @param st     statsu code 
   *  @return       status code 
   */
  StatusCode 
  Warning   ( const std::string& msg , 
              const StatusCode & st  = StatusCode::FAILURE ) const ; 
  
  /** Print the message and return status code 
   *  @param msg    warning message 
   *  @param st     status code 
   *  @param lev    print level 
   *  @return       status code 
   */
  StatusCode 
  Print     ( const std::string& msg , 
              const StatusCode & st  = StatusCode::FAILURE ,
              const MSG::Level & lev = MSG::INFO           ) const ;
  
  /** Assertion - throw exception, if condition is not fulfilled 
   *  @param ok            condition which should be "true"
   *  @param mesage       message to be associated with the exception 
   *  @param sc           status code to be returned (artificial) 
   *  @return             status code        
   */ 
  inline StatusCode 
  Assert ( const bool         ok                            , 
           const std::string& message = ""                  , 
           const StatusCode&  sc      = StatusCode::FAILURE ) const;
  
  /** Assertion - throw exception, if condition is not fulfilled 
   *  @param ok            condition which shoudl be "true"
   *  @param mesage       message to eb associated with the exception 
   *  @param sc           status code to be returned (artificial) 
   *  @return             status code        
   */ 
  inline StatusCode
  Assert ( const bool         ok                            , 
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
  Exception ( const std::string    & msg                        ,  
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
  Exception ( const std::string    & msg                        ,  
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
  Exception ( const std::string& msg = "no message"        ,  
              const MSG::Level & lvl = MSG::FATAL          ,
              const StatusCode & sc  = StatusCode::FAILURE ) const ;
  
  /** address/location/name in Transient Store of input  data container 
   */
  const std::string& inputData  () const { return m_inputData  ; }
  
  /** address/location/name in Transient Store of output data container 
   */
  const std::string& outputData () const { return m_outputData ; }
  
  /** address/location/name in Transient Store of detector data  
   */
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

};

// ============================================================================
/** Assertion - throw exception, if condition is not fulfilled 
 *  @param OK           condition which should be "true"
 *  @param mesage       message to eb associated with the exception 
 *  @param sc           status code to be returned (artificial) 
 *  @return             status code        
 */ 
// ============================================================================
inline StatusCode 
CaloAlgorithm::Assert ( const bool         OK  , 
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
inline StatusCode
CaloAlgorithm::Assert ( const bool        OK  , 
                        const char*       msg ,
                        const StatusCode& sc  ) const
{ 
  StatusCode ok ( StatusCode::SUCCESS ) ;
  return OK ? ok : Exception( msg , MSG::FATAL , sc ) ; 
};


// ============================================================================
#endif // CALOALGORITHM_H 
// ============================================================================

