// $Id: CaloTool.h,v 1.2 2002-04-01 11:00:36 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2001/11/25 14:07:38  ibelyaev
// New Package: substitution of the  previous CaloGen package
//
// Revision 1.4  2001/11/12 19:01:02  ibelyaev
//  minor  reorganization of header files
//
// ============================================================================
#ifndef CALOKERNEL_CALOTOOL_H 
#define CALOKERNEL_CALOTOOL_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
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
   *  @return status code 
   */
  virtual StatusCode    initialize ();
  
  /** standard finalization method 
   *  @return status code 
   */
  virtual StatusCode    finalize   ();
  
  /** query interafce method  
   *  @param  id  unique interface ID 
   *  @param  pI  placeholder for interface 
   *  @return status code 
   */
  virtual StatusCode queryInterface ( const InterfaceID& id ,
                                      void**             pI );
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
   *  @param svc pointer to data service (data provider)
   *  @param location data location/address in Gaudi Transient Store 
   */
  template<class TYPE>
  TYPE* get( IDataProviderSvc* svc , const std::string& location )
  {
    if( 0 == svc ) 
      { Error("get<>(): IDataProviderSvc* points to NULL!"   ) ; return 0 ; }
    SmartDataPtr<TYPE> object( svc, location ) ;
    if( !object )
      { Error("get<>(): No valid data at '" + location + "'" ) ; return 0 ; }
    ///
    return object ;
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
  
  /** accessot to detector service 
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
   *  @param sc           status code to be returned 
   *  @return             status code        
   */
  inline StatusCode 
  Assert ( const bool         ok                            , 
           const std::string& message = " "                 ,
           const StatusCode&  sc      = StatusCode::FAILURE ) const;
  
  /** Assertion - throw exception, if condition is not fulfilled 
   *  @param ok            condition which shoudl be "true"
   *  @param mesage       message to be associated with the exception 
   *  @param sc           status code to be returned 
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
  
  IChronoStatSvc*        m_chronoSvc ;
  IToolSvc*              m_toolSvc   ;
  ISvcLocator*           m_svcLoc    ;
  IDataProviderSvc*      m_detSvc    ;
  
  const DeCalorimeter*   m_det       ;
  std::string            m_detName   ;
  
};


// ============================================================================
/** Assertion - throw exception, if condition is not fulfilled 
 *  @param OK           condition which should be "true"
 *  @param mesage       message to be associated with the exception 
 *  @param sc           status code to be returned 
 *  @return             status code        
 */ 
// ============================================================================
inline StatusCode 
CaloTool::Assert ( const bool OK          , 
                   const std::string& msg ,
                   const StatusCode&  sc  ) const
{
  StatusCode ok ( StatusCode::SUCCESS );
  return OK ? ok : Exception( msg , MSG::FATAL , sc ) ; 
};

// ============================================================================
/** Assertion - throw exception, if condition is not fulfilled 
 *  @param OK           condition which should be "true"
 *  @param mesage       message to be associated with the exception 
 *  @param sc           status code to be returned 
 *  @return             status code        
 */ 
// ============================================================================
inline StatusCode
CaloTool::Assert ( const bool OK          , 
                   const char*        msg ,
                   const StatusCode&  sc  ) const
{ 
  StatusCode ok ( StatusCode::SUCCESS ) ;
  return OK ? ok : Exception( msg , MSG::FATAL , sc  ) ; 
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOKERNEL_CALOTOOL_H
// ============================================================================
