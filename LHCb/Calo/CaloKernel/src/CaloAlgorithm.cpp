// $Id: CaloAlgorithm.cpp,v 1.1.1.1 2001-11-25 14:07:38 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2001/11/13 09:34:42  ibelyaev
//  add accessor to IToolSvc interface in CaloAlgorithm base class
//
// Revision 1.4  2001/11/12 19:01:02  ibelyaev
//  minor  reorganization of header files
//
// ============================================================================
#define  CALOKERNEL_CALOALGORITHM_CPP 1 
// ============================================================================

// Include files
#include <algorithm>
#include <cstdlib> 
// from GaudiKernel
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/ObjectVector.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/SmartRef.h" 
#include "GaudiKernel/IHistogramSvc.h" 
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/MsgStream.h"
// CaloKernel 
#include "CaloKernel/CaloException.h"
#include "CaloKernel/CaloAlgorithm.h"
// local 
 
// ============================================================================
/** @file CaloAlgorithm.cpp
 * 
 *  Implementation file for class : CaloAlgorithm
 * 
 *  @aauthor Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 30/06/2001 
 */
// ============================================================================

// ============================================================================
/** Standard constructor (protected)
 *  @param name           name of the algorithm
 *  @param pSvcLocator    poinetr to Service Locator 
 */
// ============================================================================
CaloAlgorithm::CaloAlgorithm( const std::string&  name       ,
                              ISvcLocator*        pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  /// input/output/detector data
  , m_inputData  ( "" ) ///< no default value
  , m_outputData ( "" ) ///< no default value
  , m_detData    ( "" ) ///< no default value
  /// Tool Service 
  , m_toolSvc    ( 0  ) ///< NULL! 
{
  ///
  declareProperty  ("Input"        , m_inputData  );
  declareProperty  ("Output"       , m_outputData );
  declareProperty  ("Detector"     , m_detData    );
  ///
};

// ============================================================================
/** Destructor
 */
// ============================================================================
CaloAlgorithm::~CaloAlgorithm() {};

// ============================================================================
/** Print the error  message and return status code
 *  @param msg    error message 
 *  @param st     status code 
 *  @return       status code 
 */
// ============================================================================
StatusCode 
CaloAlgorithm::Error     ( const std::string& msg , 
                           const StatusCode & st  ) const 
{
  Stat stat( chronoSvc() , name()+":Error" ); 
  return Print( msg , st , MSG::ERROR ); 
};

// ============================================================================
/** Print the warning  message and return status code 
 *  @param msg    warning message 
 *  @param st     statsu code 
 *  @return       status code 
 */
// ============================================================================
StatusCode 
CaloAlgorithm::Warning   ( const std::string& msg , 
                           const StatusCode & st  ) const 
{
  Stat stat( chronoSvc() , name()+":Warning" ); 
  return Print( msg , st , MSG::WARNING ); 
};

// ============================================================================
/**  Print the message and return status code 
 *   @param msg    warning message 
 *   @param st     status code 
 *   @param lvl    print level 
 *   @return       status code 
 */
// ============================================================================
StatusCode 
CaloAlgorithm::Print     ( const std::string& msg , 
                           const StatusCode & st  ,
                           const MSG::Level & lvl ) const 
{
  MsgStream log( msgSvc() , name() ); 
  log << lvl 
      << System::typeinfoName( typeid( *this ) ) 
      << " "   << msg ;
  ///
  if( !st.isSuccess() ) { log << " \tStatusCode=" << st ;}
  ///
  log << endreq ; 
  return  st;
};

// ============================================================================
/** Create and (re)-throw the exception  
 *  @param msg    exception message 
 *  @param exc    (previous) exception of type GaudiException
 *  @param lvl    print level 
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode CaloAlgorithm::Exception ( const std::string    & msg ,
                                      const GaudiException & exc ,
                                      const MSG::Level     & lvl ,
                                      const StatusCode     & sc  ) const   
{ 
  Error( msg , lvl );
  throw CaloException( msg , exc, sc );
  return  sc ;
};

// ============================================================================
/** Create and (re)-throw the exception  
 *  @param msg    exception message 
 *  @param exc    (previous) exception of type GaudiException
 *  @param lvl    print level 
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode CaloAlgorithm::Exception ( const std::string    & msg ,  
                                      const std::exception & exc , 
                                      const MSG::Level     & lvl ,
                                      const StatusCode     & sc  ) const   
{ 
  Error( msg , lvl );
  throw CaloException( msg+"("+exc.what()+")", sc );
  return  sc ;
};

// ============================================================================
/** Create and throw the exception  
 *  @param msg    exception message 
 *  @param lvl    print level 
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode CaloAlgorithm::Exception ( const std::string    & msg ,  
                                          const MSG::Level     & lvl ,
                                          const StatusCode     & sc  ) const 
{ 
  Error( msg , lvl );
  throw CaloException( msg , sc );
  return  sc ;
};

// ============================================================================
/** standard initialization method
 *  @return status code 
 */
// ============================================================================
StatusCode CaloAlgorithm::initialize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG 
      << " ==> Initialize the base class CaloAlgorithm" << endreq;
  ///
  if( 0 != m_toolSvc ) { toolSvc()->release() ; m_toolSvc = 0 ;}
  StatusCode sc = 
    serviceLocator()->service( "ToolSvc" , m_toolSvc , true ) ;
  if( sc.isFailure() )
    { return Error("The Tool Service could not be located!",sc); }
  if( 0 == toolSvc() )
    { return Error("IToolSvc* points to NULL!");}
  toolSvc()->addRef();
  ///
  log << MSG::DEBUG
      << " \tInput data    ='" << inputData  () << "'" << endreq 
      << " \tOutput  data  ='" << outputData () << "'" << endreq 
      << " \tDetector data ='" << detData    () << "'" << endreq ;
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** standard finalization method
 *  @return status code 
 */
// ============================================================================
StatusCode CaloAlgorithm::finalize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG 
      << " ==> Finalize the base class CaloAlgorithm " << endreq;
  ///
  if( 0 != toolSvc() ) { toolSvc()->release() ; m_toolSvc = 0 ; }
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** standard execution method
 *  @return status code 
 */
// ============================================================================
StatusCode CaloAlgorithm::execute() 
{
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG 
      << "==> Execute" << endreq;
  ///
  return StatusCode::SUCCESS ;
  ///
};

// ============================================================================
// The End 
// ============================================================================
