// $Id: CaloTool.cpp,v 1.1.1.1 2001-11-01 13:21:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// GaudiKernel
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/Bootstrap.h"
// CaloGen 
#include "CaloGen/CaloException.h"
// CaloTools 
#include "CaloTools/CaloTool.h"

// ============================================================================
/** @file CaloTool.cpp
 * 
 * Implementation file for class : CaloTool
 *  
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date 01/11/2001 
 */
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param type tool type (useless)
 *  @param name tool name
 *  @param parent pointer to parent object (service, algorithm or tool)  
 */
// ============================================================================
CaloTool::CaloTool( const std::string& type   ,
                    const std::string& name   ,
                    const IInterface*  parent )
  : AlgTool     ( type , name , parent )
  , m_det       ( 0 ) 
  , m_chronoSvc ( 0 ) 
  , m_toolSvc   ( 0 )
  , m_svcLoc    ( 0 ) 
{
  /// check for servce locator 
  if   ( 0 != serviceLocator() ) { m_svcLoc = serviceLocator    () ; }
  else                           { m_svcLoc = Gaudi::svcLocator () ; }
  if( 0 != svcLoc() )            { svcLoc()->addRef()              ; }
  ///
};

// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode    CaloTool::initialize ()
{
  ///
  {
    /// reset Chrono & Stat service 
    if( 0 != chronoSvc() ) { chronoSvc()->release() ; m_chronoSvc = 0 ; }
    StatusCode sc = svcLoc()->service("ChronoStatSvc", m_chronoSvc );
    if( sc.isFailure() )
      { return Error("Could not locate ChronoStatSvc!", sc ); };
    if( 0 != chronoSvc() ) { chronoSvc()->addRef() ; }
  }
  ///
  { 
    /// reset Tool  service 
    if( 0 != toolSvc()   ) { toolSvc()->release() ; m_toolSvc = 0 ; }
    ///
    StatusCode sc = svcLoc()->service("ToolSvc", m_toolSvc );
    if( sc.isFailure() )
      { return Error("Could not locate ToolSvc!", sc ); };
    if( 0 != toolSvc()   ) { toolSvc()->addRef() ; }
  }
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode    CaloTool::finalize   ()
{
  /// release services 
  if( 0 != toolSvc    () ) { toolSvc  ()->release() ; m_toolSvc   = 0 ; }
  if( 0 != chronoSvc  () ) { chronoSvc()->release() ; m_chronoSvc = 0 ; }
  ///
  if( 0 != det        () ) { m_det = 0 ; }
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** destructor 
 */
// ============================================================================
CaloTool::~CaloTool(){};


// ============================================================================
/**  Create and (re)-throw the exception  
     @param msg    exception message 
     @param exc    (previous) exception of type GaudiException
     @param lvl    print level 
     @param sc     status code  
     @return       status code (fictive) 
*/
// ============================================================================
StatusCode CaloTool::Exception ( const std::string    & msg ,
                                 const GaudiException & exc ,
                                 const MSG::Level     & lvl ,
                                 const StatusCode     & sc  ) const   
{ 
  Error( msg , lvl );
  throw CaloException( msg , exc, sc );
  return  sc ;
};

// ============================================================================
/**  Create and (re)-throw the exception  
     @param msg    exception message 
     @param exc    (previous) exception of type GaudiException
     @param lvl    print level 
     @param sc     status code  
     @return       status code (fictive) 
*/
// ============================================================================
StatusCode CaloTool::Exception ( const std::string    & msg ,  
                                 const std::exception & exc , 
                                 const MSG::Level     & lvl ,
                                 const StatusCode     & sc  ) const   
{ 
  Error( msg , lvl );
  throw CaloException( msg+"("+exc.what()+")", sc );
  return  sc ;
};

// ============================================================================
/**  Create and throw the exception  
     @param msg    exception message 
     @param lvl    print level 
     @param sc     status code  
     @return       status code (fictive) 
*/
// ============================================================================
StatusCode CaloTool::Exception ( const std::string    & msg ,  
                                 const MSG::Level     & lvl ,
                                 const StatusCode     & sc  ) const 
{ 
  Error( msg , lvl );
  throw CaloException( msg , sc );
  return  sc ;
};

// ============================================================================
// The End
// ============================================================================
// ============================================================================
