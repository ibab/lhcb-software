// $Id: CaloTool.cpp,v 1.16 2004-02-17 11:55:55 ibelyaev Exp $
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
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Stat.h"
// LHCbKernel
#include "Kernel/CaloPrint.h"
// CaloKernel
#include "CaloKernel/CaloException.h"
#include "CaloKernel/CaloTool.h"

// ============================================================================
/** @file CaloTool.cpp
 * 
 *  Implementation file for class CaloTool
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 01/11/2001 
 */
// ============================================================================


// ============================================================================
/** Standard constructor
 *  @param type tool type (useless)
 *  @param name tool name
 *  @param parent pointer to parent object (service, algorithm or tool)  
 */
// ============================================================================
CaloTool::CaloTool
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool      ( type , name , parent )
  , m_det        ( 0  ) 
  , m_detName    ( "" )
{
  //
  StringProperty detectorName ( "Detector" , m_detName );
  if( 0 != parent ) 
  {    
    const IProperty* parentProperty =
      dynamic_cast<const IProperty*>( parent );
    if( 0 != parentProperty )
    { parentProperty->getProperty( &detectorName ); }
  }
  ///
  m_detName = detectorName.value();
  ///
  declareProperty( "Detector" , m_detName );
};
// ============================================================================

// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode    CaloTool::initialize ()
{
  ///
  { // initialize the base class 
    StatusCode sc = GaudiTool::initialize() ;
    if( sc.isFailure() ) 
    { return Error("Could not initialize base class GaudiTool" , sc ) ; }    
  }
  
  // reset detector 
  if( 0 != det() ) { m_det = 0 ; }
  if( !detName().empty() && 0 == detSvc() )
  { return Error("Detector name is '" + detName() + "' ,but detSvc()==0!" ) ; }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode    CaloTool::finalize   ()
{
  if( 0 != det        () ) { m_det = 0 ; }
  // finalize the base class 
  return GaudiTool::finalize() ;
};
// ============================================================================

// ============================================================================
/** destructor 
 */
// ============================================================================
CaloTool::~CaloTool(){};
// ============================================================================

// ============================================================================
// The End
// ============================================================================
