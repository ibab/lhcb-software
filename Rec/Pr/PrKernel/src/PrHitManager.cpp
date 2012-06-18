// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "PrKernel/PrHitManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrHitManager
//
// 2012-03-13 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PrHitManager )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrHitManager::PrHitManager( const std::string& type, 
                              const std::string& name,
                              const IInterface* parent) 
  : GaudiTool ( type, name , parent ) 
{
  declareInterface<PrHitManager>(this);
};
//=============================================================================
// Destructor
//=============================================================================
PrHitManager::~PrHitManager() { 
}
//=============================================================================
// Initialization
//=============================================================================
StatusCode PrHitManager::initialize(){
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  // invalidate measurements at the beginning of each event
  incSvc()->addListener(this, IncidentType::BeginEvent);
  while ( m_pool.size() < 10000 ) {
    PrHit* tmp = new PrHit();
    m_pool.push_back( tmp );
  }
  m_nextInPool = m_pool.begin();
  m_maxSize = 0;
  m_eventReady = false;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Finalization
//=============================================================================
StatusCode PrHitManager::finalize() {
  info() << "Maximum number of hits " << m_maxSize << endmsg;
  for ( PrHitLayers::iterator itS = m_layers.begin(); m_layers.end() != itS; ++itS ) {
    if ( NULL != *itS ) delete *itS;
  }
  return GaudiTool::finalize();
};
//=============================================================================
