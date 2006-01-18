// $Id: LbAppInit.cpp,v 1.2 2006-01-18 12:28:26 gcorti Exp $
// Include files 
#include <string>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IProperty.h"

// local
#include "Kernel/LbAppInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LbAppInit
//
// 2005-12-21 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LbAppInit );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LbAppInit::LbAppInit( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_eventCounter(0),
    m_appName(""),
    m_appVersion("")
{

}
//=============================================================================
// Destructor
//=============================================================================
LbAppInit::~LbAppInit() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LbAppInit::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  IService* appMgr = svc<IService>( "ApplicationMgr" );
  SmartIF<IProperty> propMgr ( IID_IProperty, appMgr );
  std::string value ;
  sc = propMgr->getProperty( "EvtMax", value );
  if( sc.isFailure() ) {
    return Error( " Fatal error while retrieving Property EvtMax " );
    
  } else {
    m_eventMax = std::atoi(value.c_str()) ;
    always()
      << "=================================================================="
      << endmsg;
    if( -1 == m_eventMax ) {
      always() << "Requested to process all events on input file(s)" << endmsg; 
    }
    else {
      always() << "Requested to process " << m_eventMax << " events" << endmsg; 
    }
    always()
      << "=================================================================="
      << endmsg;
  }

  sc = propMgr->getProperty( "AppName", value );
  if( sc.isFailure() ) {
    return Error( " Fatal error while retrieving Property AppName " );
  } else {
    m_appName = value;
  }

  sc = propMgr->getProperty( "AppVersion", value );
  if( sc.isFailure() ) {
    return Error( " Fatal error while retrieving Property AppVersion " );
  } else {
    m_appVersion = value;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode LbAppInit::execute() {

  debug() << "==> Execute" << endmsg;
  ++m_eventCounter;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LbAppInit::finalize() {

  always()
    << "=================================================================="
    << endmsg;;
  always() << m_eventCounter << " events processed" << endmsg;
  always()
    << "=================================================================="
    << endmsg;

  if( (-1 != m_eventMax) && (m_eventMax != m_eventCounter) ) {
    warning()
      << "Should have processed " << m_eventMax << " events" << endmsg;
    warning()
      << "=================================================================="
      << endmsg;
  }
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
void LbAppInit::printEventRun( longlong event, int run ) 
{
  info() << "Evt " << event << ",  Run " << run
         << ",  Nr. in job = " << m_eventCounter << endmsg;
}

//=============================================================================
